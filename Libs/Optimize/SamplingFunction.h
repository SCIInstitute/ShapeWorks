#pragma once

#include <vector>

#include "ParticleImageDomainWithGradients.h"
#include "VectorFunction.h"
#include "GenericContainerArray.h"

namespace shapeworks {

/**
 * \class ParticleEntropyGradientFunction
 *
 * \brief This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.

 *
 * The following description is an excerpt from
 *
 *J Cates, P T Fletcher, M Styner, M Shenton, R Whitaker. Shape Modeling and
 * Analysis with Entropy-Based Particle Systems.  Information Processing in
 * Medical Imaging IPMI 2007, LNCS 4584, pp. 333�345, 2007.
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
 */
class SamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef float
      TGradientNumericType;  // This has always been used on float images, so the curvature cache is also float

  /** Standard class typedefs. */
  typedef SamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(SamplingFunction, VectorFunction);

  /** Data type representing individual gradient components. */
  typedef TGradientNumericType GradientNumericType;

  /** Cache type for the sigma values. */
  typedef GenericContainerArray<double> SigmaCacheType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector_fixed<TGradientNumericType, VDimension> GradientVectorType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxdt) const;

  virtual VectorType Evaluate(unsigned int idx, unsigned int d, const ParticleSystem* system, double& maxdt,
                              double& energy) const {
    itkExceptionMacro("This method not implemented");
    return VectorType();
  }
  virtual double Energy(unsigned int, unsigned int, const ParticleSystem*) const {
    itkExceptionMacro("This method not implemented");
    return 0.0;
  }

  virtual void ResetBuffers() { m_SpatialSigmaCache->ZeroAllValues(); }

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma(unsigned int idx, const typename ParticleSystem::PointVectorType& neighborhood,
                               const shapeworks::ParticleDomain* domain, const std::vector<double>& weights,
                               const PointType& pos, double initial_sigma, double precision, int& err) const;

  /** Returns a weighting coefficient based on the angle between two
     vectors. Weights smoothly approach zero as the angle between two
     normals approaches 90 degrees. */
  TGradientNumericType AngleCoefficient(const GradientVectorType&, const GradientVectorType&) const;

  /** Minimum radius of the neighborhood of points that are considered in the
      calculation. The neighborhood is a spherical radius in 3D space. The
      actual radius used in a calculation may exceed this value, but will not
      exceed the MaximumNeighborhoodRadius. */
  void SetMinimumNeighborhoodRadius(double s) { m_MinimumNeighborhoodRadius = s; }
  double GetMinimumNeighborhoodRadius() const { return m_MinimumNeighborhoodRadius; }

  /** Maximum radius of the neighborhood of points that are considered in the
      calculation. The neighborhood is a spherical radius in 3D space. */
  void SetMaximumNeighborhoodRadius(double s) { m_MaximumNeighborhoodRadius = s; }
  double GetMaximumNeighborhoodRadius() const { return m_MaximumNeighborhoodRadius; }

  /** Numerical parameters*/
  void SetFlatCutoff(double s) { m_FlatCutoff = s; }
  double GetFlatCutoff() const { return m_FlatCutoff; }
  void SetNeighborhoodToSigmaRatio(double s) { m_NeighborhoodToSigmaRatio = s; }
  double GetNeighborhoodToSigmaRatio() const { return m_NeighborhoodToSigmaRatio; }

  /**Access the cache of sigma values for each particle position.  This cache
     is populated by registering this object as an observer of the correct
     particle system (see SetParticleSystem).*/
  void SetSpatialSigmaCache(SigmaCacheType* s) { m_SpatialSigmaCache = s; }
  SigmaCacheType* GetSpatialSigmaCache() { return m_SpatialSigmaCache.GetPointer(); }
  const SigmaCacheType* GetSpatialSigmaCache() const { return m_SpatialSigmaCache.GetPointer(); }

  /** Compute a set of weights based on the difference in the normals of a
      central point and each of its neighbors.  Difference of > 90 degrees
      results in a weight of 0. */
  void ComputeAngularWeights(const PointType&, int, const typename ParticleSystem::PointVectorType&,
                             const shapeworks::ParticleDomain*, std::vector<double>&) const;

  //  void ComputeNeighborho0d();

  virtual VectorFunction::Pointer Clone() {
    SamplingFunction::Pointer copy = SamplingFunction::New();

    // from itkParticleVectorFunction
    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    // local
    copy->m_FlatCutoff = this->m_FlatCutoff;
    copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
    copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
    copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;
    copy->m_SpatialSigmaCache = this->m_SpatialSigmaCache;

    return (typename VectorFunction::Pointer)copy;
  }

 protected:
  SamplingFunction() : m_FlatCutoff(0.05), m_NeighborhoodToSigmaRatio(3.0) {}
  virtual ~SamplingFunction() {}
  void operator=(const SamplingFunction&);
  SamplingFunction(const SamplingFunction&);

  double m_MinimumNeighborhoodRadius;
  double m_MaximumNeighborhoodRadius;
  double m_FlatCutoff;
  double m_NeighborhoodToSigmaRatio;
  typename SigmaCacheType::Pointer m_SpatialSigmaCache;
};

}  // namespace shapeworks
