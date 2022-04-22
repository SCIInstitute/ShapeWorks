---
title: Libs/Optimize/ParticleSystem/ConstraintType.h

---

# Libs/Optimize/ParticleSystem/ConstraintType.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[DIMENSION](../Files/ConstraintType_8h.md#define-dimension)**  |




## Macros Documentation

### define DIMENSION

```cpp
#define DIMENSION 3
```


## Source code

```cpp
#pragma once

#define DIMENSION 3

namespace shapeworks {
  enum class ConstraintType : char {
    Sphere = 'S',
    CuttingPlane = 'C',
    FreeForm = 'F'
  };
}
```


-------------------------------

Updated on 2022-04-22 at 07:13:30 +0000
