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
| class | **[shapeworks::SamplingFunction](../Classes/classshapeworks_1_1SamplingFunction.md)** <br>This function returns an estimate of the gradient of the entropy of a particle distribution with respect to change in position of a specific particle in that distribution.  |




## Source code

```cpp
#pragma once

#include <vector>

#include "Libs/Optimize/Container/GenericContainerArray.h"
#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "VectorFunction.h"

namespace shapeworks {



class SamplingFunction : public VectorFunction {
 public:
  constexpr static int VDimension = 3;
  typedef float
      TGradientNumericType;  // This has always been used on float images, so the curvature cache is also float

  typedef SamplingFunction Self;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef VectorFunction Superclass;
  itkTypeMacro(SamplingFunction, VectorFunction);

  typedef TGradientNumericType GradientNumericType;

  typedef GenericContainerArray<double> SigmaCacheType;

  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystem::PointType PointType;
  typedef vnl_vector_fixed<TGradientNumericType, VDimension> GradientVectorType;

  itkNewMacro(Self);

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

  virtual double EstimateSigma(unsigned int idx, const typename ParticleSystem::PointVectorType& neighborhood,
                               const shapeworks::ParticleDomain* domain, const std::vector<double>& weights,
                               const PointType& pos, double initial_sigma, double precision, int& err) const;

  TGradientNumericType AngleCoefficient(const GradientVectorType&, const GradientVectorType&) const;

  void SetMinimumNeighborhoodRadius(double s) { m_MinimumNeighborhoodRadius = s; }
  double GetMinimumNeighborhoodRadius() const { return m_MinimumNeighborhoodRadius; }

  void SetMaximumNeighborhoodRadius(double s) { m_MaximumNeighborhoodRadius = s; }
  double GetMaximumNeighborhoodRadius() const { return m_MaximumNeighborhoodRadius; }

  void SetFlatCutoff(double s) { m_FlatCutoff = s; }
  double GetFlatCutoff() const { return m_FlatCutoff; }
  void SetNeighborhoodToSigmaRatio(double s) { m_NeighborhoodToSigmaRatio = s; }
  double GetNeighborhoodToSigmaRatio() const { return m_NeighborhoodToSigmaRatio; }

  void SetSpatialSigmaCache(SigmaCacheType* s) { m_SpatialSigmaCache = s; }
  SigmaCacheType* GetSpatialSigmaCache() { return m_SpatialSigmaCache.GetPointer(); }
  const SigmaCacheType* GetSpatialSigmaCache() const { return m_SpatialSigmaCache.GetPointer(); }

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
```


-------------------------------

Updated on 2023-10-10 at 02:21:13 +0000
