/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: itkParticleOmegaGradientFunction.h,v $
   Date:      $Date: 2011/03/24 01:17:33 $
   Version:   $Revision: 1.2 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __itkParticleOmegaGradientFunction_h
#define __itkParticleOmegaGradientFunction_h

#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"

namespace itk
{

/**
 * \class ParticleOmegaGradientFunction
 *
 * This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.  The change in normals associated with the
 * position is also taken into account, providing an adaptive sampling of the
 * surface with respect to both position and extrinsic surface curvature.
 *
 */
template <class TGradientNumericType, unsigned int VDimension>
class ParticleOmegaGradientFunction
  : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
  /** Standard class typedefs. */
  typedef ParticleOmegaGradientFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
  itkTypeMacro( ParticleOmegaGradientFunction, ParticleEntropyGradientFunction );

  /** Inherit some parent typedefs. */
  typedef typename Superclass::GradientNumericType GradientNumericType;
  typedef typename Superclass::ParticleSystemType ParticleSystemType;
  typedef typename Superclass::VectorType VectorType;
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::GradientVectorType GradientVectorType;

  typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension>
    MeanCurvatureCacheType;

  typedef typename ParticleImageDomainWithCurvature<TGradientNumericType, VDimension>
    ::VnlMatrixType VnlMatrixType;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro( Dimension, unsigned int, VDimension );

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  inline virtual VectorType Evaluate( unsigned int a, unsigned int b, const ParticleSystemType* c,
                                      double& d ) const
  {
    double e;
    return this->Evaluate( a, b, c, d, e );
  }
  virtual VectorType Evaluate( unsigned int, unsigned int, const ParticleSystemType*,
                               double&, double & ) const;

  virtual void BeforeEvaluate( unsigned int, unsigned int, const ParticleSystemType* );

  inline virtual double Energy( unsigned int a, unsigned int b, const ParticleSystemType* c ) const
  {
    double d, e;
    this->Evaluate( a, b, c, d, e );
    return e;
  }

  inline double ComputeKappa( double mc, unsigned int d, double planeDist ) const
  {

    // NOTE: Should rethink this to scale nonlinearly with increasing distance
    // from the mean. This would be more consistent with the idea of
    // over/undersampling regions that are statistical outliers wrt curvature.
    // --jc

    
    //  double myKappa =  (1.0 + m_Rho * m_MeanCurvatureCache->operator[](
    //  this->GetDomainNumber())->operator[](idx) * (m_SamplesPerCurvature /
    //  twopi)) /
    //  ( m_SamplesPerCurvature * beta);
    double maxmc = m_MeanCurvatureCache->GetMeanCurvature( d )
                   + 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation( d );
    double minmc = m_MeanCurvatureCache->GetMeanCurvature( d )
                   - 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation( d );

    double kappa = 1.0 + m_Rho * ( mc - minmc ) / ( maxmc - minmc );

//    double MAG =  m_Rho;
//    double D   = 40;
//    if (fabs(planeDist) < D)
//      {
//      kappa = MAG + ((1.0 - MAG) / D) * fabs(planeDist);
//      }
//    else
//      {
//      kappa = 1.0;
//      }

    return kappa;
  }

  /** */
  virtual void AfterIteration() {  }

  /** */
  virtual void BeforeIteration()
  {
    //  this->ComputeKappaValues();
  }

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma( unsigned int, unsigned int,
                                const typename ParticleSystemType::PointVectorType &,
                                const std::vector<double> &,
                                const PointType &, double, double,
                                int &err, double &, unsigned int, unsigned int ) const;

  /** */
  //  void ComputeKappaValues();

  /** Access the cache of curvature-based weight values for each particle
      position. */
  void SetMeanCurvatureCache( MeanCurvatureCacheType* s )
  {    m_MeanCurvatureCache = s;  }
  MeanCurvatureCacheType* GetMeanCurvatureCache()
  {   return m_MeanCurvatureCache.GetPointer();  }
  const MeanCurvatureCacheType* GetMeanCurvatureCache() const
  {   return m_MeanCurvatureCache.GetPointer();  }

  /** Set/Get the parameters in the kappa function. */
  //  void SetGamma(double g)
  //  { m_Gamma = g; }
  //  void SetBeta(double g)
  //  { m_Beta = g; }
//   void SetCurvatureScale(double g)
//   { m_CurvatureScale = g; }
//  void SetSamplesPerCurvature(double g)
//  { m_SamplesPerCurvature = g; }
  void SetRho(double g)
  { m_Rho= g; }
  //  double GetGamma() const
  //  { return m_Gamma; }
  //  double GetBeta() const
  //  { return m_Beta; }
//   double GetCurvatureScale() const
//   { return m_CurvatureScale; }
//  double GetSamplesPerCurvature() const
//  { return m_SamplesPerCurvature; }
  double GetRho() const
  { return m_Rho; }


  virtual typename ParticleVectorFunction<VDimension>::Pointer Clone()
  {
    typename ParticleOmegaGradientFunction<TGradientNumericType, VDimension>::Pointer copy = ParticleOmegaGradientFunction<TGradientNumericType, VDimension>::New();
    copy->SetParticleSystem(this->GetParticleSystem());
    copy->m_Counter = this->m_Counter;
    copy->m_Rho = this->m_Rho;
    copy->m_avgKappa = this->m_avgKappa;
    copy->m_CurrentSigma = this->m_CurrentSigma;
    copy->m_CurrentWeights = this->m_CurrentWeights;
    copy->m_CurrentNeighborhood = this->m_CurrentNeighborhood;

    copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
    copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
    copy->m_FlatCutoff = this->m_FlatCutoff;
    copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

    copy->m_SpatialSigmaCache = this->m_SpatialSigmaCache;
    copy->m_MeanCurvatureCache = this->m_MeanCurvatureCache;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;
    copy->planePts = this->planePts;
    copy->spherePts = this->spherePts;
    copy->CToP = this->CToP;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
  }

protected:
  ParticleOmegaGradientFunction() :  m_Counter(0),
                                               m_Rho(1.0) {}
  virtual ~ParticleOmegaGradientFunction() {}
  void operator=(const ParticleOmegaGradientFunction &);
  ParticleOmegaGradientFunction(const ParticleOmegaGradientFunction &);
  typename MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;
  //  double m_Gamma;
  //  double m_Beta;
  //  double m_CurvatureScale;
  //  double m_SamplesPerCurvature;
  unsigned int m_Counter;
  double m_Rho;
  double m_avgKappa;
  
  double m_CurrentSigma;
  typename ParticleSystemType::PointVectorType m_CurrentNeighborhood;

  std::vector<double> m_CurrentWeights;

  std::vector < itk::Point<double, VDimension> > planePts;
  std::vector < itk::Point<double, VDimension> > spherePts;
  std::vector < double > CToP; // distance from sphere center to pos

  // SHIREEN
  float m_MaxMoveFactor;
  // end SHIREEN
};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleOmegaGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleOmegaGradientFunction.txx"
#endif

#include "itkParticleOmegaGradientFunction.txx"

#endif // ifndef __itkParticleOmegaGradientFunction_h
