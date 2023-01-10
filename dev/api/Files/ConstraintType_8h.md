---
title: Libs/Optimize/ConstraintType.h

---

# Libs/Optimize/ConstraintType.h



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

Updated on 2023-01-10 at 05:56:13 +0000
