---
title: Libs/Particles/ParticleFile.h

---

# Libs/Particles/ParticleFile.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[shapeworks::particles](../Namespaces/namespaceshapeworks_1_1particles.md)**  |




## Source code

```cpp
#pragma once

#include <itkPoint.h>

#include <Eigen/Core>

namespace shapeworks {

namespace particles {

//---------------------------------------------------------------------------
Eigen::VectorXd read_particles(std::string filename);

//---------------------------------------------------------------------------
std::vector<itk::Point<double, 3>> read_particles_as_vector(std::string filename);

//---------------------------------------------------------------------------
void write_particles(std::string filename, const Eigen::VectorXd& points);

//---------------------------------------------------------------------------
void write_particles_from_vector(std::string filename, std::vector<itk::Point<double, 3>> points);

}  // namespace particles

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-10-13 at 16:12:29 +0000
