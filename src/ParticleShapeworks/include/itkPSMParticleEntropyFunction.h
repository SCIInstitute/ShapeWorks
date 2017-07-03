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
#ifndef __itkPSMParticleEntropyFunction_h
#define __itkPSMParticleEntropyFunction_h

#include "itkPSMCostFunction.h"
#include "itkPSMContainerArrayAttribute.h"
#include "itkPSMImageDomainWithGradients.h"
#include <vector>
#include "itkPSMImplicitSurfaceDomain.h"

namespace itk
{

/**
 * \class PSMParticleEntropyFunction
 *
 * \brief This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.

 *
 * The following description is an excerpt from
 *
 *J Cates, P T Fletcher, M Styner, M Shenton, R Whitaker. Shape Modeling and
 * Analysis with Entropy-Based Particle Systems.  Information Processing in
 * Medical Imaging IPMI 2007, LNCS 4584, pp. 333–345, 2007.
 *
 *
 * We treat a surface as a subset of \f$\Re^d\f$, where \f$d=2\f$ or \f$d=3\f$
 * depending whether we are processing curves in the plane or surfaces in a
 * volume, refspectively.  The method we describe here deals with smooth,
 * closed manifolds of codimension one, and we will refer to such manifolds as
 * {\em surfaces}.  We sample a surface \f${\cal S} \subset \Re^d\f$ using a
 * discrete set of \f$N\f$ points that are considered random variables \f$Z =
 * (X_1, X_2, \ldots, X_N)\f$ drawn from a probability density function (PDF),
 * \f$p(X)\f$.  We denote a realization of this PDF with lower case, and thus
 * we have \f$z = (x_1, x_2,\ldots, x_N)\f$, where \f$z \in {\cal S}^N\f$.  The
 * probability of a realization \f$x\f$ is \f$p(X = x)\f$, which we denote
 * simply as \f$p(x)\f$.
 *
 * The amount of information contained in such a random sampling is, in the
 * limit, the differential entropy of the PDF, which is \f$H[X] = -\int_S p(x)
 * \log p(x) dx = -E\{\log p(X)\}\f$, where \f$E\{ \cdot \}\f$ is the
 * expectation. When we have a sufficient number of points sampled from
 * \f$p\f$, we can approximate the expectation by the sample mean, which gives
 * \f$H[X] \approx - (1/N)\sum_{i} \log p(x_i)\f$.  We must also estimate
 * \f$p(x_i)\f$.  Density functions on surfaces can be quite complex, and so we
 * use a nonparametric, Parzen windowing estimation of this density using the
 * particles themselves.  Thus we have
 *\f[
 * p(x_i) \approx \frac{1}{N(N-1)} \sum^N_{j=1, j \neq i} G(x_i - x_j,
 * \sigma_i),
 * \f]
 * where \f$G(x_i - x_j,
 * \sigma_i)\f$ is a \f$d\f$-dimensional, isotropic Gaussian with standard
 * deviation \f$\sigma_i\f$.  The cost function \f$C\f$, is therefore an
 * approximation of (negative) entropy:
 * \f[
 * -H[X] \approx C(x_1, \dots, x_N) =
 * \sum_{i} \log \frac{1}{N(N-1)} \sum_{j \neq i} G(x_i - x_j, \sigma_i).
 * \f]
 *
 * \ingroup PSM
 * \ingroup PSMOptimization
 * \ingroup PSMCostFunctions
 * \author Josh Cates
 */
template <class TGradientNumericType, unsigned int VDimension>
class PSMParticleEntropyFunction : public PSMCostFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef PSMParticleEntropyFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef PSMCostFunction<VDimension> Superclass;
  itkTypeMacro( PSMParticleEntropyFunction, PSMCostFunction);

  /** Data type representing individual gradient components. */
  typedef TGradientNumericType GradientNumericType;

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  /** Cache type for the sigma values. */
  typedef PSMContainerArrayAttribute<double, VDimension> SigmaCacheType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;
  typedef  vnl_vector_fixed<TGradientNumericType, VDimension> GradientVectorType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */
  //  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
  //                              double &) const;

  //  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
  //                              double &, double &) const
  //  {
  //    itkExceptionMacro("This method not implemented");
  //    return VectorType();
  //  }
  //  virtual double Energy(unsigned int, unsigned int, const ParticleSystemType *) const
  //  {
  //    itkExceptionMacro("This method not implemented");
  //    return 0.0;
  //  }

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystemType *,
                              double&, double & ) const;

  inline virtual VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystemType *c,
                                     double& d) const
  {
      double e;
      return this->Evaluate(a, b, c, d, e);
  }

  inline virtual double Energy(unsigned int a, unsigned int b, const ParticleSystemType *c) const
  {
      double d, e;
      this->Evaluate(a, b, c, d, e);
      return e;
  }

  virtual void ResetBuffers()
  {
    m_SpatialSigmaCache->ZeroAllValues();
  }
    
  
  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma(unsigned int, const typename ParticleSystemType::PointVectorType &,
                                const std::vector<double> &,
                                    const PointType &, double,  double,  int &err) const;

  /** Returns a weighting coefficient based on the angle between two
     vectors. Weights smoothly approach zero as the angle between two
     normals approaches 90 degrees. */
  TGradientNumericType AngleCoefficient(const GradientVectorType&,
                                        const GradientVectorType&) const;
  
  /** The minimum radius of the neighborhood of points that are
      considered in the entropy calculation. The neighborhood is a
      spherical radius in 3D space. The actual radius used in a
      calculation may exceed this value, but will not exceed the
      MaximumNeighborhoodRadius. This parameter should ALWAYS be set
      by an application, because it must be scaled according to the
      spacing in the image.  A good value is typically 5x the spacing
      of the highest-resolution dimension (the dimension with the
      smallest spacing. */
  void SetMinimumNeighborhoodRadius( double s)
  { m_MinimumNeighborhoodRadius = s; }
  double GetMinimumNeighborhoodRadius() const
  { return m_MinimumNeighborhoodRadius; }

  /** Maximum radius of the neighborhood of points that are considered
      in the calculation. The neighborhood is a spherical radius in 3D
      space. MaximumNeighborhoodRadius should be set to a value
      equal-to or less-than the length of the largest dimension of the
      image.  This parameter should ALWAYS be set by an application,
      since this class cannot know by default the size of input
      images. The radius value should be specified in real-world
      coordinates. */
  void SetMaximumNeighborhoodRadius( double s)
  { m_MaximumNeighborhoodRadius = s; }
  double GetMaximumNeighborhoodRadius() const
  { return m_MaximumNeighborhoodRadius; }

  /** The influence of particle neighbors on each other is weighted by
      the angle between the surface normals at their respective
      positions.  The "flat cutoff" parameter adjusts the degree to
      which surface normals must differ before the weighting kicks in.
      Angles below the "flat cutoff" angle, which is specified in
      radians, are given a weighting of 1.0.  Angles above this
      parameter value result in a lower-weight interaction between
      particles.  Flat cutoff is set to a default value that should
      work for most applications.  It is not necessary to set or tune
      this parameter unless you would like to tweak performance on a
      particular dataset. */
  void SetFlatCutoff(double s)
  { m_FlatCutoff = s; }
  double GetFlatCutoff() const
  { return m_FlatCutoff; }

  /** NeighborhoodToSigmaRatio defines the extent of a given
      particle's neighborhood.  A particle only interacts with
      neighbors in this neighborhood.  All other particles on the
      surface are ignored.  The neighborhood extent is computed as a
      multiple of the estimated standard deviation (sigma) of the
      local particle positions.  This parameter has a default value
      that should work well for most data.  It is not necessary to set
      or tune this parameter unless you would like to tweak
      performance for a particular dataset. */
  void SetNeighborhoodToSigmaRatio(double s)
  { m_NeighborhoodToSigmaRatio = s; }
  double GetNeighborhoodToSigmaRatio() const
  { return m_NeighborhoodToSigmaRatio; }

  /**Access the cache of sigma values for each particle position.  This cache
     is populated by registering this object as an observer of the correct
     particle system (see SetParticleSystem).*/
  void SetSpatialSigmaCache( SigmaCacheType *s)
  {    m_SpatialSigmaCache = s;  }
  SigmaCacheType *GetSpatialSigmaCache()
  {   return  m_SpatialSigmaCache.GetPointer();  }
  const SigmaCacheType *GetSpatialSigmaCache() const
  {   return  m_SpatialSigmaCache.GetPointer();  }

  /** Compute a set of weights based on the difference in the normals of a
      central point and each of its neighbors.  Difference of > 90 degrees
      results in a weight of 0. */
  void ComputeAngularWeights(const PointType &,
                             const typename ParticleSystemType::PointVectorType &,
                             const PSMImageDomainWithGradients<TGradientNumericType, VDimension> *,
                             std::vector<double> &) const;
  
protected:
 PSMParticleEntropyFunction() : m_MinimumNeighborhoodRadius(5.0),
    m_MaximumNeighborhoodRadius(1024.0), m_FlatCutoff(0.3), 
    m_NeighborhoodToSigmaRatio(3.0) {}

  virtual ~PSMParticleEntropyFunction() {}
  void operator=(const PSMParticleEntropyFunction &);
  PSMParticleEntropyFunction(const PSMParticleEntropyFunction &);

  double m_MinimumNeighborhoodRadius;
  double m_MaximumNeighborhoodRadius;
  double m_FlatCutoff;
  double m_NeighborhoodToSigmaRatio;
  typename SigmaCacheType::Pointer m_SpatialSigmaCache;
};


} //end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMParticleEntropyFunction.hxx"
#endif

#endif
