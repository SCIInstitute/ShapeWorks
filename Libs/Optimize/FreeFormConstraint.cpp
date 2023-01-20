#include "FreeFormConstraint.h"

#include <Logging.h>
#include <vtkContourFilter.h>
#include <vtkContourLoopExtraction.h>
#include <vtkFloatArray.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>

// libigl
#include <igl/cotmatrix.h>
#include <igl/exact_geodesic.h>
#include <igl/gaussian_curvature.h>
#include <igl/grad.h>
#include <igl/invert_diag.h>
#include <igl/massmatrix.h>
#include <igl/principal_curvature.h>

// geometry central
#include <geometrycentral/surface/heat_method_distance.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <geometrycentral/surface/surface_mesh_factories.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
bool FreeFormConstraint::readyForOptimize() const { return mesh_ != nullptr; }

bool FreeFormConstraint::isViolated(const Eigen::Vector3d& pt) const {
  if (constraintEval(pt) >= 0) {
    return false;
  } else {
    return true;
  }
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::setDefinition(vtkSmartPointer<vtkPolyData> polyData) {
  // ownership of the free form constraint is now held in the definitionPolyData_
  definitionPolyData_ = polyData;
  boundaries_.clear();
  inoutPolyData_ = nullptr;
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::applyToPolyData(vtkSmartPointer<vtkPolyData> polyData) {
  Mesh mesh(polyData);

  if (!inoutPolyData_) {
    auto array = createFFCPaint(polyData);
    if (!painted_) {
      array->FillComponent(0, 1.0);
    }
    return;
  }

  vtkFloatArray* inout = vtkFloatArray::SafeDownCast(inoutPolyData_->GetPointData()->GetArray("ffc_paint"));

  auto array = createFFCPaint(polyData);
  array->FillComponent(0, 1.0);

  // create a kd tree point locator
  vtkSmartPointer<vtkKdTreePointLocator> kdTree = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdTree->SetDataSet(inoutPolyData_);
  kdTree->BuildLocator();

  // iterate over all points in polyData and find the closest point in the kd tree
  for (int i = 0; i < polyData->GetNumberOfPoints(); i++) {
    double p[3];
    polyData->GetPoint(i, p);
    vtkIdType id = kdTree->FindClosestPoint(p);
    array->SetTuple(i, inout->GetTuple(id));
  }
}

//-----------------------------------------------------------------------------
std::vector<std::vector<Eigen::Vector3d>>& FreeFormConstraint::boundaries() { return boundaries_; }

//-----------------------------------------------------------------------------
void FreeFormConstraint::computeBoundaries() {
  if (!definitionPolyData_) {
    return;
  }

  boundaries_.clear();

  vtkFloatArray* array = vtkFloatArray::SafeDownCast(definitionPolyData_->GetPointData()->GetArray("ffc_paint"));
  if (!array) {
    return;
  }

  definitionPolyData_->GetPointData()->SetActiveScalars("ffc_paint");

  // must have both included and excluded
  double range[2];
  definitionPolyData_->GetPointData()->GetScalars()->GetRange(range);
  if (range[0] != 0 || range[1] != 1) {
    return;
  }

  // find valid query point
  for (int i = 0; i < definitionPolyData_->GetNumberOfPoints(); i++) {
    Eigen::Vector3d pos;
    definitionPolyData_->GetPoint(i, pos.data());
    double value;
    array->GetTuple(i, &value);
    if (value == 1.0) {  // find an included point
      queryPoint_ = pos;
    }
  }

  auto contour = vtkSmartPointer<vtkContourFilter>::New();
  contour->SetInputData(definitionPolyData_);
  contour->SetValue(0, 0.5);  // between 0 and 1
  contour->Update();

  auto loop = vtkSmartPointer<vtkContourLoopExtraction>::New();
  loop->SetInputConnection(contour->GetOutputPort());
  loop->SetOutputModeToPolylines();
  loop->Update();

  auto output = loop->GetOutput();

  for (int i = 0; i < output->GetNumberOfCells(); i++) {
    std::vector<Eigen::Vector3d> boundary;
    auto cell = output->GetCell(i);
    auto points = cell->GetPoints();
    for (int p = 0; p < cell->GetNumberOfPoints(); p++) {
      Eigen::Vector3d pos;
      points->GetPoint(p, pos.data());
      boundary.push_back(pos);
    }
    boundaries_.push_back(boundary);
  }
}

//-----------------------------------------------------------------------------
bool FreeFormConstraint::isSet() {
  bool is_set = getInoutPolyData() != nullptr || boundaries_.size() > 0 || (definitionPolyData_ != nullptr && painted_);
  return is_set;
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::setPainted(bool painted) { painted_ = painted; }

//-----------------------------------------------------------------------------
void FreeFormConstraint::reset() {
  if (definitionPolyData_) {
    auto array = createFFCPaint(definitionPolyData_);
    array->FillComponent(0, 1.0);
  }
  inoutPolyData_ = nullptr;
  boundaries_.clear();
  painted_ = false;
}

//-----------------------------------------------------------------------------
vtkFloatArray* FreeFormConstraint::createFFCPaint(vtkSmartPointer<vtkPolyData> polyData) {
  vtkFloatArray* array = vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetArray("ffc_paint"));
  if (!array) {
    array = vtkFloatArray::New();
    array->SetName("ffc_paint");
    array->SetNumberOfTuples(polyData->GetNumberOfPoints());
    array->FillComponent(0, 1.0);  // all included by default
    polyData->GetPointData()->AddArray(array);
  }
  polyData->Modified();
  return array;
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::createInoutPolyData() {
  // create a polydata with the inout field from definitionPolyData_, if there is one
  if (!definitionPolyData_) {
    return;
  }

  inoutPolyData_ = vtkSmartPointer<vtkPolyData>::New();
  inoutPolyData_->DeepCopy(definitionPolyData_);
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::computeGradientFields(std::shared_ptr<Mesh> mesh) {
  SW_DEBUG("Computing gradient fields for FFC");

  auto poly_data = mesh->getVTKMesh();
  applyToPolyData(mesh->getVTKMesh());
  setDefinition(mesh->getVTKMesh());
  createInoutPolyData();
  computeBoundaries();

  auto tmp_locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  tmp_locator->SetDataSet(mesh->getVTKMesh());
  tmp_locator->BuildLocator();
  std::vector<size_t> boundaryVerts;
  for (auto& boundary : boundaries_) {
    for (auto& pos : boundary) {
      vtkIdType ptid = tmp_locator->FindClosestPoint(pos.data());
      boundaryVerts.push_back(ptid);
    }
  }

  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  vtkSmartPointer<vtkPoints> points;
  points = mesh->getIGLMesh(V, F);
  auto values = vtkSmartPointer<vtkDoubleArray>::New();

  auto inout = getInOutScalars();

  values->SetNumberOfComponents(1);
  values->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  values->SetName("values");
  for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
    values->SetValue(i, INFINITY);
  }
  vtkSmartPointer<vtkDoubleArray> absvalues = vtkSmartPointer<vtkDoubleArray>::New();
  absvalues->SetNumberOfComponents(1);
  absvalues->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  absvalues->SetName("absvalues");

  // std::cout << "Loading eval values for FFCs in domain " << dom << std::endl;

  // debug
  Eigen::MatrixXd C(poly_data->GetNumberOfPoints(), 3);

  // Load the mesh on Geometry central
  {
    using namespace geometrycentral::surface;
    std::unique_ptr<SurfaceMesh> gcmesh;
    std::unique_ptr<VertexPositionGeometry> gcgeometry;
    std::tie(gcmesh, gcgeometry) = makeSurfaceMeshAndGeometry(V, F);
    HeatMethodDistanceSolver heatSolver(*gcgeometry);

    // Some vertices as source set
    std::vector<Vertex> sourceVerts;
    for (size_t i = 0; i < boundaryVerts.size(); i++) {
      sourceVerts.push_back(gcmesh->vertex(boundaryVerts[i]));
    }
    // Finds minimum distance to any boundary vertex from any other vertex
    for (Vertex v : sourceVerts) {
      VertexData<double> distToSource = heatSolver.computeDistance(v);
      // std::cout << distToSource[0] << " (" << values->GetValue(0) << ") ";
      for (size_t i = 0; i < points->GetNumberOfPoints(); i++) {
        if (distToSource[i] < std::abs(values->GetValue(i))) {
          absvalues->SetValue(i, distToSource[i]);
          if (inout->GetValue(i) == 1.) {
            values->SetValue(i, -distToSource[i]);
          } else {
            values->SetValue(i, distToSource[i]);
          }
        }
      }
    }
  }

  // std::cout << "Setting field" << std::endl;

  // TODO: don't set local field since user can do this if needed, and the
  // function can be const. (return vector of Fields like Mesh::distance)
  mesh->setField("value", values, Mesh::Point);

  mesh->getVTKMesh()->GetPointData()->SetActiveScalars("value");
  std::vector<Eigen::Matrix3d> face_grad = setGradientFieldForFFCs(mesh, absvalues, V, F);
}

//-----------------------------------------------------------------------------
vtkFloatArray* FreeFormConstraint::getInOutScalars() {
  if (!inoutPolyData_) {
    return nullptr;
  }
  vtkFloatArray* array = vtkFloatArray::SafeDownCast(inoutPolyData_->GetPointData()->GetArray("ffc_paint"));
  return array;
}

//-----------------------------------------------------------------------------
std::vector<Eigen::Matrix3d> FreeFormConstraint::setGradientFieldForFFCs(std::shared_ptr<Mesh> mesh,
                                                                         vtkSmartPointer<vtkDoubleArray> absvalues,
                                                                         Eigen::MatrixXd V, Eigen::MatrixXi F) {
  SW_DEBUG("Setting gradient field for FFCs");
  auto poly_data = mesh->getVTKMesh();
  // Definition of gradient field
  auto vf = vtkSmartPointer<vtkDoubleArray>::New();
  vf->SetNumberOfComponents(3);
  vf->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  vf->SetName("vf");

  // *Computing gradients for FFCs for each face
  // Compute gradient operator
  // std::cout << "Gradient preprocessing like in Karthik's code" << std::endl;
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);
  // Flattened version of libigl's gradient operator
  std::vector<Eigen::Matrix3d> face_grad;

  Eigen::MatrixXd grads(poly_data->GetNumberOfPoints(), 3);
  grads.fill(0.);

  // Flatten the gradient operator so we can quickly compute the gradient at a given point
  face_grad.resize(F.rows());
  size_t n_insertions = 0;
  for (int k = 0; k < G.outerSize(); k++) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(G, k); it; ++it) {
      const double val = it.value();
      const auto r = it.row();
      const auto c = it.col();

      const auto f = r % F.rows();
      const auto axis = r / F.rows();
      for (int i = 0; i < 3; i++) {
        if (F(f, i) == c) {
          face_grad[f](axis, i) = val;
          n_insertions++;
          break;
        }
      }
    }
  }

  // std::cout << "Computing grads" << std::endl;

  // Definition of gradient field for faces
  vtkSmartPointer<vtkDoubleArray> vff = vtkSmartPointer<vtkDoubleArray>::New();
  vff->SetNumberOfComponents(3);
  vff->SetNumberOfTuples(poly_data->GetNumberOfCells());
  vff->SetName("vff");

  // Computes grad vec for each face
  for (size_t i = 0; i < F.rows(); i++) {
    const Eigen::Vector3d vert_dists(absvalues->GetValue(F(i, 0)), absvalues->GetValue(F(i, 1)),
                                     absvalues->GetValue(F(i, 2)));

    // Compute gradient of geodesics
    const auto& G = face_grad[i];
    Eigen::Vector3d out_grad_eigen = (G * vert_dists).rowwise().sum();
    grads.row(F(i, 0)) += out_grad_eigen;
    grads.row(F(i, 1)) += out_grad_eigen;
    grads.row(F(i, 2)) += out_grad_eigen;
    vff->SetTuple3(i, out_grad_eigen(0), out_grad_eigen(1), out_grad_eigen(2));
    // out_grad_eigen *= geo_dist / out_grad_eigen.norm();
  }

  // Setting gradient field
  for (size_t i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    vf->SetTuple3(i, grads(i, 0), grads(i, 1), grads(i, 2));
  }

  // TODO: don't set field in this mesh since the user can do this if they want
  // it to be part of the mesh, and the function can therefore be const.
  mesh->setField("Gradient", vf, Mesh::Point);
  mesh->setField("vff", vff, Mesh::Face);

  return face_grad;
}

}  // namespace shapeworks
