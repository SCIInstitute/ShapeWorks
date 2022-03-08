#include "FreeFormConstraint.h"

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
  exclusionPoints_.clear();
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::applyToPolyData(vtkSmartPointer<vtkPolyData> polyData) {}

//-----------------------------------------------------------------------------
std::vector<Eigen::Vector3d> &FreeFormConstraint::getExclusionPoints() {
  if (!exclusionPoints_.empty()) {
    return exclusionPoints_;
  }

  computeInclusionExclusionPoints();

  return exclusionPoints_;
}

//-----------------------------------------------------------------------------
std::vector<Eigen::Vector3d> &FreeFormConstraint::getInclusionPoints() {
  if (!inclusionPoints_.empty()) {
    return inclusionPoints_;
  }

  computeInclusionExclusionPoints();

  return inclusionPoints_;
}

//-----------------------------------------------------------------------------
void FreeFormConstraint::computeInclusionExclusionPoints() {
  exclusionPoints_.clear();
  inclusionPoints_.clear();

  if (!definitionPolyData_) {
    return;
  }

  vtkFloatArray *array = vtkFloatArray::SafeDownCast(definitionPolyData_->GetPointData()->GetArray("ffc_paint"));
  if (!array) {
    return;
  }

  for (int i = 0; i < definitionPolyData_->GetNumberOfPoints(); i++) {
    Eigen::Vector3d pos;
    definitionPolyData_->GetPoint(i, pos.data());
    double value;
    array->GetTuple(i, &value);
    if (value > 0.0) {
      exclusionPoints_.push_back(pos);
    } else {
      inclusionPoints_.push_back(pos);
    }
  }
}

}  // namespace shapeworks
