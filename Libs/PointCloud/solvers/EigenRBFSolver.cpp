#include <EigenRBFSolver.h>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <cmath>

#define DEBUG false

/*
std::vector<double> EigenRBFSolver::solveForCoefficients(
    const RBFKernel *kernel, const Eigen::MatrixXd &points) {
  //std::cout << "\n***using solveForCoefficients for EigenRBFSolver.\n\n";

  size_t dim = size_t(points.rows());
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
}*/

std::vector<double> EigenRBFSolver::solveForCoefficients(
    const RBFKernel *kernel, const Eigen::MatrixXd &points) {
  //std::cout << "\n***using solveForCoefficients for EigenRBFSolver.\n\n";

  size_t dim = size_t(points.rows()) + 4;
  Eigen::MatrixXd A(dim, dim);
  Eigen::VectorXd b(dim);
  for (size_t i = 0; i < dim; i++) {
      for (size_t j = 0; j < dim; j++) {
          double val = 0.;
          if (i < dim - 4 && j < dim - 4) {
              double magn = sqrt(pow(points(i,0) - points(j,0), 2) + pow(points(i,1) - points(j,1), 2) + pow(points(i,2) - points(j,2), 2));
              val = kernel->compute(magn);
          } else if (j == dim-4 || i == dim-4){
              val = 1.;
          } else if (i < dim - 4) {
              val = points(i,j + 3 - dim);
          } else if (j < dim - 4) {
              val = points(j,i + 3 - dim);
          }

          A(i, j) = val;
      }
  }
  for (int i = 0; i < dim; i++) {
      b(i) = (i >= dim - 4) ? 0. : points(i,3);
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
