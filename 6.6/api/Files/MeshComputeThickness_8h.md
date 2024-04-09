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

Mesh compute_inner_mesh(const Mesh &mesh, std::string array_name);

void summarize_internal_intensities(Mesh &outer_mesh, Mesh &inner_mesh, Image &image);

}  // namespace shapeworks::mesh
```


-------------------------------

Updated on 2024-04-09 at 23:32:48 +0000
