/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGradientDescentPositionOptimizer.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGradientDescentPositionOptimizer_h
#define __itkParticleGradientDescentPositionOptimizer_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkObject.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include <vector>
#include "vnl/vnl_vector_fixed.h"
#include "itkParticleVectorFunction.h"
#include "itkParticleImageDomainWithGradients.h"

namespace itk
{

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
template <class TGradientNumericType, unsigned int VDimension>
class ParticleGradientDescentPositionOptimizer : public Object
{
public:
  /** Standard class typedefs */
  typedef ParticleGradientDescentPositionOptimizer Self;
  typedef Object Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Type of the domain. */
  typedef ParticleImageDomainWithGradients<TGradientNumericType, VDimension> DomainType;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleGradientDescentPositionOptimizer, Object);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Particle System type is based on the dimensionality. */
  typedef ParticleSystem<VDimension> ParticleSystemType;

  /** Type of the gradient function. */
  typedef ParticleVectorFunction<VDimension> GradientFunctionType;
  
  /** Numerical vector type. */
  typedef typename GradientFunctionType::VectorType VectorType;

  /** Point Type */
  typedef typename ParticleSystemType::PointType PointType;

  /** Start the optimization. */
  void StartOptimization()
  {
    if (m_OptimizationMode == 0) { this->StartJacobiOptimization(); }
    else if (m_OptimizationMode == 2) { this->StartAdaptiveGaussSeidelOptimization();}
    else { this->StartGaussSeidelOptimization(); }
  }
  void StartJacobiOptimization();
  void StartGaussSeidelOptimization();
  void StartAdaptiveGaussSeidelOptimization();

  /** */
  void SetModeToGaussSeidel() { this->m_OptimizationMode = 1; }
  void SetModeToAdaptiveGaussSeidel() { this->m_OptimizationMode = 2; }
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
  itkGetObjectMacro(GradientFunction, GradientFunctionType);
  itkSetObjectMacro(GradientFunction, GradientFunctionType);
  
protected:
  ParticleGradientDescentPositionOptimizer();
  ParticleGradientDescentPositionOptimizer(const ParticleGradientDescentPositionOptimizer &);
  const ParticleGradientDescentPositionOptimizer &operator=(const ParticleGradientDescentPositionOptimizer &);
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  virtual ~ParticleGradientDescentPositionOptimizer() {};

private:
  typename ParticleSystemType::Pointer m_ParticleSystem;
  typename GradientFunctionType::Pointer m_GradientFunction;
  bool m_StopOptimization;
  unsigned int m_NumberOfIterations;
  unsigned int m_MaximumNumberOfIterations;
  double m_Tolerance;
  double m_TimeStep;
  int m_OptimizationMode;

  std::vector< std::vector<double> > m_TimeSteps;
  
  typename GradientFunctionType::Pointer DuplicateGradientFunction();

};


} // end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleGradientDescentPositionOptimizer+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleGradientDescentPositionOptimizer.txx"
#endif

#endif

