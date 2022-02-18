#include "PlaneConstraint.h"

#include <iostream>

namespace shapeworks {

bool PlaneConstraint::isViolated(const Eigen::Vector3d &pt) const {
  double dist = planeNormal_.dot(pt - planePoint_);
  if (dist < 0) {
    return true;
  }
  return false;
}

void PlaneConstraint::print() const {
  std::cout << "normal " << planeNormal_.transpose() << " point " << planePoint_.transpose() << std::endl;
}

double PlaneConstraint::constraintEval(const Eigen::Vector3d &pt) const {
  double val = -planeNormal_.dot(pt - planePoint_);
  return val;
}

}  // namespace shapeworks
