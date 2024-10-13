---
title: Libs/Optimize/Function/SamplingFunction.h

---

# Libs/Optimize/Function/SamplingFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)**  |




## Source code

```cpp
#pragma once

#include <Libs/Optimize/Container/GenericContainerArray.h>

#include "VectorFunction.h"

namespace shapeworks {

class SamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef SamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  itkNewMacro(Self);

  using VectorType = vnl_vector_fixed<double, 3>;
  using PointType = ParticleSystem::PointType;
  using GradientVectorType = vnl_vector_fixed<float, 3>;
  using SigmaCacheType = GenericContainerArray<double>;

  VectorType Evaluate(unsigned int a, unsigned int b, const ParticleSystem* c, double& d) const override {
    double e;
    return Evaluate(a, b, c, d, e);
  }

  VectorType Evaluate(unsigned int, unsigned int, const ParticleSystem*, double&, double&) const override;

  void BeforeEvaluate(unsigned int, unsigned int, const ParticleSystem*) override;

  double Energy(unsigned int a, unsigned int b, const ParticleSystem* c) const override {
    double d, e;
    Evaluate(a, b, c, d, e);
    return e;
  }

  double EstimateSigma(unsigned int idx, unsigned int dom, const shapeworks::ParticleDomain* domain,
                       const PointType& pos, double initial_sigma, double precision, int& err, double& avg_kappa) const;

  void SetSharedBoundaryWeight(double w) { m_SharedBoundaryWeight = w; }
  double GetSharedBoundaryWeight() const { return m_SharedBoundaryWeight; }

  void SetSharedBoundaryEnabled(bool enabled) { m_IsSharedBoundaryEnabled = enabled; }
  bool GetSharedBoundaryEnabled() const { return m_IsSharedBoundaryEnabled; }

  void SetSpatialSigmaCache(SigmaCacheType* s) { m_SpatialSigmaCache = s; }
  SigmaCacheType* GetSpatialSigmaCache() { return m_SpatialSigmaCache.GetPointer(); }
  const SigmaCacheType* GetSpatialSigmaCache() const { return m_SpatialSigmaCache.GetPointer(); }

  void SetMinimumNeighborhoodRadius(double s) { m_MinimumNeighborhoodRadius = s; }
  double GetMinimumNeighborhoodRadius() const { return m_MinimumNeighborhoodRadius; }

  void SetMaximumNeighborhoodRadius(double s) { m_MaximumNeighborhoodRadius = s; }
  double GetMaximumNeighborhoodRadius() const { return m_MaximumNeighborhoodRadius; }

  void SetFlatCutoff(double s) { m_FlatCutoff = s; }
  double GetFlatCutoff() const { return m_FlatCutoff; }
  void SetNeighborhoodToSigmaRatio(double s) { m_NeighborhoodToSigmaRatio = s; }
  double GetNeighborhoodToSigmaRatio() const { return m_NeighborhoodToSigmaRatio; }

  void ResetBuffers() override { m_SpatialSigmaCache->ZeroAllValues(); }

  VectorFunction::Pointer Clone() override;

 private:
  SamplingFunction() {}
  virtual ~SamplingFunction() {}
  void operator=(const SamplingFunction&);
  SamplingFunction(const SamplingFunction&);

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

  unsigned int m_Counter{0};
  double m_avgKappa{0};
  bool m_IsSharedBoundaryEnabled{false};
  double m_SharedBoundaryWeight{1.0};
  double m_CurrentSigma{0.0};
  float m_MaxMoveFactor{0};
  double m_MinimumNeighborhoodRadius{0};
  double m_MaximumNeighborhoodRadius{0};
  double m_FlatCutoff{0.05};
  double m_NeighborhoodToSigmaRatio{3.0};

  SigmaCacheType::Pointer m_SpatialSigmaCache;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-10-13 at 06:37:12 +0000
