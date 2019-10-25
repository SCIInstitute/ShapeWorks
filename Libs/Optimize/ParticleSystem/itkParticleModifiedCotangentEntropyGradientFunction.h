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

/**
 * \class ParticleModifiedCotangentEntropyGradientFunction
 *
 * This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.  The change in normals associated with the
 * position is also taken into account, providing an adaptive sampling of the
 * surface with respect to both position and extrinsic surface curvature.
 *
 * This function depend on modified cotangent potential as defined in Meyer's thesis
 * rather than Gaussian potential (Cates's thesis).
 *
 * Modified potential only depend on a global sigma based on target number of particles in the domain
 *
 *
 */
template <class TGradientNumericType, unsigned int VDimension>
class ParticleModifiedCotangentEntropyGradientFunction
        : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
    /** Standard class typedefs. */
    typedef ParticleModifiedCotangentEntropyGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
    itkTypeMacro( ParticleModifiedCotangentEntropyGradientFunction, ParticleEntropyGradientFunction);

    /** Inherit some parent typedefs. */
    typedef typename Superclass::GradientNumericType GradientNumericType;
    typedef typename Superclass::ParticleSystemType ParticleSystemType;
    typedef typename Superclass::VectorType VectorType;
    typedef typename Superclass::PointType PointType;
    typedef typename Superclass::GradientVectorType GradientVectorType;

    typedef typename ParticleImageDomainWithCurvature<TGradientNumericType, VDimension>
    ::VnlMatrixType VnlMatrixType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Dimensionality of the domain of the particle system. */
    itkStaticConstMacro(Dimension, unsigned int, VDimension);

    /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
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

    /** */
    virtual void AfterIteration()  {  }

    /** */

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
