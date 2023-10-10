---
title: Libs/Optimize/Function/CurvatureSamplingFunction.h

---

# Libs/Optimize/Function/CurvatureSamplingFunction.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::CurvatureSamplingFunction](../Classes/classshapeworks_1_1CurvatureSamplingFunction.md)** <br>Please note: This is the sampling function currently being used.  |




## Source code

```cpp
#pragma once

#include "Libs/Optimize/Container/MeanCurvatureContainer.h"
#include "Libs/Optimize/Domain/ImageDomainWithCurvature.h"
#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Neighborhood/ParticleSurfaceNeighborhood.h"
#include "SamplingFunction.h"
#include "itkCommand.h"

namespace shapeworks {


class CurvatureSamplingFunction : public SamplingFunction {
 public:
  constexpr static int VDimension = 3;
  typedef float
      TGradientNumericType;  // This has always been used on float images, so the curvature cache is also float
  typedef CurvatureSamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef SamplingFunction Superclass;
  itkTypeMacro(CurvatureSamplingFunction, SamplingFunction);

  typedef Superclass::GradientNumericType GradientNumericType;
  typedef Superclass::VectorType VectorType;
  typedef Superclass::PointType PointType;
  typedef Superclass::GradientVectorType GradientVectorType;

  typedef MeanCurvatureContainer<TGradientNumericType, VDimension> MeanCurvatureCacheType;

  typedef shapeworks::ImageDomainWithCurvature<TGradientNumericType>::VnlMatrixType VnlMatrixType;

  itkNewMacro(Self);

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

  inline double ComputeKappa(double mc, unsigned int d) const;

  virtual void AfterIteration() {}

  virtual void BeforeIteration() {
    //  this->ComputeKappaValues();
  }

  virtual double EstimateSigma(unsigned int idx, unsigned int dom, const shapeworks::ParticleDomain* domain,
                               const PointType& pos, double initial_sigma, double precision, int& err,
                               double& avgKappa) const;

  // Access the cache of curvature-based weight values for each particle position
  void SetMeanCurvatureCache(MeanCurvatureCacheType* s) { m_MeanCurvatureCache = s; }
  MeanCurvatureCacheType* GetMeanCurvatureCache() { return m_MeanCurvatureCache.GetPointer(); }

  const MeanCurvatureCacheType* GetMeanCurvatureCache() const { return m_MeanCurvatureCache.GetPointer(); }

  void SetRho(double g) { m_Rho = g; }
  double GetRho() const { return m_Rho; }

  void SetSharedBoundaryWeight(double w) { m_SharedBoundaryWeight = w; }
  double GetSharedBoundaryWeight() const { return m_SharedBoundaryWeight; }

  void SetSharedBoundaryEnabled(bool enabled) { m_IsSharedBoundaryEnabled = enabled; }
  bool GetSharedBoundaryEnabled() const { return m_IsSharedBoundaryEnabled; }

  virtual VectorFunction::Pointer Clone() {
    // todo Do we really need to clone all of this?

    CurvatureSamplingFunction::Pointer copy = CurvatureSamplingFunction::New();
    copy->SetParticleSystem(this->GetParticleSystem());
    copy->m_Counter = this->m_Counter;
    copy->m_Rho = this->m_Rho;
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
    copy->m_MeanCurvatureCache = this->m_MeanCurvatureCache;

    copy->m_DomainNumber = this->m_DomainNumber;
    copy->m_ParticleSystem = this->m_ParticleSystem;

    return (VectorFunction::Pointer)copy;
  }

 protected:
  CurvatureSamplingFunction() : m_Counter(0), m_Rho(1.0) {}
  virtual ~CurvatureSamplingFunction() {}
  void operator=(const CurvatureSamplingFunction&);
  CurvatureSamplingFunction(const CurvatureSamplingFunction&);
  MeanCurvatureCacheType::Pointer m_MeanCurvatureCache;

  unsigned int m_Counter;
  double m_Rho;

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
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-10 at 07:25:30 +0000
