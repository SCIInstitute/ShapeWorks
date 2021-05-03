#pragma once

#include <Eigen/Core>
#include <kernels/RBFKernel.h>
#include <vector>

/**
 * The supported RBF Evaluators
 */
enum class RBFEvaluatorMode { CPU, GPU, BATCH_GPU, OPENMP };


/**
 * Interface for classes that will evaluate RBFs. This interface 
 * should be used for CPU, OpenMP, and GPU based computation.
 */
class RBFEvaluator {
 public:  
  RBFEvaluator(){}
  virtual ~RBFEvaluator(){}
    
  virtual double evaluate(const RBFKernel *kernel, 
    const Eigen::Vector3d &evalPoint, const Eigen::MatrixXd &rbfPoints,
    const std::vector<double> &coefficients,
    const Eigen::Vector3d &linearCoefficients,
    double constant = 0) = 0;

  virtual std::vector<double> batchEvaluate(const RBFKernel *kernel, 
    const Eigen::MatrixXd &evalPoints, const Eigen::MatrixXd &rbfPoints,
    const std::vector<double> &coefficients,
    const Eigen::Vector3d &linearCoefficients,
    double constant = 0) = 0;
};
