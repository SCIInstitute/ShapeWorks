---
title: Libs/Optimize/ParticleSystem/itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h

---

# Libs/Optimize/ParticleSystem/itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleConstrainedModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleConstrainedModifiedCotangentEntropyGradientFunction.md)**  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[NBHD_SIGMA_FACTOR](../Files/itkParticleConstrainedModifiedCotangentEntropyGradientFunction_8h.md#define-nbhd-sigma-factor)**  |




## Macros Documentation

### define NBHD_SIGMA_FACTOR

```cpp
#define NBHD_SIGMA_FACTOR 1.3
```


## Source code

```cpp
#pragma once

#include "itkParticleEntropyGradientFunction.h"
#include "ParticleImageDomainWithGradients.h"
#include "ParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"

#include <fstream>
#include <math.h>
#include "itkMath.h"

#define NBHD_SIGMA_FACTOR 1.3

namespace itk
{

template <class TGradientNumericType, unsigned int VDimension>
class ParticleConstrainedModifiedCotangentEntropyGradientFunction
        : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
    typedef ParticleConstrainedModifiedCotangentEntropyGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
    itkTypeMacro( ParticleConstrainedModifiedCotangentEntropyGradientFunction, ParticleEntropyGradientFunction);

    typedef typename Superclass::GradientNumericType GradientNumericType;
    typedef typename Superclass::ParticleSystemType ParticleSystemType;
    typedef typename Superclass::VectorType VectorType;
    typedef typename Superclass::PointType PointType;
    typedef typename Superclass::GradientVectorType GradientVectorType;

    typedef typename shapeworks::ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType VnlMatrixType;

    itkNewMacro(Self);

    itkStaticConstMacro(Dimension, unsigned int, VDimension);

    inline virtual VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystemType *c,
                                       double& d) const
    {
        double e;
        return this->Evaluate(a, b, c, d, e);
    }
    virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
                                double&, double & ) const;

    virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystemType *);

    inline virtual double Energy(unsigned int a, unsigned int b, const ParticleSystemType *c) const
    {
        double d, e;
        this->Evaluate(a, b, c, d, e);
        return e;
    }

    virtual void AfterIteration()  {  }

    virtual void BeforeIteration()
    {
        // compute the global sigma for the whole particle system using its current status (particles position)
        if(m_RunStatus == 1 ) // initialization
        {
            // allow sigma to change during initialization only when particles are increased
            double oldSigma = m_GlobalSigma;
            this->EstimateGlobalSigma(this->GetParticleSystem());

            if(m_GlobalSigma >= 0.0) // make sure that we update the global sigma at the beginning (in the constructor, it is -1)
            {
                if ( (abs(oldSigma - m_GlobalSigma)/m_GlobalSigma) < 0.1)
                {
                    // not that much change, probably same number of particles, don't change the global sigma
                    m_GlobalSigma = oldSigma;
                }
            }
        }
        else // optimization
        {
            if(m_GlobalSigma < 0.0) // only compute sigma once during optimization
                this->EstimateGlobalSigma(this->GetParticleSystem());

        }

//        std::cout << "m_GlobalSigma: " << m_GlobalSigma << std::endl;
    }

    void EstimateGlobalSigma(const ParticleSystemType * system);


    inline double ComputeModifiedCotangent(double rij)const
    {
        if (rij > m_GlobalSigma)
            return 0.0;

        const double epsilon = 1.0e-6;

        double r     = itk::Math::pi_over_2 * rij/(m_GlobalSigma + epsilon) ;
        double cotan = cos(r)/(sin(r) + epsilon);
        double val   = cotan + r - itk::Math::pi_over_2;
        return val;
    }

    inline double ComputeModifiedCotangentDerivative(double rij)const
    {
        if (rij > m_GlobalSigma)
            return 0.0;

        const double epsilon = 1.0e-6;

        double r = itk::Math::pi_over_2 * rij/(m_GlobalSigma + epsilon) ;

        double sin_2 = 1.0 / ( pow(sin(r),2.0) + epsilon);
        double val   = -1.0 * (itk::Math::pi_over_2 * (1.0 / (m_GlobalSigma+epsilon) ) ) * (1 - sin_2);
        return val;
    }

    void SetDomainsPerShape(int i)
    { m_DomainsPerShape = i; }

    int GetDomainsPerShape() const
    { return m_DomainsPerShape; }

    // set/get run status : 1 for initialization and 2 for optimization
    void SetRunStatus(int i)
    { m_RunStatus = i; }

    int GetRunStatus() const
    { return m_RunStatus; }

    void SetDiagnosticsOutputPrefix( const std::string s)
    { m_diagnostics_prefix = s; }

    virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
    {
        typename ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::Pointer copy = ParticleConstrainedModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::New();
        copy->SetParticleSystem(this->GetParticleSystem());
        copy->m_Counter = this->m_Counter;
        copy->m_CurrentWeights = this->m_CurrentWeights;
        copy->m_CurrentNeighborhood = this->m_CurrentNeighborhood;
        copy->m_GlobalSigma = this->m_GlobalSigma;

        copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
        copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
        copy->m_FlatCutoff = this->m_FlatCutoff;
        copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

        copy->m_DomainNumber = this->m_DomainNumber;
        copy->m_ParticleSystem = this->m_ParticleSystem;

        copy->m_DomainsPerShape    = this->m_DomainsPerShape;
        copy->m_diagnostics_prefix = this->m_diagnostics_prefix;
        copy->m_RunStatus          = this->m_RunStatus;

        return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
    }

protected:
    ParticleConstrainedModifiedCotangentEntropyGradientFunction() :  m_Counter(0), m_DomainsPerShape(1), m_GlobalSigma(-1.0) {}
    virtual ~ParticleConstrainedModifiedCotangentEntropyGradientFunction() {}
    void operator=(const ParticleConstrainedModifiedCotangentEntropyGradientFunction &);
    ParticleConstrainedModifiedCotangentEntropyGradientFunction(const ParticleConstrainedModifiedCotangentEntropyGradientFunction &);

    unsigned int m_Counter;

    //double m_CurrentSigma;
    typename ParticleSystemType::PointVectorType m_CurrentNeighborhood;

    std::vector<double> m_CurrentWeights;

    std::string m_diagnostics_prefix;
    int m_DomainsPerShape;
    int m_RunStatus; // 1 for initialization and 2 for optimization

    double m_GlobalSigma;
    float m_MaxMoveFactor;

};

} //end namespace

#include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.txx"
```


-------------------------------

Updated on 2022-04-15 at 07:45:38 +0000