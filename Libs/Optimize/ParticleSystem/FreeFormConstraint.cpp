#include "FreeFormConstraint.h"

#include <vtkContourFilter.h>
#include <vtkContourLoopExtraction.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
bool FreeFormConstraint::isViolated(const Eigen::Vector3d &pt) const {
  if (constraintEval(pt) >= 0) {
    return false;
  } else {
    return true;
  }
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::setDefinition(vtkSmartPointer<vtkPolyData> polyData) {
  definitionPolyData_ = polyData;
  boundaries_.clear();
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::applyToPolyData(vtkSmartPointer<vtkPolyData> polyData) {}

//-----------------------------------------------------------------------------
std::vector<std::vector<Eigen::Vector3d> > &FreeFormConstraint::boundaries() {
  if (boundaries_.empty()) {
    computeBoundaries();
  }
  return boundaries_;
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::computeBoundaries() {
  boundaries_.clear();
  if (!definitionPolyData_) {
    return;
  }

  vtkFloatArray *array = vtkFloatArray::SafeDownCast(definitionPolyData_->GetPointData()->GetArray("ffc_paint"));
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
    if (value == 0.0) {
      queryPoint_ = pos;
    }
  }

  auto contour = vtkSmartPointer<vtkContourFilter>::New();
  contour->SetInputData(definitionPolyData_);
  contour->SetValue(0, 0.5);
  contour->Update();

  auto loop = vtkSmartPointer<vtkContourLoopExtraction>::New();
  loop->SetInputConnection(contour->GetOutputPort());
  loop->SetOutputModeToPolylines();
  loop->Update();

  auto output = loop->GetOutput();

  std::cerr << "number of cells = " << output->GetNumberOfCells() << "\n";
  for (int i = 0; i < output->GetNumberOfCells(); i++) {
    std::vector<Eigen::Vector3d> boundary;
    auto cell = output->GetCell(i);
    std::cerr << "number of points = " << cell->GetNumberOfPoints() << "\n";
    auto points = cell->GetPoints();
    for (int p = 0; p < cell->GetNumberOfPoints(); p++) {
      Eigen::Vector3d pos;
      points->GetPoint(p, pos.data());
      boundary.push_back(pos);
    }
    boundaries_.push_back(boundary);
  }
}

}  // namespace shapeworks
