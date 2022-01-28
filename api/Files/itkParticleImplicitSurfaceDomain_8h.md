---
title: Libs/Optimize/ParticleSystem/itkParticleImplicitSurfaceDomain.h

---

# Libs/Optimize/ParticleSystem/itkParticleImplicitSurfaceDomain.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleImplicitSurfaceDomain](../Classes/classitk_1_1ParticleImplicitSurfaceDomain.md)**  |




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

#include "itkParticleImageDomainWithCurvature.h"

#include "vnl/vnl_inverse.h"

#include "TriMesh.h"
#include "TriMesh_algo.h"

#include "meshFIM.h"

namespace itk
{
template <class T>
class ParticleImplicitSurfaceDomain : public ParticleImageDomainWithCurvature<T>
{
public:
  typedef ParticleImageDomainWithCurvature<T> Superclass;
  typedef SmartPointer<ParticleImplicitSurfaceDomain>  Pointer;

  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  itkNewMacro(ParticleImplicitSurfaceDomain);

  virtual void SetTolerance(const T _Tolerance) {
    if (this->m_Tolerance != _Tolerance)
    {
      this->m_Tolerance = _Tolerance;
      this->Modified();
    }
  }
  virtual T GetTolerance() {
    return this->m_Tolerance;
  }

  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Image;
  }

  virtual bool ApplyConstraints(PointType &p, int idx, bool dbg = false) const override;


  inline PointType UpdateParticlePosition(const PointType &point, int idx, vnl_vector_fixed<double, DIMENSION> &update) const override {
    PointType newpoint;

    // Master merge conflict

    //vnl_vector_fixed<float, DIMENSION> negativeUpdate;
    //for (unsigned int i = 0; i < DIMENSION; i++) { negativeUpdate[i] = -update[i]; }
    //for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] + negativeUpdate[i]; }

    for (unsigned int i = 0; i < 3; i++) { newpoint[i] = point[i] - update[i]; }


    //for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] - update[i]; }

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

  void SetMesh(TriMesh *mesh);
  void SetFeaMesh(const char *feaFile);
  void SetFeaGrad(const char *feaGradFile);
  void SetFids(const char *fidsFile);
  meshFIM* GetMesh()
  {
      return m_mesh;
  }
  meshFIM* GetMesh() const
  {
      return m_mesh;
  }

    PointType GetZeroCrossingPoint() const override {
      PointType p;
      // TODO Hong
      // Return point that doesn't violate plane constraints.
      return p;
    }

protected:
  ParticleImplicitSurfaceDomain() : m_Tolerance(1.0e-4)
  {
    m_mesh = NULL;
  }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ParticleImplicitSurfaceDomain() {};

private:
  T m_Tolerance;
  
  meshFIM *m_mesh;

};

} // end namespace itk

#include "itkParticleImplicitSurfaceDomain.txx"
```


-------------------------------

Updated on 2022-01-28 at 21:13:54 +0000
