#include "FreeFormConstraint.h"

namespace shapeworks {

bool FreeFormConstraint::isViolated(const Eigen::Vector3d &pt) const {
  if (constraintEval(pt) >= 0) {
    return false;
  } else {
    return true;
  }
}



}  // namespace shapeworks
