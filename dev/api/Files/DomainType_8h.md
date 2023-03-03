---
title: Libs/Optimize/DomainType.h

---

# Libs/Optimize/DomainType.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[DIMENSION](../Files/DomainType_8h.md#define-dimension)**  |




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
  enum class DomainType : char {
    Image = 'I',
    Mesh = 'M',
    Contour = 'C'
  };
}
```


-------------------------------

Updated on 2023-03-03 at 23:21:16 +0000
