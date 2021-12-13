---
title: Libs/Optimize/ParticleSystem/itkParticleImageDomainWithGradients.h

---

# Libs/Optimize/ParticleSystem/itkParticleImageDomainWithGradients.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleImageDomainWithGradients](../Classes/classitk_1_1ParticleImageDomainWithGradients.md)**  |




## Source code

```cpp
/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "itkParticleImageDomain.h"
#include "itkGradientImageFilter.h"
#include "itkFixedArray.h"

namespace itk
{
template <class T>
class ParticleImageDomainWithGradients : public ParticleImageDomain<T>
{
public:
  typedef SmartPointer<ParticleImageDomainWithGradients<T>>  Pointer;

  typedef typename ParticleImageDomain<T>::PointType PointType;
  typedef typename ParticleImageDomain<T>::ImageType ImageType;

  typedef FixedArray<T, DIMENSION> VectorType;
  typedef vnl_vector_fixed<T, DIMENSION> VnlVectorType;

  void SetImage(ImageType *I, double narrow_band) {
    ParticleImageDomain<T>::SetImage(I, narrow_band);
    m_VDBGradient = openvdb::tools::gradient(*this->GetVDBImage());
  }

  inline vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &p, int idx) const {
    return this->SampleGradientVnl(p, idx);
  }

  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &p, int idx) const {
    vnl_vector_fixed<float, DIMENSION> grad = this->SampleGradientVnl(p, idx);
    return grad.normalize();
  }

  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos, int idx) const override
  {
    double dotprod = 0.0;  
    VnlVectorType normal =  this->SampleNormalAtPoint(pos, idx);
    for (unsigned int i = 0; i < DIMENSION; i++) {   dotprod  += normal[i] * gradE[i]; }
    vnl_vector_fixed<double, DIMENSION> result;
    for (unsigned int i = 0; i < DIMENSION; i++) { result[i] = gradE[i] - normal[i] * dotprod; }
    return result;
  }

  void DeleteImages() override
  {
    ParticleImageDomain<T>::DeleteImages();
    m_VDBGradient = 0;
  }
  
protected:
  ParticleImageDomainWithGradients() {}
  virtual ~ParticleImageDomainWithGradients() {}

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleImageDomain<T>::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBGradient->activeVoxelCount() << std::endl;
  }

  openvdb::VectorGrid::Ptr GetVDBGradient() {
    return m_VDBGradient;
  }


private:



  inline VnlVectorType SampleGradientVnl(const PointType &p, int idx) const {
    return VnlVectorType(this->SampleGradient(p, idx).GetDataPointer());
  }
  inline VectorType SampleGradient(const PointType &p, int idx) const {
    if (this->IsInsideBuffer(p)) {
      const auto coord = this->ToVDBCoord(p);
      const auto _v = openvdb::tools::BoxSampler::sample(m_VDBGradient->tree(), coord);
      const VectorType v(_v.asPointer()); // This copies 3 floats from a VDB vector to a vnl vector
      return v;
    }
    else {
      itkExceptionMacro("Gradient queried for a Point, " << p << ", outside the given image domain.");
    }
  }


  openvdb::VectorGrid::Ptr m_VDBGradient;
};

} // end namespace itk
```


-------------------------------

Updated on 2021-12-13 at 17:35:59 +0000
