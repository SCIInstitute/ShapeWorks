#pragma once

#include <functional>
#include <vector>

#include "EarlyStoppingConfig.h"
#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Function/EarlyStop/EarlyStopping.h"
#include "Libs/Optimize/Function/VectorFunction.h"
#include "ParticleSystem.h"

namespace shapeworks {

/**
 * @class ParticleGradientDescentPositionOptimizer
 *
 * This class optimizes a list of particle system positions with respect to a
 * specified energy function using a simple gradient descent strategy.  A
 * function which computes the gradient of the function with respect to
 * particle position must be specified.  The optimization performs Jacobi
 * updates (each particle position is changed as soon as its new position is
 * computed).
 *
 */
class GradientDescentOptimizer {
 public:
  static constexpr unsigned int Dimension = 3;

  /** Numerical vector type. */
  using VectorType = VectorFunction::VectorType;

  /** Point Type */
  using PointType = ParticleSystem::PointType;

  /** Start the optimization. */
  void StartOptimization() { StartAdaptiveGaussSeidelOptimization(); }
  void StartAdaptiveGaussSeidelOptimization();
  void SetEarlyStoppingConfig(const EarlyStoppingConfig& config);
  void InitializeEarlyStoppingScoreFunction(const ParticleSystem* p);

  void AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                      const double& maximumUpdateAllowed, size_t index);

  /** Stop the optimization.  This method sets a flag that aborts the
      StartOptimization method after the current iteration. */
  void StopOptimization() { stop_optimization_ = true; }

  void AbortProcessing() {
    stop_optimization_ = true;
    abort_processing_ = true;
  }

  /** Get/Set the number of iterations performed by the solver. */
  unsigned int GetNumberOfIterations() const { return number_of_iterations_; }
  void SetNumberOfIterations(unsigned int val) { number_of_iterations_ = val; }

  void SetVerbosity(unsigned int val) { verbosity_ = val; }
  unsigned int GetVerbosity() const { return verbosity_; }

  /** Get/Set a time step parameter for the update.  Each update is simply
      scaled by this value. */
  double GetTimeStep() const { return time_step_; }
  void SetTimeStep(double val) { time_step_ = val; }

  /** Get/Set the maximum iterations to allow this solver to use. */
  unsigned int GetMaximumNumberOfIterations() const { return max_iterations_; }
  void SetMaximumNumberOfIterations(unsigned int val) { max_iterations_ = val; }

  /** Get/Set the precision of the solution. */
  double GetTolerance() const { return tolerance_; }
  void SetTolerance(double val) { tolerance_ = val; }

  /** Get/Set the ParticleSystem modified by this optimizer. */
  ParticleSystem* GetParticleSystem() { return particle_system_.GetPointer(); }
  const ParticleSystem* GetParticleSystem() const { return particle_system_.GetPointer(); }
  void SetParticleSystem(ParticleSystem* val) { particle_system_ = val; }

  /** Get/Set the gradient function used by this optimizer. */
  VectorFunction* GetGradientFunction() { return gradient_function_.GetPointer(); }
  const VectorFunction* GetGradientFunction() const { return gradient_function_.GetPointer(); }
  void SetGradientFunction(VectorFunction* val) { gradient_function_ = val; }

  /// Determines if this is an initialization (true) or an optimization (false)
  void SetInitializationMode(bool b) { initialization_mode_ = b; }

  /// Sets the number of iterations when we check for convergence
  void SetCheckIterations(size_t si) { check_iterations_ = si; }

  /// Sets the scaling factor at the beginning of the initialization
  void SetInitializationStartScalingFactor(double si) { init_start_scaling_factor_ = si; }

  /// Set a callback to be called after each iteration
  void SetIterationCallback(std::function<void()> callback) { iteration_callback_ = callback; }

  /// Get the current iteration callback
  std::function<void()> GetIterationCallback() const { return iteration_callback_; }

  // Constructor and destructor are now public for std::shared_ptr usage
  GradientDescentOptimizer();
  ~GradientDescentOptimizer() = default;

 private:
  // Disable copy and assignment
  GradientDescentOptimizer(const GradientDescentOptimizer&) = delete;
  GradientDescentOptimizer& operator=(const GradientDescentOptimizer&) = delete;

  void ResetTimeStepVectors();

  ParticleSystem::Pointer particle_system_;
  VectorFunction::Pointer gradient_function_;
  bool stop_optimization_ = false;
  bool abort_processing_ = false;
  unsigned int number_of_iterations_ = 0;
  unsigned int max_iterations_ = 0;
  double tolerance_ = 0.0;
  double time_step_ = 1.0;
  std::vector<std::vector<double>> time_steps_;
  unsigned int verbosity_ = 0;
  EarlyStopping early_stopping_;
  bool early_stopping_enabled_ = false;
  bool early_stopping_score_ready_ = false;

  // Adaptive Initialization variables
  bool initialization_mode_ = false;
  size_t check_iterations_ = 50;
  double init_start_scaling_factor_ = 0.0;

  // Iteration callback
  std::function<void()> iteration_callback_;
};

}  // namespace shapeworks
