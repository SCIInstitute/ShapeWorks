---
title: Libs/Optimize/Neighborhood/ParticleSurfaceNeighborhood.h

---

# Libs/Optimize/Neighborhood/ParticleSurfaceNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleSurfaceNeighborhood](../Classes/classshapeworks_1_1ParticleSurfaceNeighborhood.md)**  |




## Source code

```cpp
#pragma once

#include "Libs/Optimize/Domain/ImplicitSurfaceDomain.h"
#include "ParticleRegionNeighborhood.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {
class ParticleSurfaceNeighborhood : public ParticleRegionNeighborhood {
 public:
  typedef itk::Image<float, 3> TImage;
  typedef TImage ImageType;
  typedef ParticleSurfaceNeighborhood Self;
  typedef ParticleRegionNeighborhood Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;
  typedef typename ImageType::PixelType NumericType;
  typedef typename Superclass::PointTreeType PointTreeType;

  typedef vnl_vector_fixed<NumericType, TImage::ImageDimension> GradientVectorType;

  itkStaticConstMacro(Dimension, unsigned int, 3);

  itkNewMacro(Self);

  itkTypeMacro(ParticleSurfaceNeighborhood, ParticleRegionNeighborhood);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef shapeworks::ImplicitSurfaceDomain<typename TImage::PixelType> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, std::vector<double>&,
                                                 double) const override;
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&,
                                                 double) const override;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetWeightingEnabled(bool is_enabled) { m_WeightingEnabled = is_enabled; }

  bool IsWeightingEnabled() const { return m_WeightingEnabled; }

  void SetForceEuclidean(bool is_enabled) { m_ForceEuclidean = is_enabled; }

  bool IsForceEuclidean() const { return m_ForceEuclidean; }

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 protected:
  ParticleSurfaceNeighborhood() : m_FlatCutoff(0.30) {}
  virtual ~ParticleSurfaceNeighborhood(){};

 private:
  ParticleSurfaceNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);               // purposely not implemented
  double m_FlatCutoff;
  bool m_WeightingEnabled{true};
  bool m_ForceEuclidean{false};
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2023-06-30 at 16:21:41 +0000
