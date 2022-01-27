---
title: Applications/shapeworks/SharedCommandData.h

---

# Applications/shapeworks/SharedCommandData.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[shapeworks::SharedCommandData](../Classes/structshapeworks_1_1SharedCommandData.md)**  |




## Source code

```cpp
#pragma once

#include "Image.h"
#include "Mesh.h"
#include "ParticleSystem.h"
#include "Shapeworks.h"
#include <string>

namespace shapeworks {

struct SharedCommandData
{
  Image image;
  std::unique_ptr<Mesh> mesh;
  PhysicalRegion region;
  ParticleSystem particleSystem;
  Field field;

  bool validImage() const { return image.image != nullptr; }
  bool validMesh() const { return mesh != nullptr; }
  bool validParticleSystem() const {return particleSystem.N() >= 1 && particleSystem.D() >= 1; }
};


} // shapeworks
```


-------------------------------

Updated on 2022-01-27 at 02:24:32 +0000
