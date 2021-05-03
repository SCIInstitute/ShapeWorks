#pragma once

#include <solvers/RBFSolver.h>
#include <kernels/RBFKernel.h>
#include <vector>
#include <iostream>
#include <Eigen/Core>


class EigenRBFSolver : public RBFSolver {
 public:
  EigenRBFSolver(){}
  ~EigenRBFSolver(){}
  std::vector<double> solveForCoefficients(
    const RBFKernel *kernel, const Eigen::MatrixXd &points);
};
