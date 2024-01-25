---
title: Libs/Optimize/Domain/ImageDomainWithGradN.h

---

# Libs/Optimize/Domain/ImageDomainWithGradN.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ImageDomainWithGradN](../Classes/classshapeworks_1_1ImageDomainWithGradN.md)**  |




## Source code

```cpp
#pragma once

#include "ImageDomainWithGradients.h"
#include "itkDerivativeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkFixedArray.h"
#include "itkGradientImageFilter.h"
#include "itkImage.h"
#include "itkImageDuplicator.h"
#include "vnl/vnl_matrix_fixed.h"

namespace shapeworks {
template <class T>
class ImageDomainWithGradN : public ImageDomainWithGradients<T> {
 public:
  typedef ImageDomainWithGradients<T> Superclass;
  using Pointer = std::shared_ptr<ImageDomainWithGradN>;

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::ImageType ImageType;

  typedef typename Superclass::GradNType GradNType;
  typedef typename Superclass::GradNType VnlMatrixType;

  void SetImage(ImageType* I, double narrow_band) {
    Superclass::SetImage(I, narrow_band);

    const auto grad = this->GetVDBGradient();

    // Compute the gradient of normals component-wise
    for (int i = 0; i < 3; i++) {
      auto norm_i = openvdb::FloatGrid::create();
      norm_i->setTransform(this->transform());
      auto norm_i_accessor = norm_i->getAccessor();
      for (openvdb::VectorGrid::ValueOnCIter it = grad->cbeginValueOn(); it.test(); ++it) {
        const openvdb::Vec3f& v = *it;
        norm_i_accessor.setValue(it.getCoord(), v[i] / v.length());
      }

      m_VDBGradNorms[i] = openvdb::tools::gradient(*norm_i);
    }
  }  // end setimage

  inline GradNType SampleGradNAtPoint(const PointType& p, int idx) const override {
    const auto coord = this->ToVDBCoord(p);

    GradNType grad_n;
    for (int i = 0; i < 3; i++) {
      auto grad_ni = openvdb::tools::BoxSampler::sample(m_VDBGradNorms[i]->tree(), coord);
      grad_n.set(i, 0, grad_ni[0]);
      grad_n.set(i, 1, grad_ni[1]);
      grad_n.set(i, 2, grad_ni[2]);
    }
    return grad_n;
  }

  void DeletePartialDerivativeImages() override {
    for (unsigned int i = 0; i < DIMENSION; i++) {
      m_VDBGradNorms[i] = 0;
    }
  }

  void DeleteImages() override {
    Superclass::DeleteImages();
    DeletePartialDerivativeImages();
  }

 protected:
  ImageDomainWithGradN() {}
  virtual ~ImageDomainWithGradN(){};

  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }

 private:
  typename openvdb::VectorGrid::Ptr m_VDBGradNorms[3];
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2024-01-25 at 03:19:26 +0000
