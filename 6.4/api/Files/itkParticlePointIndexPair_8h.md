---
title: Libs/Optimize/itkParticlePointIndexPair.h

---

# Libs/Optimize/itkParticlePointIndexPair.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[itk::ParticlePointIndexPair](../Classes/structitk_1_1ParticlePointIndexPair.md)**  |




## Source code

```cpp
#pragma once

#include "itkPoint.h"
namespace itk {
template <unsigned int VDimension>
struct ParticlePointIndexPair {
  typedef itk::Point<double, VDimension> PointType;
  ParticlePointIndexPair() {}
  ParticlePointIndexPair(const PointType &p, unsigned int i = 0) {
    Point = p;
    Index = i;
  }
  ParticlePointIndexPair(const ParticlePointIndexPair &o) { this->operator=(o); }
  const ParticlePointIndexPair &operator=(const ParticlePointIndexPair &o) {
    Point = o.Point;
    Index = o.Index;
    return *this;
  }
  PointType Point;
  unsigned int Index;
};

}  // namespace itk
```


-------------------------------

Updated on 2023-01-31 at 02:05:30 +0000
