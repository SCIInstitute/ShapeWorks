---
title: Libs/Optimize/DomainType.h

---

# Libs/Optimize/DomainType.h



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

Updated on 2022-11-16 at 06:00:36 +0000
