#include "PlaneConstraint.h"

#include <vtkPlane.h>
#include <vtkTriangle.h>

#include <iostream>

namespace shapeworks {

//-----------------------------------------------------------------------------
bool PlaneConstraint::isViolated(const Eigen::Vector3d &pt) const {
  double dist = planeNormal_.dot(pt - planePoint_);
  if (dist < 0) {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
void PlaneConstraint::print() const {
  std::cout << "normal " << planeNormal_.transpose() << " point " << planePoint_.transpose() << std::endl;
}

//-----------------------------------------------------------------------------
double PlaneConstraint::getOffset() { return offset_; }

//-----------------------------------------------------------------------------
void PlaneConstraint::setOffset(double offset) { offset_ = offset; }

//-----------------------------------------------------------------------------
double PlaneConstraint::constraintEval(const Eigen::Vector3d &pt) const {
  double val = -planeNormal_.dot(pt - planePoint_);
  return val;
}

//-----------------------------------------------------------------------------
void PlaneConstraint::updatePlaneFromPoints() {
  if (points().size() == 3) {
    planePoint_ = (points_[0] + points_[1] + points_[2]) / 3.0;
    vtkTriangle::ComputeNormal(points_[0].data(), points_[1].data(), points_[2].data(), planeNormal_.data());
  }
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPlane> PlaneConstraint::getVTKPlane() {
  auto plane = vtkSmartPointer<vtkPlane>::New();
  plane->SetNormal(getPlaneNormal().data());
  plane->SetOrigin(getPlanePoint().data());
  return plane;
}

}  // namespace shapeworks
