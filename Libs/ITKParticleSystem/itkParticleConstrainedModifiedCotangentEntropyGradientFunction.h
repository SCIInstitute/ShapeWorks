/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleConstrainedModifiedCotangentEntropyGradientFunction.h,v $
  Date:      $Date: 2014/08/18 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: shireen $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleConstrainedModifiedCotangentEntropyGradientFunction_h
#define __itkParticleConstrainedModifiedCotangentEntropyGradientFunction_h

#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"

// SHIREEN
#include "Utils.h"
#include <fstream>
#include <math.h>
#include "itkMath.h"

#define NBHD_SIGMA_FACTOR 1.3
// end SHIREEN

namespace itk
{

/**
 * \class ParticleConstrainedModifiedCotangentEntropyGradientFunction
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
 * Modified potential only depend on a global sigma which defined the neighborhood of each particle
 * compared to a sigma per particle in case of Gaussian potential.
 *
 * This class also constrains the particle distribution according to the provided spheres (in or out) ?!!!
 *
 */
template <class TGradientNumericType, unsigned int VDimension>
class ParticleConstrainedModifiedCotangentEntropyGradientFunction
        : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
    /** Standard class typedefs. */
    typedef ParticleConstrainedModifiedCotangentEntropyGradientFunction Self;
    typedef SmartPointer<Self>  Pointer;
    typedef SmartPointer<const Self>  ConstPointer;
    typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
    itkTypeMacro( ParticleConstrainedModifiedCotangentEntropyGradientFunction, ParticleEntropyGradientFunction);

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

    virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystemType *);

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

    /** Estimate the best sigma for Parzen windowing.
     * This is almost twice the average distance to the nearest 6 neigbhors (hexagonal packing) */
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
    // Praful -- SHIREEN
     float m_MaxMoveFactor;
     // end Praful -- SHIREEN

};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleConstrainedModifiedCotangentEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.txx"
#endif

#include "itkParticleConstrainedModifiedCotangentEntropyGradientFunction.txx"

#endif
