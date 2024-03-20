---
title: Libs/Optimize/Neighborhood/ParticlePointIndexPair.h

---

# Libs/Optimize/Neighborhood/ParticlePointIndexPair.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[shapeworks::ParticlePointIndexPair](../Classes/structshapeworks_1_1ParticlePointIndexPair.md)**  |




## Source code

```cpp
#pragma once

#include "itkPoint.h"
namespace shapeworks {
struct ParticlePointIndexPair {
  typedef itk::Point<double, 3> PointType;
  ParticlePointIndexPair() {}
  ParticlePointIndexPair(const PointType& p, unsigned int i = 0) {
    Point = p;
    Index = i;
  }
  ParticlePointIndexPair(const ParticlePointIndexPair& o) { this->operator=(o); }
  const ParticlePointIndexPair& operator=(const ParticlePointIndexPair& o) {
    Point = o.Point;
    Index = o.Index;
    return *this;
  }
  PointType Point;
  unsigned int Index;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-03-20 at 15:59:40 +0000
