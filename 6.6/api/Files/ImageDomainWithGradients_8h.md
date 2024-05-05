---
title: Libs/Optimize/Domain/ImageDomainWithGradients.h

---

# Libs/Optimize/Domain/ImageDomainWithGradients.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ImageDomainWithGradients](../Classes/classshapeworks_1_1ImageDomainWithGradients.md)**  |




## Source code

```cpp
#pragma once

#include "ImageDomain.h"
#include "itkFixedArray.h"
#include "itkGradientImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"

namespace shapeworks {
template <class T>
class ImageDomainWithGradients : public ImageDomain<T> {
 public:
  using Pointer = std::shared_ptr<ImageDomainWithGradients<T>>;

  typedef typename ImageDomain<T>::PointType PointType;
  typedef typename ImageDomain<T>::ImageType ImageType;

  typedef itk::FixedArray<T, DIMENSION> VectorType;
  typedef vnl_vector_fixed<T, DIMENSION> VnlVectorType;

  void SetImage(ImageType* I, double narrow_band) {
    ImageDomain<T>::SetImage(I, narrow_band);
    m_VDBGradient = openvdb::tools::gradient(*this->GetVDBImage());
  }

  inline vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType& p, int idx) const {
    return this->SampleGradientVnl(p, idx);
  }

  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType& p, int idx) const {
    vnl_vector_fixed<float, DIMENSION> grad = this->SampleGradientVnl(p, idx);
    return grad.normalize();
  }

  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE,
                                                                    const PointType& pos, int idx) const override {
    double dotprod = 0.0;
    VnlVectorType normal = this->SampleNormalAtPoint(pos, idx);
    for (unsigned int i = 0; i < DIMENSION; i++) {
      dotprod += normal[i] * gradE[i];
    }
    vnl_vector_fixed<double, DIMENSION> result;
    for (unsigned int i = 0; i < DIMENSION; i++) {
      result[i] = gradE[i] - normal[i] * dotprod;
    }
    return result;
  }

  void DeleteImages() override {
    ImageDomain<T>::DeleteImages();
    m_VDBGradient = 0;
  }

 protected:
  ImageDomainWithGradients() {}
  virtual ~ImageDomainWithGradients() {}

  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    ImageDomain<T>::PrintSelf(os, indent);
    os << indent << "VDB Active Voxels = " << m_VDBGradient->activeVoxelCount() << std::endl;
  }

  openvdb::VectorGrid::Ptr GetVDBGradient() { return m_VDBGradient; }

 private:
  inline VnlVectorType SampleGradientVnl(const PointType& p, int idx) const {
    return VnlVectorType(this->SampleGradient(p, idx).GetDataPointer());
  }
  inline VectorType SampleGradient(const PointType& p, int idx) const {
    if (this->IsInsideBuffer(p)) {
      const auto coord = this->ToVDBCoord(p);
      const auto _v = openvdb::tools::BoxSampler::sample(m_VDBGradient->tree(), coord);
      const VectorType v(_v.asPointer());  // This copies 3 floats from a VDB vector to a vnl vector
      return v;
    } else {
      std::ostringstream message;
      message << "Gradient queried for a Point, " << p << ", outside the given image domain.";
      throw std::runtime_error(message.str());
    }
  }

  openvdb::VectorGrid::Ptr m_VDBGradient;
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2024-05-05 at 15:30:46 +0000
