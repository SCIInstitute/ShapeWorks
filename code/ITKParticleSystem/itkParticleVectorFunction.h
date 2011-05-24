/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleVectorFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleVectorFunction_h
#define __itkParticleVectorFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/**
 * \class ParticleVectorFunction
 *
 * This is the base class defining the API for a function that takes a particle
 * system, domain, and location index as arguments and returns a vector-valued
 * result. 
 *
 */
template <unsigned int VDimension>
class ParticleVectorFunction : public LightObject
{
public:
 /** Standard class typedefs. */
  typedef ParticleVectorFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( ParticleVectorFunction, LightObject);

  /** Type of particle system. */
  typedef ParticleSystem<VDimension> ParticleSystemType;

  /** Type of vectors. */
  typedef vnl_vector_fixed<double, VDimension> VectorType;

  /** Method for object allocation through the factory. */
  //  itkNewMacro(Self);
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep, double &energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const =0;


  /** May be called by the solver class. */
  virtual void ResetBuffers() { }
  
  /** This method is called by a solver after each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void AfterIteration() { }

  /** This method is called by a solver before each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void BeforeIteration()  {}

  /** This method may be called to set up the state of the function object
      before a call to Evaluate.  It is necessary in order to initialize
      certain constants and variables that may be used for calculating the
      Energy as well as the Gradients.  Typically this is only necessary for
      the adaptive gradient descent algorithm.*/
  virtual void BeforeEvaluate(unsigned int , unsigned int, const ParticleSystemType *) {}
  
  /** Some subclasses may require a pointer to the particle system and its
      domain number.  These methods set/get those values. */
  virtual void SetParticleSystem( ParticleSystemType *p)
  {    m_ParticleSystem = p;  }
  virtual ParticleSystemType *GetParticleSystem() const
  { return m_ParticleSystem; }
  virtual void SetDomainNumber( unsigned int i)
  {    m_DomainNumber = i;  }
  virtual int GetDomainNumber() const
  { return m_DomainNumber; }

protected:
  ParticleVectorFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~ParticleVectorFunction() {}
  void operator=(const ParticleVectorFunction &);
  ParticleVectorFunction(const ParticleVectorFunction &);

  ParticleSystemType *m_ParticleSystem;
  unsigned int m_DomainNumber;  
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleVectorFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleVectorFunction.txx"
#endif

#endif
