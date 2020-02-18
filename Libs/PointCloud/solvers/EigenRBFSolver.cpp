#include <EigenRBFSolver.h>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <cmath>

#define DEBUG false

std::vector<double> EigenRBFSolver::solveForCoefficients(
    const RBFKernel *kernel, const Eigen::MatrixXd &points) {
  std::cout << "\n***using solveForCoefficients for EigenRBFSolver.\n\n";

  size_t dim = size_t(points.size());
  Eigen::MatrixXd A(dim, dim);
  Eigen::VectorXd b(dim);
  for (size_t i = 0; i < dim; i++) {
      for (size_t j = 0; j < dim; j++) {
          double val = 0.;
          double magn = sqrt(pow(points(i,0) - points(j,0), 2) + pow(points(i,1) - points(j,1), 2) + pow(points(i,2) - points(j,2), 2));
          val = kernel->compute(magn);
          A(i, j) = val;
      }
  }
  for (int i = 0; i < dim; i++) {
      b(i) = points(i,3);
  }

  // solve the equation system based on QR factorization
  Eigen::VectorXd x;
  x = A.colPivHouseholderQr().solve(b);

  // populate solution
  std::vector<double> solution;
  for (size_t i = 0; i < dim; i++) {
    solution.push_back(x(i));
  }

  return solution;
}
