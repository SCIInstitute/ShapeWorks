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

  bool validImage() const { return image.itk_image_ != nullptr; }
  bool validMesh() const { return mesh != nullptr; }
  bool validParticleSystem() const {return particleSystem.N() >= 1 && particleSystem.D() >= 1; }
};


} // shapeworks
```


-------------------------------

Updated on 2022-10-24 at 22:34:05 +0000
