---
title: Libs/Optimize/Constraints/ConstraintType.h

---

# Libs/Optimize/Constraints/ConstraintType.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

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

Updated on 2023-04-29 at 18:09:09 +0000
