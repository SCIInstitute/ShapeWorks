#pragma once

#include <kernels/RBFKernel.h>
#include <vector>
#include <Eigen/Core>

class RBFSolver {
public:
  virtual ~RBFSolver(){}
  virtual std::vector<double> solveForCoefficients(
      const RBFKernel *kernel, const Eigen::MatrixXd &points) = 0;
};
