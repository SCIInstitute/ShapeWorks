---
title: Libs/Optimize/Neighborhood/ParticleNeighborhood.h

---

# Libs/Optimize/Neighborhood/ParticleNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**  |




## Source code

```cpp
#pragma once

#include <vector>

#include "Libs/Optimize/Container/GenericContainer.h"
#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "ParticlePointIndexPair.h"
#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace shapeworks {
class ParticleNeighborhood : public itk::DataObject {
 public:
  constexpr static unsigned int VDimension = 3;
  typedef ParticleNeighborhood Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleNeighborhood, DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  typedef itk::Point<double, VDimension> PointType;

  using DomainType = shapeworks::ParticleDomain;

  typedef GenericContainer<PointType> PointContainerType;

  typedef std::vector<ParticlePointIndexPair> PointVectorType;

  itkSetObjectMacro(PointContainer, PointContainerType);
  itkGetConstObjectMacro(PointContainer, PointContainerType);

  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual PointVectorType FindNeighborhoodPoints(const PointType&, int idx, std::vector<double>&, std::vector<double>&,
                                                 double) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
  }
  virtual unsigned int FindNeighborhoodPoints(const PointType&, int idx, double, PointVectorType&) const {
    itkExceptionMacro("No algorithm for finding neighbors has been specified.");
    return 0;
  }

  // itkSetObjectMacro(Domain, DomainType);
  // itkGetConstObjectMacro(Domain, DomainType);
  virtual void SetDomain(DomainType::Pointer domain) {
    m_Domain = domain;
    this->Modified();
  };
  DomainType::Pointer GetDomain() const { return m_Domain; };

  virtual void AddPosition(const PointType& p, unsigned int idx, int threadId = 0) {}
  virtual void SetPosition(const PointType& p, unsigned int idx, int threadId = 0) {}
  virtual void RemovePosition(unsigned int idx, int threadId = 0) {}

 protected:
  ParticleNeighborhood() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const { Superclass::PrintSelf(os, indent); }
  virtual ~ParticleNeighborhood(){};

 private:
  ParticleNeighborhood(const Self&);  // purposely not implemented
  void operator=(const Self&);        // purposely not implemented

  typename PointContainerType::Pointer m_PointContainer;
  typename DomainType::Pointer m_Domain;
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2024-07-14 at 16:33:03 +0000
