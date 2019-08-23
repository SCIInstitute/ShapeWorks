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
#ifndef __itkParticleTwoCostFunction_h
#define __itkParticleTwoCostFunction_h

#include "itkPSMCostFunction.h"

namespace itk
{

/**
 * \class PSMTwoCostFunction
 *
 * This class combines two PSMCostFunction objects into a single
 * PSMCostFunction that is the linear combination of the two. The
 * second function (FunctionB) can be scaled relative to the first
 * function (SetRelativeGradientScaling), and evaluation of both
 * functions may be toggled on and off.
 *
 * An example of how to use this class is given by
 * itkPSMEntropyModelFilter, which combines an
 * itkPSMParticleEntropyFunction (FunctionA) with an
 * itkPSMShapeEntropyFunction (Function B) to perform simultaneous
 * maximization of entropy on the surfaces of a collection of shapes
 * with minimization of the entropy of the collection's distribution
 * in shape space.
 *
 * \sa itkPSMEntropyModelFilter
 *
 * \ingroup PSM
 * \ingroup PSMOptimization
 * \ingroup PSMCostFunctions
 * \author Josh Cates
 */
template <unsigned int VDimension>
class PSMTwoCostFunction : public PSMCostFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef PSMTwoCostFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef PSMCostFunction<VDimension> Superclass;
  itkTypeMacro( PSMTwoCostFunction, PSMCostFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  /** Vector type. */
  typedef typename Superclass::VectorType VectorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  virtual VectorType Evaluate(unsigned int idx, unsigned int d,
                              const ParticleSystemType *system,
                              double &maxmove) const;
  virtual VectorType Evaluate(unsigned int idx, unsigned int d,
			      const ParticleSystemType *system,
			      double &maxmove, double &energy) const;
  virtual double Energy(unsigned int idx, unsigned int d, 
			const ParticleSystemType *system) const;
  
  /** */
  virtual void BeforeEvaluate(unsigned int idx, unsigned int d,
                              const ParticleSystemType *system)
  {
    if (m_AOn) m_FunctionA->BeforeEvaluate(idx, d, system);    
    if (m_BOn) m_FunctionB->BeforeEvaluate(idx, d, system);
  }
  
  /** This method is called by a solver after each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void AfterIteration()
  {
    if (m_AOn) m_FunctionA->AfterIteration();
    if (m_BOn) m_FunctionB->AfterIteration();
  }
  
  /** This method is called by a solver before each iteration.  Subclasses may
      or may not implement this method.*/
  virtual void BeforeIteration()
  {
    if (m_AOn) m_FunctionA->BeforeIteration();
    if (m_BOn) m_FunctionB->BeforeIteration();
    
    m_AverageGradMagA = 0.0;
    m_AverageGradMagB = 0.0;
    
    m_AverageEnergyA = 0.0;
    m_AverageEnergyB = 0.0;
    
    m_Counter = 0.0;
  }
  
  /** Some subclasses may require a pointer to the particle system and its
      domain number.  These methods set/get those values. */
  virtual void SetParticleSystem( ParticleSystemType *p)
  {
    Superclass::SetParticleSystem(p);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetParticleSystem(p);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetParticleSystem(p);
  }

  void SetDomainNumber( unsigned int i)
  {
    Superclass::SetDomainNumber(i);
    if (m_FunctionA.GetPointer() != 0) m_FunctionA->SetDomainNumber(i);
    if (m_FunctionB.GetPointer() != 0) m_FunctionB->SetDomainNumber(i);
  }

  void SetFunctionA( PSMCostFunction<VDimension> *o)
  {
    m_FunctionA = o;
    m_FunctionA->SetDomainNumber(this->GetDomainNumber());
    m_FunctionA->SetParticleSystem(this->GetParticleSystem());
  }

  void SetFunctionB( PSMCostFunction<VDimension> *o)
  {
    m_FunctionB = o;
    m_FunctionB->SetDomainNumber(this->GetDomainNumber());
    m_FunctionB->SetParticleSystem(this->GetParticleSystem());
  }

  /** Turn each term on and off. */
  void SetAOn()   { m_AOn = true;  }
  void SetAOff() { m_AOn = false;  }
  void SetAOn(bool s)  {    m_AOn = s;  }
  bool GetAOn() const {  return m_AOn;  }
  void SetBOn()   {  m_BOn = true; }
  void SetBOff() {  m_BOn = false;  }
  void SetBOn(bool s) {  m_BOn = s;  }
  bool GetBOn() const { return m_BOn;  }

  /** The relative scaling scales the gradient B relative to A.  By default
      this value is 1.0. */
  void SetRelativeEnergyScaling(double r)
  {
    m_RelativeEnergyScaling = r;
  }
  double GetRelativeEnergyScaling() const
  {
    return m_RelativeEnergyScaling;
  }

  void SetRelativeGradientScaling(double r)
  {
    m_RelativeGradientScaling = r;
  }
  double GetRelativeGradientScaling() const
  {
    return m_RelativeGradientScaling;
  }
  
  double GetAverageGradMagA() const
  {
    if (m_Counter != 0.0) return m_AverageGradMagA / m_Counter;
    else return 0.0;
  }
  double GetAverageGradMagB() const
  {
    if (m_Counter != 0.0) return m_AverageGradMagB / m_Counter;
    else return 0.0;
  }

  double GetAverageEnergyA() const
  {
    if (m_Counter != 0.0) return m_AverageEnergyA / m_Counter;
    else return 0.0;
  }
  double GetAverageEnergyB() const
  {
    if (m_Counter != 0.0) return m_AverageEnergyB / m_Counter;
    else return 0.0;
  }
  
protected:
  PSMTwoCostFunction() : m_AOn(true), m_BOn(true), 
    m_RelativeGradientScaling(1.0), m_RelativeEnergyScaling(1.0) {}

  virtual ~PSMTwoCostFunction() {}
  void operator=(const PSMTwoCostFunction &);
  PSMTwoCostFunction(const PSMTwoCostFunction &);

  bool m_AOn;
  bool m_BOn;
  double m_RelativeGradientScaling;
  double m_RelativeEnergyScaling;
  double m_AverageGradMagA;
  double m_AverageGradMagB;
  double m_AverageEnergyA;
  double m_AverageEnergyB;
  double m_Counter;
  
  typename PSMCostFunction<VDimension>::Pointer m_FunctionA;
  typename PSMCostFunction<VDimension>::Pointer m_FunctionB;
};

} //end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMTwoCostFunction.hxx"
#endif


#endif
