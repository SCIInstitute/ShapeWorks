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
 * @class GradientDescentOptimizer
 * @brief Optimizes particle system positions using gradient descent
 *
 * This class optimizes a list of particle system positions with respect to a
 * specified energy function using a simple gradient descent strategy. A
 * function which computes the gradient of the function with respect to
 * particle position must be specified. The optimization performs Jacobi
 * updates (each particle position is changed as soon as its new position is
 * computed).
 */
class GradientDescentOptimizer {
 public:
  static constexpr unsigned int Dimension = 3;

  /// Numerical vector type
  using VectorType = VectorFunction::VectorType;

  /// Point type
  using PointType = ParticleSystem::PointType;

  /// Start the optimization
  void start_optimization() { start_adaptive_gauss_seidel_optimization(); }
  void start_adaptive_gauss_seidel_optimization();
  void set_early_stopping_config(const EarlyStoppingConfig& config);
  void initialize_early_stopping_score_function(const ParticleSystem* p);

  void augmented_lagrangian_constraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                        const double& maximum_update_allowed, size_t index);

  /// Stop the optimization
  /// @brief Sets a flag that aborts the start_optimization method after the current iteration
  void stop_optimization() { stop_optimization_ = true; }

  void abort_processing() {
    stop_optimization_ = true;
    abort_processing_ = true;
  }

  /// Get the number of iterations performed by the solver
  unsigned int get_number_of_iterations() const { return number_of_iterations_; }

  /// Set the number of iterations performed by the solver
  void set_number_of_iterations(unsigned int val) { number_of_iterations_ = val; }

  /// Set the verbosity level
  void set_verbosity(unsigned int val) { verbosity_ = val; }

  /// Get the verbosity level
  unsigned int get_verbosity() const { return verbosity_; }

  /// Get the time step parameter for the update
  /// @brief Each update is scaled by this value
  double get_time_step() const { return time_step_; }

  /// Set the time step parameter for the update
  void set_time_step(double val) { time_step_ = val; }

  /// Get the maximum number of iterations allowed
  unsigned int get_maximum_number_of_iterations() const { return max_iterations_; }

  /// Set the maximum number of iterations allowed
  void set_maximum_number_of_iterations(unsigned int val) { max_iterations_ = val; }

  /// Get the precision/tolerance of the solution
  double get_tolerance() const { return tolerance_; }

  /// Set the precision/tolerance of the solution
  void set_tolerance(double val) { tolerance_ = val; }

  /// Get the ParticleSystem modified by this optimizer
  ParticleSystem* get_particle_system() { return particle_system_.GetPointer(); }

  /// Get the ParticleSystem modified by this optimizer (const version)
  const ParticleSystem* get_particle_system() const { return particle_system_.GetPointer(); }

  /// Set the ParticleSystem modified by this optimizer
  void set_particle_system(ParticleSystem* val) { particle_system_ = val; }

  /// Get the gradient function used by this optimizer
  std::shared_ptr<VectorFunction> get_gradient_function() { return gradient_function_; }

  /// Get the gradient function used by this optimizer (const version)
  std::shared_ptr<const VectorFunction> get_gradient_function() const { return gradient_function_; }

  /// Set the gradient function used by this optimizer
  void set_gradient_function(std::shared_ptr<VectorFunction> val) { gradient_function_ = val; }

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

  /// Constructor
  GradientDescentOptimizer();

  /// Destructor
  ~GradientDescentOptimizer() = default;

 private:
  /// Disable copy constructor
  GradientDescentOptimizer(const GradientDescentOptimizer&) = delete;

  /// Disable assignment operator
  GradientDescentOptimizer& operator=(const GradientDescentOptimizer&) = delete;

  void reset_time_step_vectors();

  ParticleSystem::Pointer particle_system_;
  std::shared_ptr<VectorFunction> gradient_function_;
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
