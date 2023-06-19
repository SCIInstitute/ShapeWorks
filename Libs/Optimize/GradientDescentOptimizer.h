#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Function/VectorFunction.h"
#include "Libs/Optimize/Function/DualVectorFunction.h"
#include "ParticleSystem.h"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"

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
class GradientDescentOptimizer : public itk::Object {
 public:
  using TGradientNumericType = float;
  constexpr static unsigned int VDimension = 3;
  /** Standard class typedefs */
  typedef GradientDescentOptimizer Self;
  typedef itk::Object Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type of the domain. */
  typedef shapeworks::ImageDomainWithGradients<TGradientNumericType> DomainType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleGradientDescentPositionOptimizer, Object);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

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
  itkGetMacro(NumberOfIterations, unsigned int);
  itkSetMacro(NumberOfIterations, unsigned int);

  void SetVerbosity(unsigned int val) { m_verbosity = val; }

  unsigned int GetVerbosity() { return m_verbosity; }

  /** Get/Set a time step parameter for the update.  Each update is simply
      scaled by this value. */
  itkGetMacro(TimeStep, double);
  itkSetMacro(TimeStep, double);

  /** Get/Set the maximum iterations to allow this solver to use. */
  itkGetMacro(MaximumNumberOfIterations, unsigned int);
  itkSetMacro(MaximumNumberOfIterations, unsigned int);

  /** Get/Set the precision of the solution. */
  itkGetMacro(Tolerance, double);
  itkSetMacro(Tolerance, double);

  /** Get/Set the ParticleSystem modified by this optimizer. */
  itkGetObjectMacro(ParticleSystem, ParticleSystemType);
  itkSetObjectMacro(ParticleSystem, ParticleSystemType);

  /** Get/Set the gradient function used by this optimizer. */
  itkGetObjectMacro(GradientFunction, GradientFunctionType);
  itkSetObjectMacro(GradientFunction, GradientFunctionType);

  /// Determines if this is an initialization (true) or an optimization (false)
  void SetInitializationMode(bool b) { m_initialization_mode = b; }

  /// Sets the number of iterations when we check for convergence
  void SetCheckIterations(size_t si) { m_check_iterations = si; }

  /// Sets the scaling factor at the beginning of the initialization
  void SetInitializationStartScalingFactor(double si) { m_initialization_start_scaling_factor = si; }

 protected:
  GradientDescentOptimizer();
  GradientDescentOptimizer(const GradientDescentOptimizer&);
  const GradientDescentOptimizer& operator=(const GradientDescentOptimizer&);
  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }
  virtual ~GradientDescentOptimizer(){};

 private:
  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename GradientFunctionType::Pointer m_GradientFunction;
  bool m_StopOptimization;
  bool m_AbortProcessing = false;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  std::vector<std::vector<double> > m_TimeSteps;
  std::vector<std::vector<double> > m_TimeStepsForOffsets;
  
  unsigned int m_verbosity;

  // Adaptive Initialization variables
  bool m_initialization_mode = false;
  size_t m_check_iterations = 50;
  double m_initialization_start_scaling_factor;

  void ResetTimeStepVectors();
};

}  // namespace shapeworks
