---
title: Libs/Optimize/ParticleSystem/ParticleImplicitSurfaceDomain.h

---

# Libs/Optimize/ParticleSystem/ParticleImplicitSurfaceDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleImplicitSurfaceDomain](../Classes/classshapeworks_1_1ParticleImplicitSurfaceDomain.md)**  |




## Source code

```cpp
#pragma once

#include "ParticleImageDomainWithCurvature.h"
#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "meshFIM.h"
#include "vnl/vnl_cross.h"
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_math.h"

namespace shapeworks {
template <class T>
class ParticleImplicitSurfaceDomain : public ParticleImageDomainWithCurvature<T> {
 public:
  typedef ParticleImageDomainWithCurvature<T> Superclass;
  typedef std::shared_ptr<ParticleImplicitSurfaceDomain> Pointer;

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

  virtual bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override {
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

  inline PointType UpdateParticlePosition(const PointType &point, int idx,
                                          vnl_vector_fixed<double, DIMENSION> &update) const override {
    PointType newpoint;

    // Master merge conflict

    // vnl_vector_fixed<float, DIMENSION> negativeUpdate;
    // for (unsigned int i = 0; i < DIMENSION; i++) { negativeUpdate[i] = -update[i]; }
    // for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] + negativeUpdate[i]; }

    for (unsigned int i = 0; i < 3; i++) {
      newpoint[i] = point[i] - update[i];
    }

    // for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] - update[i]; }

    // debuggg
    ApplyConstraints(newpoint, idx);

    // debuggg
    /*
    if(!this->GetConstraints()->IsAnyViolated(point) && this->GetConstraints()->IsAnyViolated(newpoint)){
        std::cerr << "####### Violation within apply constraints #######" << std::endl;
    }
    */

    /*
    if(point[2] >= 0 && newpoint[2] < 0){
        std::cerr << "NewPoint " << newpoint << std::endl;
        std::cerr << "Point " << point << std::endl;
        std::cerr << "Update " << update << std::endl;
    }
    */
    return newpoint;
  }

  void SetMesh(TriMesh *mesh) {
    m_mesh = new meshFIM();
    m_mesh->SetMesh(mesh);
  };

  void SetFeaMesh(const char *feaFile) { m_mesh->ReadFeatureFromFile(feaFile); };
  void SetFeaGrad(const char *feaGradFile) { m_mesh->ReadFeatureGradientFromFile(feaGradFile); };
  void SetFids(const char *fidsFile) {
    m_mesh->ReadFaceIndexMap(fidsFile);
    const typename ImageType::PointType orgn = this->GetOrigin();
    m_mesh->imageOrigin[0] = orgn[0];
    m_mesh->imageOrigin[1] = orgn[1];
    m_mesh->imageOrigin[2] = orgn[2];
    typename ImageType::RegionType::SizeType sz = this->GetSize();
    m_mesh->imageSize[0] = sz[0];
    m_mesh->imageSize[1] = sz[1];
    m_mesh->imageSize[2] = sz[2];
    typename ImageType::SpacingType sp = this->GetSpacing();
    m_mesh->imageSpacing[0] = sp[0];
    m_mesh->imageSpacing[1] = sp[1];
    m_mesh->imageSpacing[2] = sp[2];
    typename ImageType::RegionType::IndexType idx = this->GetIndex();
    m_mesh->imageIndex[0] = idx[0];
    m_mesh->imageIndex[1] = idx[1];
    m_mesh->imageIndex[2] = idx[2];
  };
  meshFIM *GetMesh() { return m_mesh; }
  meshFIM *GetMesh() const { return m_mesh; }

  PointType GetZeroCrossingPoint() const override {
    PointType p;
    // TODO Hong
    // Return point that doesn't violate plane constraints.
    return p;
  }

  ParticleImplicitSurfaceDomain() : m_Tolerance(1.0e-4) { m_mesh = NULL; }
  void PrintSelf(std::ostream &os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ParticleImplicitSurfaceDomain(){};

 private:
  T m_Tolerance;

  meshFIM *m_mesh;
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2022-06-10 at 06:10:26 +0000
