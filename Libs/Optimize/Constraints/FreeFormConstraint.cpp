#include "FreeFormConstraint.h"

#include <vtkClipPolyData.h>
#include <vtkContourFilter.h>
#include <vtkContourLoopExtraction.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkFloatArray.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>
#include <vtkSelectPolyData.h>

#include "Libs/Common/Logging.h"

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

//-----------------------------------------------------------------------------
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

  if (!inoutPolyData_ && boundaries_.empty()) {  // nothing defined (old or new)
    auto array = createFFCPaint(polyData);
    if (!painted_) {
      array->FillComponent(0, 1.0);
    }
    return;
  }

  if (!inoutPolyData_ && !boundaries_.empty()) {  // legacy FFC definitions using only boundary/query point
    convertLegacyFFC(polyData);
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

  // Load the mesh on Geometry central
  {
    using namespace geometrycentral::surface;
    std::unique_ptr<SurfaceMesh> gcmesh;
    std::unique_ptr<VertexPositionGeometry> gcgeometry;
    std::tie(gcmesh, gcgeometry) = makeSurfaceMeshAndGeometry(V, F);
    HeatMethodDistanceSolver heatSolver(*gcgeometry, 1.0, true);

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

  // TODO: don't set local field since user can do this if needed, and the
  // function can be const. (return vector of Fields like Mesh::distance)
  mesh->setField("value", values, Mesh::Point);

  mesh->getVTKMesh()->GetPointData()->SetActiveScalars("value");
  std::vector<Eigen::Matrix3d> face_grad = setGradientFieldForFFCs(mesh, values, V, F);
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

//-----------------------------------------------------------------------------
void FreeFormConstraint::convertLegacyFFC(vtkSmartPointer<vtkPolyData> polyData) {
  if (polyData->GetPointData()->GetArray("ffc_paint")) {
    // clear out any old versions of the inout array or else they will get merged in
    polyData->GetPointData()->RemoveArray("ffc_paint");
  }

  auto boundaries = boundaries_;

  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  Mesh mesh(polyData);
  vtkSmartPointer<vtkPoints> points;
  points = mesh.getIGLMesh(V, F);

  for (size_t bound = 0; bound < boundaries.size(); bound++) {
    // Creating cutting loop
    vtkPoints* selectionPoints = vtkPoints::New();
    std::vector<size_t> boundaryVerts;

    // the locator is continuously rebuilt during this function, so don't use the cached version
    auto tmp_locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
    tmp_locator->SetDataSet(polyData);
    tmp_locator->BuildLocator();

    // Create path creator
    auto dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInputData(polyData);

    vtkIdType lastId = 0;
    for (size_t i = 0; i < boundaries[bound].size(); i++) {
      Eigen::Vector3d pt = boundaries[bound][i];
      double ptdob[3] = {pt[0], pt[1], pt[2]};
      vtkIdType ptid = tmp_locator->FindClosestPoint(ptdob);
      polyData->GetPoint(ptid, ptdob);

      // Add first point in boundary
      if (i == 0) {
        lastId = ptid;
        selectionPoints->InsertNextPoint(polyData->GetPoint(ptid));
        boundaryVerts.push_back(ptid);
      }

      // If the current and last vertices are different, then add all vertices in the path to the boundaryVerts list
      if (lastId != ptid) {
        selectionPoints->InsertNextPoint(polyData->GetPoint(ptid));
        boundaryVerts.push_back(ptid);
      }

      lastId = ptid;
    }

    if (selectionPoints->GetNumberOfPoints() < 3) {
      /// TODO: log an event that this occurred.  It's not really fatal as we may be applying to a mesh where this
      /// doesn't apply
      continue;
    }

    auto select = vtkSmartPointer<vtkSelectPolyData>::New();
    select->SetLoop(selectionPoints);
    select->SetInputData(polyData);
    select->GenerateSelectionScalarsOn();
    select->SetSelectionModeToLargestRegion();

    // Clipping mesh
    auto selectclip = vtkSmartPointer<vtkClipPolyData>::New();
    selectclip->SetInputConnection(select->GetOutputPort());
    selectclip->SetValue(0.0);
    selectclip->Update();

    auto* halfmesh = selectclip->GetOutput();

    if (halfmesh->GetNumberOfPoints() == 0) {
      /// TODO: log an event that this occurred.  It's not really fatal as we may be applying to a mesh where this
      /// doesn't apply
      continue;
    }

    vtkSmartPointer<vtkFloatArray> inout = computeInOutForFFCs(polyData, queryPoint_, halfmesh);
  }

  setDefinition(polyData);
  createInoutPolyData();
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkFloatArray> FreeFormConstraint::computeInOutForFFCs(vtkSmartPointer<vtkPolyData> polyData,
                                                                       Eigen::Vector3d query,
                                                                       vtkSmartPointer<vtkPolyData> halfmesh) {
  // Finding which half is in and which is out.
  bool halfmeshisin = true;
  auto* arr =
      vtkFloatArray::SafeDownCast(polyData->GetPointData()->GetArray("ffc_paint"));  // Check if an inout already exists

  // Create half-mesh tree
  auto kdhalf_locator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  kdhalf_locator->SetDataSet(halfmesh);
  kdhalf_locator->BuildLocator();

  // Create full-mesh tree
  auto pointLocator = vtkSmartPointer<vtkKdTreePointLocator>::New();
  pointLocator->SetDataSet(polyData);
  pointLocator->BuildLocator();

  // Checking which mesh is closer to the query point. Recall that the query point must not necessarely lie on the mesh,
  // so we check both the half mesh and the full mesh.
  double querypt[3] = {query[0], query[1], query[2]};

  vtkIdType halfi = kdhalf_locator->FindClosestPoint(querypt);
  vtkIdType fulli = pointLocator->FindClosestPoint(querypt);

  double halfp[3];
  halfmesh->GetPoint(halfi, halfp);

  double fullp[3];
  polyData->GetPoint(fulli, fullp);

  if (halfp[0] != fullp[0] || halfp[1] != fullp[1] || halfp[2] != fullp[2]) {
    halfmeshisin = false;  // If the closest point in halfmesh is not the closest point in fullmesh, then halfmesh is
                           // not the in mesh.
  }

  vtkSmartPointer<vtkFloatArray> inout = vtkSmartPointer<vtkFloatArray>::New();
  inout->SetNumberOfComponents(1);
  inout->SetNumberOfTuples(polyData->GetNumberOfPoints());
  inout->SetName("ffc_paint");

  for (vtkIdType i = 0; i < polyData->GetNumberOfPoints(); i++) {
    polyData->GetPoint(i, fullp);

    halfi = kdhalf_locator->FindClosestPoint(fullp);
    halfmesh->GetPoint(halfi, halfp);
    // std::cout << i <<  " (" << fullp[0] << " " << fullp[1] << " " << fullp[2] << ") " << halfi << " (" << halfp[0] <<
    // " " << halfp[1] << " " << halfp[2] << " )" << std::endl;
    bool ptinhalfmesh = false;
    if (fullp[0] == halfp[0] && fullp[1] == halfp[1] && fullp[2] == halfp[2]) {
      // If in halfmesh
      ptinhalfmesh = true;
    }
    // The relationship becomes an xor operation between halfmeshisin and ptinhalfmesh to determine whether each point
    // is in or out. Thus we set values for the scalar field.
    if (!halfmeshisin ^ ptinhalfmesh) {
      if (arr) {
        inout->SetValue(i, std::min<float>(1.0f, arr->GetValue(i)));
      } else {
        inout->SetValue(i, 1.);
      }
    } else {
      inout->SetValue(i, 0.);
    }
  }

  // Setting scalar field
  inout->SetName("ffc_paint");
  polyData->GetPointData()->AddArray(inout);

  return inout;
}

}  // namespace shapeworks
