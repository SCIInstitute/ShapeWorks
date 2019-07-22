/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMGradientDescentOptimizer_h
#define __itkPSMGradientDescentOptimizer_h

#include <vector>
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkPSMParticleSystem.h"
#include "itkPSMCostFunction.h"
#include "itkPSMImageDomainWithGradients.h"

namespace itk
{

/**
 * \class PSMGradientDescentOptimizer
 *
 * This class optimizes a list of particle system positions with respect to a
 * specified energy function using a simple gradient descent strategy.  A
 * function which computes the gradient of the function with respect to
 * particle position must be specified.  The optimization performs Jacobi
 * updates (each particle position is changed as soon as its new position is
 * computed).
 *
 */
template <class TGradientNumericType, unsigned int VDimension>
class PSMGradientDescentOptimizer : public Object
{
public:
  /** Standard class typedefs */
  typedef PSMGradientDescentOptimizer Self;
  typedef Object Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type of the domain. */
  typedef PSMImageDomainWithGradients<TGradientNumericType, VDimension> DomainType;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMGradientDescentOptimizer, Object);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Particle System type is based on the dimensionality. */
  typedef PSMParticleSystem<VDimension> ParticleSystemType;

  /** Type of the gradient function. */
  typedef PSMCostFunction<VDimension> CostFunctionType;
  
  /** Numerical vector type. */
  typedef typename CostFunctionType::VectorType VectorType;

  /** Point Type */
  typedef typename ParticleSystemType::PointType PointType;

  /** Start the optimization. */
  void StartOptimization()
  {
    if (m_OptimizationMode == 0)
    { this->StartJacobiOptimization(); }
    else if (m_OptimizationMode == 1)
    { this->StartGaussSeidelOptimization(); }
    else if (m_OptimizationMode == 2)
    { this->StartAdaptiveGaussSeidelOptimization(); }
  }
  void StartJacobiOptimization();
  void StartGaussSeidelOptimization();
  void StartAdaptiveGaussSeidelOptimization();

  /** */
  void SetModeToAdaptiveGaussSeidel() { this->m_OptimizationMode = 2; }
  void SetModeToGaussSeidel() { this->m_OptimizationMode = 1; }
  void SetModeToJacobi() { this->m_OptimizationMode = 0; }
  
  /** Stop the optimization.  This method sets a flag that aborts the
      StartOptimization method after the current iteration. */
  inline void StopOptimization()
  {    this->m_StopOptimization = true;  }

  /** Get/Set the number of iterations performed by the solver. */
  itkGetMacro(NumberOfIterations, unsigned int);
  itkSetMacro(NumberOfIterations, unsigned int);

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
  itkGetObjectMacro(CostFunction, CostFunctionType);
  itkSetObjectMacro(CostFunction, CostFunctionType);
  
protected:
  PSMGradientDescentOptimizer();
  PSMGradientDescentOptimizer(const PSMGradientDescentOptimizer &);
  const PSMGradientDescentOptimizer &operator=(const PSMGradientDescentOptimizer &);
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~PSMGradientDescentOptimizer() {};

private:
  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename CostFunctionType::Pointer m_CostFunction;
  bool m_StopOptimization;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  int m_OptimizationMode;

  std::vector< std::vector<double> > m_TimeSteps;
};

} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMGradientDescentOptimizer.hxx"
#endif 

#endif

