---
title: Libs/Particles/ParticleNormalEvaluation.h

---

# Libs/Particles/ParticleNormalEvaluation.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleNormalEvaluation](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md)**  |




## Source code

```cpp
#pragma once

#include <Particles/ParticleSystemEvaluation.h>

#include <memory>

namespace shapeworks {


class VtkMeshWrapper;
class ParticleNormalEvaluation {
 public:
  static std::vector<bool> evaluate_particle_normals(const Eigen::MatrixXd& particles, const Eigen::MatrixXd& normals,
                                                     double max_angle_degrees);

  static Eigen::MatrixXd compute_particle_normals(const Eigen::MatrixXd& particles,
                                                  std::vector<std::shared_ptr<VtkMeshWrapper>> meshes);

 private:
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-04-29 at 04:54:50 +0000
