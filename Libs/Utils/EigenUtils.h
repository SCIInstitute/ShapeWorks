#pragma once

#include <Eigen/Core>

#include "Shapeworks.h"

namespace shapeworks {

/// Creates a shapeworks::Matrix33 from the given Eigen::MatrixXd (must also be 3x3)
Matrix33 createMatrix(const Eigen::MatrixXd &mat);

} // shapeworks
