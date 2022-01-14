---
title: Libs/Optimize/ParticleSystem/itkParticleModifiedCotangentEntropyGradientFunction.h

---

# Libs/Optimize/ParticleSystem/itkParticleModifiedCotangentEntropyGradientFunction.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleModifiedCotangentEntropyGradientFunction](../Classes/classitk_1_1ParticleModifiedCotangentEntropyGradientFunction.md)**  |




## Source code

```cpp
/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleModifiedCotangentEntropyGradientFunction.h,v $
  Date:      $Date: 2017/06/26 $
  Version:   $Revision: 4 $
  Author:    $Author: Praful $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleModifiedCotangentEntropyGradientFunction_h
#define __itkParticleModifiedCotangentEntropyGradientFunction_h

#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"

// PRATEEP
#include <fstream>
#include <math.h>
#include "itkMath.h"

// end PRATEEP

namespace itk
{

template <class TGradientNumericType, unsigned int VDimension>
class ParticleModifiedCotangentEntropyGradientFunction
        : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
    typedef ParticleModifiedCotangentEntropyGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
    itkTypeMacro( ParticleModifiedCotangentEntropyGradientFunction, ParticleEntropyGradientFunction);

    typedef typename Superclass::GradientNumericType GradientNumericType;
    typedef typename Superclass::ParticleSystemType ParticleSystemType;
    typedef typename Superclass::VectorType VectorType;
    typedef typename Superclass::PointType PointType;
    typedef typename Superclass::GradientVectorType GradientVectorType;

    typedef typename ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType VnlMatrixType;

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

    virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystemType *) {}

    inline virtual double Energy(unsigned int a, unsigned int b, const ParticleSystemType *c) const
    {
        double d, e;
        this->Evaluate(a, b, c, d, e);
        return e;
    }

    virtual void AfterIteration()  {  }

    virtual void BeforeIteration()
    {
       // not needed -- Praful
    }

    inline double ComputeModifiedCotangent(double rij, unsigned int d)const
    {
        if (rij > m_GlobalSigma[d])
            return 0.0;

        const double epsilon = 1.0e-6;
        rij += epsilon;
        double r     = itk::Math::pi_over_2 * rij/m_GlobalSigma[d] ;
        double cotan = cos(r)/sin(r);
        double val   = cotan + r - itk::Math::pi_over_2;
        double A     = -1.0 *itk::Math::pi_over_4 * m_GlobalSigma[d] - itk::Math::pi_over_4 * std::pow(epsilon, 2) / m_GlobalSigma[d] + itk::Math::pi_over_2 * epsilon;
        A -= (m_GlobalSigma[d]/itk::Math::pi_over_2) * std::log( std::sin(epsilon * itk::Math::pi_over_2 / m_GlobalSigma[d]) );
        val /= A;
        return val;
    }

    inline double ComputeModifiedCotangentDerivative(double rij, unsigned int d)const
    {
        if (rij > m_GlobalSigma[d])
            return 0.0;

        const double epsilon = 1.0e-6;
        rij += epsilon;
        double r     = itk::Math::pi_over_2 * rij/m_GlobalSigma[d] ;
        double sin_2 = 1.0 / pow(sin(r),2.0);
        double val   = (itk::Math::pi_over_2 / m_GlobalSigma[d]) * (1.0 - sin_2);

        double A     = -1.0 *itk::Math::pi_over_4 * m_GlobalSigma[d] - itk::Math::pi_over_4 * std::pow(epsilon, 2) / m_GlobalSigma[d] + itk::Math::pi_over_2 * epsilon;
        A -= (m_GlobalSigma[d]/itk::Math::pi_over_2) * std::log( std::sin(epsilon * itk::Math::pi_over_2 / m_GlobalSigma[d]) );

        val /= A;
        return val;
    }

    void ClearGlobalSigma()
    {
        m_GlobalSigma.clear();
    }

    void SetGlobalSigma(std::vector<double> i)
    { m_GlobalSigma = i; }

    void SetGlobalSigma(double i)
    { m_GlobalSigma.push_back(i); }

    virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
    {
        typename ParticleModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::Pointer copy = ParticleModifiedCotangentEntropyGradientFunction<TGradientNumericType, VDimension>::New();
        copy->SetParticleSystem(this->GetParticleSystem());
        copy->m_GlobalSigma = this->m_GlobalSigma;

        copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
        copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
        copy->m_FlatCutoff = this->m_FlatCutoff;
        copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

        copy->m_DomainNumber = this->m_DomainNumber;
        copy->m_ParticleSystem = this->m_ParticleSystem;

        return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
    }

protected:
    ParticleModifiedCotangentEntropyGradientFunction() {}
    virtual ~ParticleModifiedCotangentEntropyGradientFunction() {}
    void operator=(const ParticleModifiedCotangentEntropyGradientFunction &);
    ParticleModifiedCotangentEntropyGradientFunction(const ParticleModifiedCotangentEntropyGradientFunction &);

    std::vector<double> m_GlobalSigma;
};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleModifiedCotangentEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleModifiedCotangentEntropyGradientFunction.txx"
#endif

#include "itkParticleModifiedCotangentEntropyGradientFunction.txx"

#endif
```


-------------------------------

Updated on 2022-01-14 at 02:26:01 +0000
