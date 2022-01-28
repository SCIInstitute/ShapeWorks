---
title: Libs/Optimize/ParticleSystem/itkParticleImageDomainWithGradN.h

---

# Libs/Optimize/ParticleSystem/itkParticleImageDomainWithGradN.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleImageDomainWithGradN](../Classes/classitk_1_1ParticleImageDomainWithGradN.md)**  |




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

#include "itkParticleImageDomainWithGradients.h"
#include "itkImage.h"
#include "itkGradientImageFilter.h"
#include "itkFixedArray.h"
#include "itkImageDuplicator.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkDerivativeImageFilter.h"
#include "vnl/vnl_matrix_fixed.h"

namespace itk
{
template <class T>
class ParticleImageDomainWithGradN : public ParticleImageDomainWithGradients<T>
{
public:
  typedef ParticleImageDomainWithGradients<T> Superclass;
  typedef SmartPointer<ParticleImageDomainWithGradN>  Pointer;

  typedef typename Superclass::PointType PointType;
  typedef typename Superclass::ImageType ImageType;

  typedef typename Superclass::GradNType GradNType;
  typedef typename Superclass::GradNType VnlMatrixType;

  void SetImage(ImageType *I, double narrow_band)
  {
    Superclass::SetImage(I, narrow_band);

    const auto grad = this->GetVDBGradient();

    // Compute the gradient of normals component-wise
    for(int i=0; i<3; i++) {
      auto norm_i = openvdb::FloatGrid::create();
      norm_i->setTransform(this->transform());
      auto norm_i_accessor = norm_i->getAccessor();
      for(openvdb::VectorGrid::ValueOnCIter it = grad->cbeginValueOn(); it.test(); ++it) {
        const openvdb::Vec3f& v = *it;
        norm_i_accessor.setValue(it.getCoord(), v[i] / v.length());
      }

      m_VDBGradNorms[i] = openvdb::tools::gradient(*norm_i);
    }
  } // end setimage

  inline GradNType SampleGradNAtPoint(const PointType &p, int idx) const override
  {
    const auto coord = this->ToVDBCoord(p);

    GradNType grad_n;
    for(int i=0; i<3; i++) {
      auto grad_ni = openvdb::tools::BoxSampler::sample(m_VDBGradNorms[i]->tree(), coord);
      grad_n.set(i, 0, grad_ni[0]);
      grad_n.set(i, 1, grad_ni[1]);
      grad_n.set(i, 2, grad_ni[2]);
    }
    return grad_n;
  }

  void DeletePartialDerivativeImages() override
  {
    for (unsigned int i = 0; i < DIMENSION; i++) {
      m_VDBGradNorms[i] = 0;
    }
  }

  void DeleteImages() override
  {
    Superclass::DeleteImages();
    DeletePartialDerivativeImages();
  }

protected:
  ParticleImageDomainWithGradN() {}
  virtual ~ParticleImageDomainWithGradN() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

  
private:
  typename openvdb::VectorGrid::Ptr m_VDBGradNorms[3];
};

} // end namespace itk
```


-------------------------------

Updated on 2022-01-28 at 21:13:54 +0000
