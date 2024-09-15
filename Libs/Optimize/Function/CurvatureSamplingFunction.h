#pragma once

#include <Libs/Optimize/Container/GenericContainerArray.h>

#include "VectorFunction.h"

namespace shapeworks {

/**
 * \class SamplingFunction
 *
 * This function is responsible for the sampling term of the optimization.
 * E.g. the repulsion force between particles on a single shape
 *
 */
class SamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  /** Standard class typedefs. */
  typedef SamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  itkTypeMacro(CurvatureSamplingFunction, SamplingFunction);
  itkNewMacro(Self);

  using VectorType = vnl_vector_fixed<double, 3>;
  using PointType = ParticleSystem::PointType;
  using GradientVectorType = vnl_vector_fixed<float, 3>;
  using SigmaCacheType = GenericContainerArray<double>;

  inline VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const override {
    double e;
    return this->Evaluate(a, b, c, d, e);
  }

  VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double&, double&) const override;

  void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystem*) override;

  double Energy(unsigned int a, unsigned int b, const ParticleSystem* c) const override {
    double d, e;
    this->Evaluate(a, b, c, d, e);
    return e;
  }

  void AfterIteration() override {}

  void BeforeIteration() override {}

  /** Estimate the best sigma for Parzen windowing in a given neighborhood.
      The best sigma is the sigma that maximizes probability at the given point  */
  double EstimateSigma(unsigned int idx, unsigned int dom, const shapeworks::ParticleDomain* domain,
                       const PointType& pos, double initial_sigma, double precision, int& err, double& avgKappa) const;

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

  void ResetBuffers() override { m_SpatialSigmaCache->ZeroAllValues(); }

  VectorFunction::Pointer Clone() override {
    // todo Do we really need to clone all of this?

    SamplingFunction::Pointer copy = SamplingFunction::New();
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
  SamplingFunction() {}
  virtual ~SamplingFunction() {}
  void operator=(const SamplingFunction&);
  SamplingFunction(const SamplingFunction&);

  unsigned int m_Counter{0};

  double m_avgKappa{0};
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

  float m_MaxMoveFactor{0};

  typename SigmaCacheType::Pointer m_SpatialSigmaCache;

  double m_MinimumNeighborhoodRadius{0};
  double m_MaximumNeighborhoodRadius{0};
  double m_FlatCutoff{0.05};
  double m_NeighborhoodToSigmaRatio{3.0};
};

}  // namespace shapeworks
