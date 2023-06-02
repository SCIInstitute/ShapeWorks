---
title: Libs/Optimize/Domain/DomainType.h

---

# Libs/Optimize/Domain/DomainType.h



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

Updated on 2023-06-02 at 07:19:26 +0000
