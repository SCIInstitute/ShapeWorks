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
 *  Unless required by applicabcle law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMCostFunction_h
#define __itkPSMCostFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkPSMParticleSystem.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/**
 * \class PSMCostFunction
 *
 * This class defines the base API for functions in the PSM
 * optimization framework.  These are functions that take a particle
 * system, a particle system domain, and a location index as arguments
 * and return a vector-valued result. Subclasses of these functions
 * are used as metrics in optimizations of the positions of particles
 * in a particle system (PSMParticleSystem).
 *
 * A unique feature of these function objects is that they implement
 * several methods specific to the PSM optimizer framework, including
 * a BeforeIteration and AfterIteration method, which may be used to
 * apply constraints to gradients, such as constraining particles to
 * lie on surfaces.
 * 
 * \ingroup PSM
 * \ingroup PSMOptimization
 * \ingroup PSMCostFunctions
 * \author Josh Cates
 */
template <unsigned int VDimension>
class PSMCostFunction : public LightObject
{
public:
 /** Standard class typedefs. */
  typedef PSMCostFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef LightObject Superclass;
  itkTypeMacro( PSMCostFunction, LightObject);

  /** Type of particle system. */
  typedef PSMParticleSystem<VDimension> ParticleSystemType;

  /** Type of vectors. */
  typedef vnl_vector_fixed<double, VDimension> VectorType;

  /** Method for object allocation through the factory. */
  //  itkNewMacro(Self);
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Evaluate returns the vector-valued result of the function. The
      first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.
      The third argument is the index of the particle location within
      the given domain. */
  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep) const = 0;
  virtual VectorType Evaluate(unsigned int , unsigned int, const ParticleSystemType *,
                              double &maxtimestep, double &energy) const = 0;
  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const = 0;


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
  PSMCostFunction() : m_ParticleSystem(0), m_DomainNumber(0) {}
  virtual ~PSMCostFunction() {}
  void operator=(const PSMCostFunction &);
  PSMCostFunction(const PSMCostFunction &);

  ParticleSystemType *m_ParticleSystem;
  unsigned int m_DomainNumber;  
};

} //end namespace

#endif
