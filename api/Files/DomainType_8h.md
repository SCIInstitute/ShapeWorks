---
title: Libs/Optimize/ParticleSystem/DomainType.h

---

# Libs/Optimize/ParticleSystem/DomainType.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-01-27 at 02:24:32 +0000
