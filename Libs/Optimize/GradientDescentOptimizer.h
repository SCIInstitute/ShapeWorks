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
  void start_optimization() { start_adaptive_gauss_seidel_optimization(); }
  void start_adaptive_gauss_seidel_optimization();
  void set_early_stopping_config(const EarlyStoppingConfig& config);
  void initialize_early_stopping_score_function(const ParticleSystem* p);

  void augmented_lagrangian_constraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                        const double& maximum_update_allowed, size_t index);

  /** Stop the optimization.  This method sets a flag that aborts the
      start_optimization method after the current iteration. */
  void stop_optimization() { stop_optimization_ = true; }

  void abort_processing() {
    stop_optimization_ = true;
    abort_processing_ = true;
  }

  /** Get/Set the number of iterations performed by the solver. */
  unsigned int get_number_of_iterations() const { return number_of_iterations_; }
  void set_number_of_iterations(unsigned int val) { number_of_iterations_ = val; }

  void set_verbosity(unsigned int val) { verbosity_ = val; }
  unsigned int get_verbosity() const { return verbosity_; }

  /** Get/Set a time step parameter for the update.  Each update is simply
      scaled by this value. */
  double get_time_step() const { return time_step_; }
  void set_time_step(double val) { time_step_ = val; }

  /** Get/Set the maximum iterations to allow this solver to use. */
  unsigned int get_maximum_number_of_iterations() const { return max_iterations_; }
  void set_maximum_number_of_iterations(unsigned int val) { max_iterations_ = val; }

  /** Get/Set the precision of the solution. */
  double get_tolerance() const { return tolerance_; }
  void set_tolerance(double val) { tolerance_ = val; }

  /** Get/Set the ParticleSystem modified by this optimizer. */
  ParticleSystem* get_particle_system() { return particle_system_.GetPointer(); }
  const ParticleSystem* get_particle_system() const { return particle_system_.GetPointer(); }
  void set_particle_system(ParticleSystem* val) { particle_system_ = val; }

  /** Get/Set the gradient function used by this optimizer. */
  VectorFunction* get_gradient_function() { return gradient_function_.GetPointer(); }
  const VectorFunction* get_gradient_function() const { return gradient_function_.GetPointer(); }
  void set_gradient_function(VectorFunction* val) { gradient_function_ = val; }

  /// Determines if this is an initialization (true) or an optimization (false)
  void set_initialization_mode(bool b) { initialization_mode_ = b; }

  /// Sets the number of iterations when we check for convergence
  void set_check_iterations(size_t si) { check_iterations_ = si; }

  /// Sets the scaling factor at the beginning of the initialization
  void set_initialization_start_scaling_factor(double si) { init_start_scaling_factor_ = si; }

  /// Set a callback to be called after each iteration
  void set_iteration_callback(std::function<void()> callback) { iteration_callback_ = callback; }

  /// Get the current iteration callback
  std::function<void()> get_iteration_callback() const { return iteration_callback_; }

  // Constructor and destructor are now public for std::shared_ptr usage
  GradientDescentOptimizer();
  ~GradientDescentOptimizer() = default;

 private:
  // Disable copy and assignment
  GradientDescentOptimizer(const GradientDescentOptimizer&) = delete;
  GradientDescentOptimizer& operator=(const GradientDescentOptimizer&) = delete;

  void reset_time_step_vectors();

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
