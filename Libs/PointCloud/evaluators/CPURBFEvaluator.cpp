#include <CPURBFEvaluator.h>

#include <iostream>

#include <chrono>
#include <vector>

#define DEBUG false
#define B_DEBUG_ALLTIME false

/**
 * Evaluate the given point against all rbf points.
 * NOTE: The evaluation loop should be controlled by the number of points
 *       rather than the number of coeffs. In case of TPS, the coefficients
 *       vector is already augment by four coeffs (at the end of the vector)
 *       that encode the linear part of the tps function.
 */
double CPURBFEvaluator::evaluate(const RBFKernel *kernel, const Eigen::Vector3d &evalPoint,
                                 const Eigen::MatrixXd &rbfPoints,
                                 const std::vector<double> &coefficients,
                                 const Eigen::Vector3d &linearCoefficients,
                                 double constant) {

  double result = 0;
  for (int i = 0; i < rbfPoints.size(); i++) {
    const Eigen::Vector4d& point = rbfPoints.row(i);
    double d = sqrt(pow(point(0), 2) + pow(point(1), 2) + pow(point(2), 2));
    double kernelValue = kernel->compute(d);
    //std::cout << kernelValue << " ";
    double pointCoeff = coefficients[i];
    //std::cout << pointCoeff << " ";
    result += pointCoeff * kernelValue;
  }
  //std::cout << "\n";

  result += evalPoint.head<3>().dot(linearCoefficients) + constant;

  return result;
}

/**
 * Evaluate all given points against all rbf points.
 */
std::vector<double> CPURBFEvaluator::batchEvaluate(const RBFKernel *kernel,
    const Eigen::MatrixXd &evalPoints, const Eigen::MatrixXd &rbfPoints,
    const std::vector<double> &coefficients,
    const Eigen::Vector3d &linearCoefficients, double constant) {

  std::vector<double> results;
  for (int i = 0; i < evalPoints.size(); i++) {
    const Eigen::Vector4d& point = evalPoints.row(i);
    double result = this->evaluate(kernel, point.head<3>(), rbfPoints, coefficients,
      linearCoefficients, constant);
    results.push_back(result);
  }

  return results;
}
