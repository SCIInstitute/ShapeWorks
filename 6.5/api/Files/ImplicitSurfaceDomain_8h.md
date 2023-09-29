---
title: Libs/Optimize/Domain/ImplicitSurfaceDomain.h

---

# Libs/Optimize/Domain/ImplicitSurfaceDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ImplicitSurfaceDomain](../Classes/classshapeworks_1_1ImplicitSurfaceDomain.md)**  |




## Source code

```cpp
#pragma once

#include "ImageDomainWithCurvature.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_math.h"

namespace shapeworks {
template <class T>
class ImplicitSurfaceDomain : public ImageDomainWithCurvature<T> {
 public:
  typedef ImageDomainWithCurvature<T> Superclass;
  typedef std::shared_ptr<ImplicitSurfaceDomain> Pointer;

  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  virtual void SetTolerance(const T _Tolerance) {
    if (this->m_Tolerance != _Tolerance) {
      this->m_Tolerance = _Tolerance;
      // this->Modified();
    }
  }
  virtual T GetTolerance() { return this->m_Tolerance; }

  shapeworks::DomainType GetDomainType() const override { return shapeworks::DomainType::Image; }

  virtual bool ApplyConstraints(PointType& p, int idx, bool dbg = false) const override {
    // First apply and constraints imposed by superclasses.  This will
    // guarantee the point starts in the correct image domain.
    bool flag = Superclass::ApplyConstraints(p);

    unsigned int k = 0;
    double mult = 1.0;

    const T epsilon = m_Tolerance * 0.001;
    T f = this->Sample(p);

    T gradmag = 1.0;
    while (fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
    //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
    {
      PointType p_old = p;
      // vnl_vector_fixed<T, DIMENSION> grad = -this->SampleGradientAtPoint(p);
      vnl_vector_fixed<T, DIMENSION> gradf = this->SampleGradientAtPoint(p, idx);
      vnl_vector_fixed<double, DIMENSION> grad;
      grad[0] = double(gradf[0]);
      grad[1] = double(gradf[1]);
      grad[2] = double(gradf[2]);

      gradmag = grad.magnitude();
      // vnl_vector_fixed<T, DIMENSION> vec = grad * (f / (gradmag + epsilon));
      vnl_vector_fixed<double, DIMENSION> vec = grad * (double(f) / (gradmag + double(epsilon)));

      for (unsigned int i = 0; i < DIMENSION; i++) {
        p[i] -= vec[i];
      }

      f = this->Sample(p);

      // Raise the tolerance if we have done too many iterations.
      k++;
      if (k > 10000) {
        mult *= 2.0;
        k = 0;
      }
    }  // end while
    return flag;
  };

  inline PointType UpdateParticlePosition(const PointType& point, int idx,
                                          vnl_vector_fixed<double, DIMENSION>& update) const override {
    PointType newpoint;

    for (unsigned int i = 0; i < 3; i++) {
      newpoint[i] = point[i] - update[i];
    }

    ApplyConstraints(newpoint, idx);

    return newpoint;
  }


  PointType GetZeroCrossingPoint() const override {
    PointType p;
    // TODO Hong
    // Return point that doesn't violate plane constraints.
    return p;
  }

  ImplicitSurfaceDomain() : m_Tolerance(1.0e-4) { }
  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ImplicitSurfaceDomain(){};

 private:
  T m_Tolerance;


};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2023-09-29 at 06:24:12 +0000
