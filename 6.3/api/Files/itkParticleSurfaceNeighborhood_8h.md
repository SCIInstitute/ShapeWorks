---
title: Libs/Optimize/ParticleSystem/itkParticleSurfaceNeighborhood.h

---

# Libs/Optimize/ParticleSystem/itkParticleSurfaceNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleSurfaceNeighborhood](../Classes/classitk_1_1ParticleSurfaceNeighborhood.md)**  |




## Source code

```cpp
#pragma once

#include "itkParticleRegionNeighborhood.h"
#include "ParticleImplicitSurfaceDomain.h"
#include "vnl/vnl_vector_fixed.h"

namespace itk
{
template <class TImage>
class ITK_EXPORT ParticleSurfaceNeighborhood : public ParticleRegionNeighborhood<TImage::ImageDimension>
{
public:
  typedef TImage ImageType;
  typedef ParticleSurfaceNeighborhood Self;
  typedef ParticleRegionNeighborhood<TImage::ImageDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  typedef typename ImageType::PixelType NumericType;
  typedef typename Superclass::PointTreeType PointTreeType;

  typedef  vnl_vector_fixed<NumericType, TImage::ImageDimension> GradientVectorType;

  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);

  itkNewMacro(Self);

  itkTypeMacro(ParticleSurfaceNeighborhood, ParticleRegionNeighborhood);

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::PointContainerType PointContainerType;
  typedef shapeworks::ParticleImplicitSurfaceDomain<typename TImage::PixelType> DomainType;
  typedef typename Superclass::PointVectorType PointVectorType;

  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &,
          std::vector<double>&, double) const override;
  virtual PointVectorType FindNeighborhoodPoints(const PointType &, int idx, std::vector<double> &, double) const override;
  //  virtual unsigned int  FindNeighborhoodPoints(const PointType &, double, PointVectorType &) const;

  void SetWeightingEnabled(bool is_enabled)
  {
    m_WeightingEnabled = is_enabled;
  }

  bool IsWeightingEnabled() const
  {
    return m_WeightingEnabled;
  }

  void SetForceEuclidean(bool is_enabled) {
    m_ForceEuclidean = is_enabled;
  }

  bool IsForceEuclidean() const {
    return m_ForceEuclidean;
  }

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }


protected:
  ParticleSurfaceNeighborhood() : m_FlatCutoff(0.30)  {  }
  virtual ~ParticleSurfaceNeighborhood() {};

private:
  ParticleSurfaceNeighborhood(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  double m_FlatCutoff;
  bool m_WeightingEnabled{true};
  bool m_ForceEuclidean{false};

};

} // end namespace itk


#include "itkParticleSurfaceNeighborhood.txx"
```


-------------------------------

Updated on 2022-04-08 at 15:18:27 +0000
