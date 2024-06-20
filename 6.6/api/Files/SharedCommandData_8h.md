---
title: Applications/shapeworks/SharedCommandData.h

---

# Applications/shapeworks/SharedCommandData.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[shapeworks::SharedCommandData](../Classes/structshapeworks_1_1SharedCommandData.md)**  |




## Source code

```cpp
#pragma once

#include <string>

#include "Image.h"
#include "Mesh.h"
#include "ParticleSystemEvaluation.h"
#include "Shapeworks.h"

namespace shapeworks {

struct SharedCommandData
{
  Image image;
  std::unique_ptr<Mesh> mesh;
  PhysicalRegion region;
  ParticleSystemEvaluation particleSystem;
  Field field;

  bool validImage() const { return image.itk_image_ != nullptr; }
  bool validMesh() const { return mesh != nullptr; }
  bool validParticleSystem() const {return particleSystem.num_samples() >= 1 && particleSystem.num_dims() >= 1; }
};


} // shapeworks
```


-------------------------------

Updated on 2024-06-20 at 08:12:54 +0000
