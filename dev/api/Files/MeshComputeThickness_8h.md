---
title: Libs/Mesh/MeshComputeThickness.h

---

# Libs/Mesh/MeshComputeThickness.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[shapeworks::mesh](../Namespaces/namespaceshapeworks_1_1mesh.md)**  |




## Source code

```cpp
#pragma once

#include <Image.h>
#include <Mesh.h>

namespace shapeworks::mesh {

void compute_thickness(Mesh &mesh, Image &image, Image *dt, double max_dist, double median_radius,
                       std::string distance_mesh);

}  // namespace shapeworks::mesh
```


-------------------------------

Updated on 2023-11-09 at 17:05:57 +0000
