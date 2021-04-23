/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleCurvatureEntropyGradientFunction.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleCurvatureEntropyGradientFunction_h
#define __itkParticleCurvatureEntropyGradientFunction_h

#include "itkParticleEntropyGradientFunction.h"
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleImageDomainWithCurvature.h"
#include "itkParticleMeanCurvatureAttribute.h"
#include "itkCommand.h"
#include "itkParticleSurfaceNeighborhood.h"

namespace itk
{

/**
 * \class ParticleCurvatureEntropyGradientFunction
 *
 * This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.  The change in normals associated with the
 * position is also taken into account, providing an adaptive sampling of the
 * surface with respect to both position and extrinsic surface curvature.
 *
 */
template <class TGradientNumericType, unsigned int VDimension>
class ParticleCurvatureEntropyGradientFunction
  : public ParticleEntropyGradientFunction<TGradientNumericType, VDimension>
{
public:
 /** Standard class typedefs. */
  typedef ParticleCurvatureEntropyGradientFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef ParticleEntropyGradientFunction<TGradientNumericType, VDimension> Superclass;
  itkTypeMacro( ParticleCurvatureEntropyGradientFunction, ParticleEntropyGradientFunction);

  /** Inherit some parent typedefs. */
  typedef typename Superclass::GradientNumericType GradientNumericType;
  typedef typename Superclass::ParticleSystemType ParticleSystemType;
  typedef typename Superclass::VectorType VectorType;
  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::GradientVectorType GradientVectorType;
  
  typedef ParticleMeanCurvatureAttribute<TGradientNumericType, VDimension> MeanCurvatureCacheType;

  typedef typename ParticleImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType VnlMatrixType;

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
  
  inline double ComputeKappa(double mc, unsigned int d) const
  {
    // Miriah code
    //  double myKappa =  (1.0 + m_Rho * m_MeanCurvatureCache->operator[](
    //  this->GetDomainNumber())->operator[](idx) * (m_SamplesPerCurvature /
    //  twopi)) /
    //  ( m_SamplesPerCurvature * beta);

    // First code
    double maxmc = m_MeanCurvatureCache->GetMeanCurvature(d)
      + 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation(d);
    double minmc = m_MeanCurvatureCache->GetMeanCurvature(d)
      - 2.0 * m_MeanCurvatureCache->GetCurvatureStandardDeviation(d);
    return 1.0 +  m_Rho * (mc - minmc) / (maxmc - minmc);

    // New sigmoid function



  }
  
  /** */
  virtual void AfterIteration()  {  }

  /** */
  virtual void BeforeIteration()
  {
    //  this->ComputeKappaValues();
  }

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma( unsigned int idx, unsigned int dom, const ParticleDomain *domain,
                                const PointType &pos, double initial_sigma,  double precision,  int &err, double &avgKappa) const;

  /** */
  //  void ComputeKappaValues();
  
  /** Access the cache of curvature-based weight values for each particle
      position. */
  void SetMeanCurvatureCache( MeanCurvatureCacheType *s)
  {    m_MeanCurvatureCache = s;  }
  MeanCurvatureCacheType *GetMeanCurvatureCache()
  {   return  m_MeanCurvatureCache.GetPointer();  }
  
  const MeanCurvatureCacheType *GetMeanCurvatureCache() const
  {   return  m_MeanCurvatureCache.GetPointer();  }
 
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
    typename ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>::Pointer copy = ParticleCurvatureEntropyGradientFunction<TGradientNumericType, VDimension>::New();
    copy->SetParticleSystem(this->GetParticleSystem());
    copy->m_Counter = this->m_Counter;
    copy->m_Rho = this->m_Rho;
    copy->m_avgKappa = this->m_avgKappa;
    copy->m_CurrentSigma = this->m_CurrentSigma;
    copy->m_CurrentNeighborhood = this->m_CurrentNeighborhood;

    copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
    copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
    copy->m_FlatCutoff = this->m_FlatCutoff;
    copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

    copy->m_SpatialSigmaCache = this->m_SpatialSigmaCache;
    copy->m_MeanCurvatureCache = this->m_MeanCurvatureCache;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    return (typename ParticleVectorFunction<VDimension>::Pointer)copy;
  }

protected:
  ParticleCurvatureEntropyGradientFunction() :  m_Counter(0),
                                               m_Rho(1.0) {}
  virtual ~ParticleCurvatureEntropyGradientFunction() {}
  void operator=(const ParticleCurvatureEntropyGradientFunction &);
  ParticleCurvatureEntropyGradientFunction(const ParticleCurvatureEntropyGradientFunction &);
  typename MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;
  //  double m_Gamma;
  //  double m_Beta;
  //  double m_CurvatureScale;
  //  double m_SamplesPerCurvature;
  unsigned int m_Counter;
  double m_Rho;
  
  double m_avgKappa;
  
  double m_CurrentSigma;
  struct CrossDomainNeighborhood {
    ParticlePointIndexPair<3> pi_pair;
    double weight;
    double distance;
    int dom;

    CrossDomainNeighborhood(const ParticlePointIndexPair<3>& pi_pair_,
                            double weight_,
                            double distance_,
                            int dom_) : pi_pair(pi_pair_), weight(weight_), distance(distance_), dom(dom_) {

    }
  };
  std::vector<CrossDomainNeighborhood> m_CurrentNeighborhood;
  void UpdateNeighborhood(const PointType& pos, int idx, int d, double radius, const ParticleSystemType* system) {
    const auto domains_per_shape = system->GetDomainsPerShape();
    const auto domain_base = d / domains_per_shape;
    const auto domain_sub = d % domains_per_shape;

    m_CurrentNeighborhood.clear();
    for(int offset=0; offset<domains_per_shape; offset++) {
      const auto domain_t = domain_base*domains_per_shape + offset;
      const auto neighborhood_ = system->GetNeighborhood(domain_t).GetPointer();
      using ImageType = itk::Image<float, Dimension>;
      auto neighborhood__ = dynamic_cast<const ParticleSurfaceNeighborhood<ImageType>*>(neighborhood_);
      auto neighborhood = const_cast<ParticleSurfaceNeighborhood<ImageType>*>(neighborhood__);


      const bool this_is_contour = system->GetDomain(d)->GetDomainType() == shapeworks::DomainType::Contour;
      if(this_is_contour && domain_t != d) {
        continue;
      }
      const bool other_is_contour = system->GetDomain(domain_t)->GetDomainType() == shapeworks::DomainType::Contour;
      if(!other_is_contour && domain_t != d) {
        continue;
      }

      // yup, no weighting for now
      neighborhood->SetWeightingEnabled(false);

      std::vector<double> weights;
      std::vector<double> distances;
      std::vector<ParticlePointIndexPair<3>> res;
      if(domain_t == d) {
        res = neighborhood->FindNeighborhoodPoints(pos, idx, weights, distances, radius);
      } else {
        neighborhood->SetForceEuclidean(true);
        res = neighborhood->FindNeighborhoodPoints(pos, -1, weights, distances, radius);
        neighborhood->SetForceEuclidean(false);
      }

      assert(weights.size() == distances.size() && res.size() == weights.size());

      for(int i=0; i<res.size(); i++) {
        m_CurrentNeighborhood.emplace_back(
          res[i],
          weights[i],
          distances[i],
          domain_t
        );
      }
    }
  }

  float m_MaxMoveFactor;
  
};

} //end namespace

#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleCurvatureEntropyGradientFunction+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleCurvatureEntropyGradientFunction.txx"
#endif

#include "itkParticleCurvatureEntropyGradientFunction.txx"

#endif
