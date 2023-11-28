---
title: Libs/Optimize/Domain/ParticleRegionDomain.h

---

# Libs/Optimize/Domain/ParticleRegionDomain.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleRegionDomain](../Classes/classshapeworks_1_1ParticleRegionDomain.md)**  |




## Source code

```cpp
#pragma once

#include "ParticleDomain.h"

namespace shapeworks {

class ParticleRegionDomain : public ParticleDomain {
 public:
  using Pointer = std::shared_ptr<ParticleRegionDomain>;

  typedef typename ParticleDomain::PointType PointType;

  virtual bool ApplyConstraints(PointType &p) const {
    bool changed = false;
    for (unsigned int i = 0; i < DIMENSION; i++) {
      if (p[i] < GetLowerBound()[i]) {
        changed = true;
        p[i] = GetLowerBound()[i];
      } else if (p[i] > GetUpperBound()[i]) {
        changed = true;
        p[i] = GetUpperBound()[i];
      }
    }
    return changed;
  }

  virtual const PointType &GetUpperBound() const { return m_UpperBound; }
  virtual const PointType &GetLowerBound() const { return m_LowerBound; }
  void SetUpperBound(const PointType _UpperBound) { m_UpperBound = _UpperBound; }
  void SetLowerBound(const PointType _LowerBound) { m_LowerBound = _LowerBound; }
  void SetRegion(const PointType &lowerBound, const PointType &upperBound) {
    SetLowerBound(lowerBound);
    SetUpperBound(upperBound);
  }

 protected:
  ParticleRegionDomain() {}
  virtual ~ParticleRegionDomain(){};

  void PrintSelf(std::ostream &os, itk::Indent indent) const {
    os << "LowerBound = " << GetLowerBound() << std::endl;
    os << "UpperBound = " << GetUpperBound() << std::endl;
  }

  inline bool IsInsideBuffer(const PointType &p) const {
    for (int i = 0; i < DIMENSION; i++) {
      if (p[i] < m_LowerBound[i] || p[i] > m_UpperBound[i]) {
        return false;
      }
    }
    return true;
  }

 private:
  PointType m_LowerBound;
  PointType m_UpperBound;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-11-28 at 06:02:30 +0000
