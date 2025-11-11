#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Function/VectorFunction.h"
#include "Libs/Optimize/Function/EarlyStop/EarlyStopping.h"
#include "EarlyStoppingConfig.h"
#include "ParticleSystem.h"

namespace shapeworks {

/**
 * \class ParticleGradientDescentPositionOptimizer
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
  using TGradientNumericType = float;
  constexpr static unsigned int VDimension = 3;

  /** Type of the domain. */
  typedef shapeworks::ImageDomainWithGradients<TGradientNumericType> DomainType;

  /** Dimensionality of the domain of the particle system. */
  static constexpr unsigned int Dimension = VDimension;

  /** Particle System type is based on the dimensionality. */
  typedef ParticleSystem ParticleSystemType;

  /** Type of the gradient function. */
  typedef VectorFunction GradientFunctionType;

  /** Numerical vector type. */
  typedef typename GradientFunctionType::VectorType VectorType;

  /** Point Type */
  typedef typename ParticleSystemType::PointType PointType;

  /** Start the optimization. */
  void StartOptimization() { this->StartAdaptiveGaussSeidelOptimization(); }
  void StartAdaptiveGaussSeidelOptimization();
  void SetEarlyStoppingConfig(const EarlyStoppingConfig& config);
  void InitializeEarlyStoppingScoreFunction(const ParticleSystemType* p);

  void AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt, const size_t& dom,
                                      const double& maximumUpdateAllowed, size_t index);

  /** Stop the optimization.  This method sets a flag that aborts the
      StartOptimization method after the current iteration. */
  inline void StopOptimization() { this->m_StopOptimization = true; }

  inline void AbortProcessing() {
    this->m_StopOptimization = true;
    this->m_AbortProcessing = true;
  }

  /** Get/Set the number of iterations performed by the solver. */
  unsigned int GetNumberOfIterations() const { return m_NumberOfIterations; }
  void SetNumberOfIterations(unsigned int val) { m_NumberOfIterations = val; }

  void SetVerbosity(unsigned int val) { m_verbosity = val; }
  unsigned int GetVerbosity() const { return m_verbosity; }

  /** Get/Set a time step parameter for the update.  Each update is simply
      scaled by this value. */
  double GetTimeStep() const { return m_TimeStep; }
  void SetTimeStep(double val) { m_TimeStep = val; }

  /** Get/Set the maximum iterations to allow this solver to use. */
  unsigned int GetMaximumNumberOfIterations() const { return m_MaximumNumberOfIterations; }
  void SetMaximumNumberOfIterations(unsigned int val) { m_MaximumNumberOfIterations = val; }

  /** Get/Set the precision of the solution. */
  double GetTolerance() const { return m_Tolerance; }
  void SetTolerance(double val) { m_Tolerance = val; }

  /** Get/Set the ParticleSystem modified by this optimizer. */
  ParticleSystemType* GetParticleSystem() { return m_ParticleSystem.GetPointer(); }
  const ParticleSystemType* GetParticleSystem() const { return m_ParticleSystem.GetPointer(); }
  void SetParticleSystem(ParticleSystemType* val) { m_ParticleSystem = val; }

  /** Get/Set the gradient function used by this optimizer. */
  GradientFunctionType* GetGradientFunction() { return m_GradientFunction.GetPointer(); }
  const GradientFunctionType* GetGradientFunction() const { return m_GradientFunction.GetPointer(); }
  void SetGradientFunction(GradientFunctionType* val) { m_GradientFunction = val; }

  /// Determines if this is an initialization (true) or an optimization (false)
  void SetInitializationMode(bool b) { m_initialization_mode = b; }

  /// Sets the number of iterations when we check for convergence
  void SetCheckIterations(size_t si) { m_check_iterations = si; }

  /// Sets the scaling factor at the beginning of the initialization
  void SetInitializationStartScalingFactor(double si) { m_initialization_start_scaling_factor = si; }

  /// Set a callback to be called after each iteration
  void SetIterationCallback(std::function<void()> callback) { m_iteration_callback = callback; }

  /// Get the current iteration callback
  std::function<void()> GetIterationCallback() const { return m_iteration_callback; }

  // Constructor and destructor are now public for std::shared_ptr usage
  GradientDescentOptimizer();
  virtual ~GradientDescentOptimizer(){};

 private:
  // Disable copy and assignment
  GradientDescentOptimizer(const GradientDescentOptimizer&) = delete;
  GradientDescentOptimizer& operator=(const GradientDescentOptimizer&) = delete;

  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename GradientFunctionType::Pointer m_GradientFunction;
  bool m_StopOptimization;
  bool m_AbortProcessing = false;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  std::vector<std::vector<double> > m_TimeSteps;
  unsigned int m_verbosity;
  EarlyStopping m_EarlyStopping;
  bool m_EarlyStoppingEnabled = false;
  bool m_EarlyStoppingScoreFunctionReady = false;

  // Adaptive Initialization variables
  bool m_initialization_mode = false;
  size_t m_check_iterations = 50;
  double m_initialization_start_scaling_factor;

  // Iteration callback
  std::function<void()> m_iteration_callback;

  void ResetTimeStepVectors();
};

}  // namespace shapeworks
