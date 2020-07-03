/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleDualVectorFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleDualVectorFunction_h
#define __itkParticleDualVectorFunction_h

#include "itkLightObject.h"
#include "itkObjectFactory.h"
#include "itkWeakPointer.h"
#include "itkParticleSystem.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{

/**
 * \class ParticleDualVectorFunction
 *
 * This class combines the results of evaluating 2 ParticleVector functions and
 * presents the interface of a single function evaluation. Optionally, only the
 * first function can be used by calling SetLinkOff().
 *
 */
template <unsigned int VDimension>
class ParticleDualVectorFunction : public ParticleVectorFunction<VDimension>
{
public:
    /** Standard class typedefs. */
    typedef ParticleDualVectorFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleVectorFunction<VDimension> Superclass;
    itkTypeMacro( ParticleDualVectorFunction, ParticleVectorFunction);

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
                                double &maxmove) const
    {

        double maxA, maxB, maxC;
        maxA = 0; maxB = 0; maxC = 0;
        VectorType ansA; ansA.fill(0.0);
        VectorType ansB; ansB.fill(0.0);
        VectorType ansC; ansC.fill(0.0);

        const_cast<ParticleDualVectorFunction *>(this)->m_Counter = m_Counter + 1.0;

        // evaluate individual functions: A = surface energy, B = correspondence
        if (m_AOn == true)
        {
            ansA = m_FunctionA->Evaluate(idx, d, system, maxA);
            const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
        }

        if (m_BOn == true)
        {
            ansB = m_FunctionB->Evaluate(idx, d, system, maxB);
            const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
        }

        if( m_RelativeGradientScaling == 0.0)
        {
            ansB.fill(0.0);
            maxB = 0.0;
        }

        // get maxmove and predicted move for current configuration
        VectorType predictedMove; predictedMove.fill(0.0);
        if (m_BOn == true)
        {
            if (m_AOn == true)  // both A and B are active
            {
                if (maxB > maxA)
                {
                    maxmove = maxB;
                }
                else
                {
                    maxmove = maxA;
                }

                maxmove = maxA; // always driven by the sampling to decrease the senstivity to covariance regularization

                predictedMove = ansA + m_RelativeGradientScaling * ansB;

                return (predictedMove);
            }
            else // B is active, A is not active
            {
                maxmove = maxB;

                predictedMove = ansB;

                return (predictedMove);
            }
        }
        else  // only A is active
        {
            maxmove = maxA;
            return ansA;
        }
        maxmove = 0.0;
        return ansA;
    }

    virtual double EnergyA(unsigned int idx, unsigned int d, const ParticleSystemType *system) const
    {
        m_FunctionA->BeforeEvaluate(idx, d, system);
        double ansA = 0.0;
        if (m_AOn == true)
        {
            ansA = m_FunctionA->Energy(idx, d, system);
        }
        return ansA;
    }

    virtual double EnergyB(unsigned int idx, unsigned int d, const ParticleSystemType *system) const
    {
        m_FunctionB->BeforeEvaluate(idx, d, system);
        double ansB = 0.0;
        if (m_BOn == true)
        {
            ansB = m_FunctionB->Energy(idx, d, system);
        }
        ansB *= m_RelativeEnergyScaling;
        return ansB;
    }

    virtual double Energy(unsigned int idx, unsigned int d, const ParticleSystemType *system) const
    {
        double ansA = 0.0;
        double ansB = 0.0;
        double ansC = 0.0;
        double finalEnergy = 0.0;

        // evaluate individual functions: A = surface energy, B = correspondence
        if (m_AOn == true)
        {
            ansA = m_FunctionA->Energy(idx, d, system);
        }

        if (m_BOn == true)
        {
            ansB = m_FunctionB->Energy(idx, d, system);
        }

        if( m_RelativeEnergyScaling == 0)
        {
            ansB = 0.0;
        }

        // compute final energy for current configuration
        if (m_BOn == true)
        {
            if (m_AOn == true)  // both A and B are active
            {
                finalEnergy = ansA + m_RelativeEnergyScaling * ansB;
                return (finalEnergy);
            }
            else // B is active, A is not active
            {
                finalEnergy = ansB;
                return finalEnergy ;
            }
        }
        else  // only A is active
        {
            return ansA;
        }

        return 0.0;
    }

    virtual VectorType Evaluate(unsigned int idx, unsigned int d,
                                const ParticleSystemType *system,
                                double &maxmove, double &energy) const
    {
        double maxA = 0.0;
        double maxB = 0.0;
        double energyA = 0.0;
        double energyB = 0.0;
        VectorType ansA; ansA.fill(0.0);
        VectorType ansB; ansB.fill(0.0);

        const_cast<ParticleDualVectorFunction *>(this)->m_Counter = m_Counter + 1.0;

        // evaluate individual functions: A = surface energy, B = correspondence
        if (m_AOn == true)
        {
            ansA = m_FunctionA->Evaluate(idx, d, system, maxA, energyA);
            if (idx == 0 && d == 0) std::cerr << "ansA = " << ansA[0] << " " << ansA[1] << " " << ansA[2] << "\n";

            const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagA = m_AverageGradMagA + ansA.magnitude();
            const_cast<ParticleDualVectorFunction *>(this)->m_AverageEnergyA = m_AverageEnergyA + energyA;
        }

        if (m_BOn == true)
        {
            ansB = m_FunctionB->Evaluate(idx, d, system, maxB, energyB);
            if (idx == 0 && d == 0) std::cerr << "ansB = " << ansB[0] << " " << ansB[1] << " " << ansB[2] << "\n";

            const_cast<ParticleDualVectorFunction *>(this)->m_AverageGradMagB = m_AverageGradMagB + ansB.magnitude();
            const_cast<ParticleDualVectorFunction *>(this)->m_AverageEnergyB = m_AverageEnergyB + energyB;
        }

        if( m_RelativeEnergyScaling == 0.0)
        {
            energyB = 0.0;
            ansB.fill(0.0);
        }

        if (m_RelativeGradientScaling == 0.0)
        {
            maxB = 0.0;
            ansB.fill(0.0);
        }

        // compute final energy, maxmove and predicted move based on current configuration
        VectorType predictedMove; predictedMove.fill(0.0);
        if (m_BOn == true)
        {
            if (m_AOn == true)  // both A and B are active
            {
                if (maxB > maxA)
                {
                    maxmove = maxB;
                }
                else
                {
                    maxmove = maxA;
                }

                energy = energyA + m_RelativeEnergyScaling * energyB;

                maxmove = maxA; // always driven by the sampling to decrease the senstivity to covariance regularization

                predictedMove = ansA + m_RelativeGradientScaling * ansB;

                return (predictedMove);
            }
            else // only B is active, A is not active
            {
                maxmove = maxB;
                energy = energyB;
                predictedMove = ansB;

                return (predictedMove);
            }
        }
        else  // only A is active
        {
            maxmove = maxA;
            energy = energyA;
            return ansA;
        }
        maxmove = 0.0;
        return ansA;
    }

    virtual void BeforeEvaluate(unsigned int idx, unsigned int d,
                                const ParticleSystemType *system)
    {
        if (m_AOn == true)
        {
            m_FunctionA->BeforeEvaluate(idx, d, system);
        }

        if (m_BOn == true)
        {
            m_FunctionB->BeforeEvaluate(idx, d, system);
        }
    }

    /** This method is called by a solver after each iteration.  Subclasses may
      or may not implement this method.*/
    virtual void AfterIteration()
    {
        if (m_AOn) m_FunctionA->AfterIteration();
        if (m_BOn)
        {
            m_FunctionB->AfterIteration();

        }
    }

    /** This method is called by a solver before each iteration.  Subclasses may
      or may not implement this method.*/
    virtual void BeforeIteration()
    {
        if (m_AOn) m_FunctionA->BeforeIteration();
        if (m_BOn)
        {
            m_FunctionB->BeforeIteration();

        }
        m_AverageGradMagA = 0.0;
        m_AverageGradMagB = 0.0;
        m_AverageEnergyA = 0.0;
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

    void SetFunctionA( ParticleVectorFunction<VDimension> *o)
    {
        m_FunctionA = o;
        m_FunctionA->SetDomainNumber(this->GetDomainNumber());
        m_FunctionA->SetParticleSystem(this->GetParticleSystem());
    }

    ParticleVectorFunction<VDimension> * GetFunctionA()
    {
        return m_FunctionA.GetPointer();
    }

    ParticleVectorFunction<VDimension> * GetFunctionB()
    {
        return m_FunctionB.GetPointer();
    }

    void SetFunctionB( ParticleVectorFunction<VDimension> *o)
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

    virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
    {
        typename ParticleDualVectorFunction<VDimension>::Pointer copy = ParticleDualVectorFunction<VDimension>::New();
        copy->m_AOn = this->m_AOn;
        copy->m_BOn = this->m_BOn;

        copy->m_RelativeGradientScaling = this->m_RelativeGradientScaling;
        copy->m_RelativeEnergyScaling = this->m_RelativeEnergyScaling;
        copy->m_AverageGradMagA = this->m_AverageGradMagA;
        copy->m_AverageGradMagB = this->m_AverageGradMagB;
        copy->m_AverageEnergyA = this->m_AverageEnergyA;
        copy->m_AverageEnergyB = this->m_AverageEnergyB;
        copy->m_Counter = this->m_Counter;

        if (this->m_FunctionA) copy->m_FunctionA = this->m_FunctionA->Clone();
        if (this->m_FunctionB) copy->m_FunctionB = this->m_FunctionB->Clone();

        if (!copy->m_FunctionA) copy->m_AOn = false;
        if (!copy->m_FunctionB) copy->m_BOn = false;

        copy->m_DomainNumber = this->m_DomainNumber;
        copy->m_ParticleSystem = this->m_ParticleSystem;

        return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
    }

protected:
    ParticleDualVectorFunction() : m_AOn(true), m_BOn(false),
        m_RelativeGradientScaling(1.0),
        m_RelativeEnergyScaling(1.0)  {}

    virtual ~ParticleDualVectorFunction() {}
    void operator=(const ParticleDualVectorFunction &);
    ParticleDualVectorFunction(const ParticleDualVectorFunction &);

    bool m_AOn;
    bool m_BOn;
    double m_RelativeGradientScaling;
    double m_RelativeEnergyScaling;
    double m_AverageGradMagA;
    double m_AverageGradMagB;
    double m_AverageEnergyA;
    double m_AverageEnergyB;
    double m_Counter;

    typename ParticleVectorFunction<VDimension>::Pointer m_FunctionA;
    typename ParticleVectorFunction<VDimension>::Pointer m_FunctionB;
};


} //end namespace

#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleDualVectorFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
//# include "itkParticleDualVectorFunction.txx"
#endif

#endif
