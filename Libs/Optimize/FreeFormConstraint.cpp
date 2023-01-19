#include "FreeFormConstraint.h"

#include <vtkContourFilter.h>
#include <vtkContourLoopExtraction.h>
#include <vtkFloatArray.h>
#include <vtkKdTreePointLocator.h>
#include <vtkPointData.h>

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
bool FreeFormConstraint::isSet() { return boundaries_.size() > 0 || (definitionPolyData_ != nullptr && painted_); }

//-----------------------------------------------------------------------------
void FreeFormConstraint::setPainted(bool painted) { painted_ = painted; }

//-----------------------------------------------------------------------------
void FreeFormConstraint::reset() {
  if (definitionPolyData_) {
    auto array = createFFCPaint(definitionPolyData_);
    array->FillComponent(0, 1.0);
  }
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
  applyToPolyData(mesh->getVTKMesh());
  setDefinition(mesh->getVTKMesh());
  computeBoundaries();


  std::vector<size_t> boundaryVerts;
  for (auto& boundary : boundaries_) {
    for (auto& pos : boundary) {
      boundaryVerts.push_back(pos);
    }
  }

  // Extract mesh vertices and faces
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;

  vtkSmartPointer<vtkPoints> points;
  points = mesh->getIGLMesh(V, F);
  auto values = vtkSmartPointer<vtkDoubleArray>::New();
  auto absvalues = setDistanceToBoundaryValueFieldForFFCs(values, points, boundaryVerts, getInOutScalars(), V, F);
  mesh->getVTKMesh()->GetPointData()->SetActiveScalars("value");
  std::vector<Eigen::Matrix3d> face_grad = mesh->setGradientFieldForFFCs(absvalues, V, F);
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

}  // namespace shapeworks
