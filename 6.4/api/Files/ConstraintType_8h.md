---
title: Libs/Optimize/ConstraintType.h

---

# Libs/Optimize/ConstraintType.h



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

Updated on 2023-03-24 at 08:53:23 +0000
