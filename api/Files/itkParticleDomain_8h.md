---
title: Libs/Optimize/ParticleSystem/itkParticleDomain.h

---

# Libs/Optimize/ParticleSystem/itkParticleDomain.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::ParticleDomain](../Classes/classitk_1_1ParticleDomain.md)**  |




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

#include "itkDataObject.h"
#include "itkPoint.h"

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"

#include "DomainType.h"
#include "Constraints.h"

namespace itk
{
class ParticleDomain : public DataObject
{
public:
  typedef SmartPointer<ParticleDomain>  Pointer;

  typedef Point<double, DIMENSION> PointType;
  typedef vnl_matrix_fixed<float, DIMENSION, DIMENSION> GradNType;

  virtual bool ApplyConstraints(PointType& p, int idx, bool dbg = false) const = 0;

  //todo update should be const?
  virtual PointType UpdateParticlePosition(const PointType &point, int idx, vnl_vector_fixed<double, DIMENSION> &update) const = 0;

  virtual void InvalidateParticlePosition(int idx) const {
  }

  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos, int idx) const = 0;

  virtual vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point, int idx) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point, int idx) const = 0;
  virtual GradNType SampleGradNAtPoint(const PointType &p, int idx) const = 0;

  virtual double Distance(const PointType &a, int idx_a,
                          const PointType &b, int idx_b,
                          vnl_vector_fixed<double, DIMENSION> *out_grad=nullptr) const {
    if(out_grad != nullptr) {
      for(int i=0; i<DIMENSION; i++) {
        (*out_grad)[i] = a[i] - b[i];
      }
    }
    return a.EuclideanDistanceTo(b);
  }
  virtual double SquaredDistance(const PointType &a, int idx_a,
                                 const PointType &b, int idx_b) const {
    return a.SquaredEuclideanDistanceTo(b);
  }
  virtual bool IsWithinDistance(const PointType &a, int idx_a,
                                const PointType &b, int idx_b,
                                double test_dist, double& distance) const {
    distance = this->Distance(a, idx_a, b, idx_b);
    return distance < test_dist;
  }

  virtual double GetCurvature(const PointType &p, int idx) const = 0;
  virtual double GetSurfaceMeanCurvature() const = 0;
  virtual double GetSurfaceStdDevCurvature() const = 0;

  virtual const PointType& GetLowerBound() const = 0;
  virtual const PointType& GetUpperBound() const = 0;

  virtual PointType GetZeroCrossingPoint() const = 0;
  virtual double GetSurfaceArea() const = 0;

  virtual PointType GetValidLocationNear(PointType p) const = 0;
  virtual double GetMaxDiameter() const = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;
  virtual void UpdateZeroCrossingPoint() = 0;

  bool IsDomainFixed() const {
    return m_FixedDomain;
  }
  virtual shapeworks::DomainType GetDomainType() const = 0;

  std::shared_ptr<Constraints> GetConstraints() const {return constraints;}

  // Use `random` to advance a particle and return a new position
  virtual PointType GetPositionAfterSplit(const PointType& pt,
                                          const vnl_vector_fixed<double, 3>& random, double epsilon) const {
    // todo this has been copied from itkParticleSystem::AdvancedAllParticleSplitting.
    //  Ideally, we should compute a direction that is "consistent" depending on the domain type and use the
    //  `UpdateParticlePosition` API to advance the particle. See ContourDomain for an example. Leaving this be for
    //  now because we'd have to retest all MeshDomain and ImageDomain use cases if this behaviour changes.
    PointType new_pt;
    for (unsigned int k = 0; k < 3; k++) {
      new_pt[k] = pt[k] + epsilon * random[k] / 5.;
    }
    return new_pt;
  }

  void SetDomainID(int id) {
    this->m_DomainID = id;
  }

  void SetDomainName(std::string name) {
    this->m_DomainName = name;
  }

protected:

  // is this a fixed domain or not?  We start as fixed and if an image or mesh is set, we set this to false
  bool m_FixedDomain{true};

  int m_DomainID{-1};
  std::string m_DomainName;

  ParticleDomain() {
      this->constraints = std::make_shared<Constraints>();
  }
  virtual ~ParticleDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  std::shared_ptr<Constraints> constraints;

private:
  ParticleDomain(const ParticleDomain&); //purposely not implemented
  void operator=(const ParticleDomain&); //purposely not implemented


};

} // end namespace itk
```


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000
