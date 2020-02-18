#pragma once

#include <Eigen/Core>
#include <evaluators/RBFEvaluator.h>
#include <vector>

/**
 * CPU Implementation of the RBFEvaluator Interface.
 */
class CPURBFEvaluator : public RBFEvaluator{
 public:
  CPURBFEvaluator(){}
  ~CPURBFEvaluator(){}

  double evaluate(const RBFKernel *kernel, const Eigen::Vector3d &evalPoint,
      const Eigen::MatrixXd &rbfPoints,
      const std::vector<double> &coefficients,
      const Eigen::Vector3d &linearCoefficients,
      double constant = 0) override;

  std::vector<double> batchEvaluate(const RBFKernel *kernel, 
    const Eigen::MatrixXd &evalPoints, const Eigen::MatrixXd &rbfPoints,
    const std::vector<double> &coefficients,
    const Eigen::Vector3d &linearCoefficientsd,
    double constant = 0) override;
};
