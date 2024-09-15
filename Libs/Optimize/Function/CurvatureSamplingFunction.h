#pragma once

#include "SamplingFunction.h"

namespace shapeworks {

/// Please note: This is the sampling function currently being used

/**
 * \class CurvatureSamplingFunction
 *
 * This function returns an estimate of the gradient of the entropy of a
 * particle distribution with respect to change in position of a specific
 * particle in that distribution.  The change in normals associated with the
 * position is also taken into account, providing an adaptive sampling of the
 * surface with respect to both position and extrinsic surface curvature.
 *
 */
class CurvatureSamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef float
      TGradientNumericType;  // This has always been used on float images, so the curvature cache is also float
  /** Standard class typedefs. */
  typedef CurvatureSamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef SamplingFunction Superclass;
  itkTypeMacro(CurvatureSamplingFunction, SamplingFunction);

  /** Inherit some parent typedefs. */
  typedef float GradientNumericType;
  typedef vnl_vector_fixed<double, 3> VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector_fixed<float, 3> GradientVectorType;

  /** Cache type for the sigma values. */
  typedef GenericContainerArray<double> SigmaCacheType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  inline virtual VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const {
    double e;
    return this->Evaluate(a, b, c, d, e);
  }

  virtual VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double&, double&) const;

  virtual void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystem*);

  inline virtual double Energy(unsigned int a, unsigned int b, const ParticleSystem* c) const {
    double d, e;
    this->Evaluate(a, b, c, d, e);
    return e;
  }

  virtual void AfterIteration() {}

  virtual void BeforeIteration() {}

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  virtual double EstimateSigma(unsigned int idx, unsigned int dom, const shapeworks::ParticleDomain* domain,
                               const PointType& pos, double initial_sigma, double precision, int& err,
                               double& avgKappa) const;

  void SetSharedBoundaryWeight(double w) { m_SharedBoundaryWeight = w; }
  double GetSharedBoundaryWeight() const { return m_SharedBoundaryWeight; }

  void SetSharedBoundaryEnabled(bool enabled) { m_IsSharedBoundaryEnabled = enabled; }
  bool GetSharedBoundaryEnabled() const { return m_IsSharedBoundaryEnabled; }

  /**Access the cache of sigma values for each particle position.  This cache
     is populated by registering this object as an observer of the correct
     particle system (see SetParticleSystem).*/
  void SetSpatialSigmaCache(SigmaCacheType* s) { m_SpatialSigmaCache = s; }
  SigmaCacheType* GetSpatialSigmaCache() { return m_SpatialSigmaCache.GetPointer(); }
  const SigmaCacheType* GetSpatialSigmaCache() const { return m_SpatialSigmaCache.GetPointer(); }

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

  virtual void ResetBuffers() { m_SpatialSigmaCache->ZeroAllValues(); }

  virtual VectorFunction::Pointer Clone() {
    // todo Do we really need to clone all of this?

    CurvatureSamplingFunction::Pointer copy = CurvatureSamplingFunction::New();
    copy->SetParticleSystem(this->GetParticleSystem());
    copy->m_Counter = this->m_Counter;
    copy->m_avgKappa = this->m_avgKappa;
    copy->m_IsSharedBoundaryEnabled = this->m_IsSharedBoundaryEnabled;
    copy->m_SharedBoundaryWeight = this->m_SharedBoundaryWeight;
    copy->m_CurrentSigma = this->m_CurrentSigma;
    copy->m_CurrentNeighborhood = this->m_CurrentNeighborhood;

    copy->m_MinimumNeighborhoodRadius = this->m_MinimumNeighborhoodRadius;
    copy->m_MaximumNeighborhoodRadius = this->m_MaximumNeighborhoodRadius;
    copy->m_FlatCutoff = this->m_FlatCutoff;
    copy->m_NeighborhoodToSigmaRatio = this->m_NeighborhoodToSigmaRatio;

    copy->m_SpatialSigmaCache = this->m_SpatialSigmaCache;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    return (VectorFunction::Pointer)copy;
  }

 protected:
  CurvatureSamplingFunction() {}
  virtual ~CurvatureSamplingFunction() {}
  void operator=(const CurvatureSamplingFunction&);
  CurvatureSamplingFunction(const CurvatureSamplingFunction&);

  unsigned int m_Counter{0};

  double m_avgKappa;
  bool m_IsSharedBoundaryEnabled{false};
  double m_SharedBoundaryWeight{1.0};

  double m_CurrentSigma;

  struct CrossDomainNeighborhood {
    ParticlePointIndexPair pi_pair;
    double weight;
    double distance;
    int dom;

    CrossDomainNeighborhood(const ParticlePointIndexPair& pi_pair_, double weight_, double distance_, int dom_)
        : pi_pair(pi_pair_), weight(weight_), distance(distance_), dom(dom_) {}
  };
  std::vector<CrossDomainNeighborhood> m_CurrentNeighborhood;
  void UpdateNeighborhood(const PointType& pos, int idx, int d, double radius, const ParticleSystem* system);

  float m_MaxMoveFactor = 0;

  typename SigmaCacheType::Pointer m_SpatialSigmaCache;

  double m_MinimumNeighborhoodRadius;
  double m_MaximumNeighborhoodRadius;
  double m_FlatCutoff{0.05};
  double m_NeighborhoodToSigmaRatio{3.0};
};

}  // namespace shapeworks
