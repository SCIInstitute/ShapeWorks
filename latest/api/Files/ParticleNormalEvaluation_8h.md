---
title: Libs/Particles/ParticleNormalEvaluation.h

---

# Libs/Particles/ParticleNormalEvaluation.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleNormalEvaluation](../Classes/classshapeworks_1_1ParticleNormalEvaluation.md)**  |




## Source code

```cpp
#pragma once

#include <Libs/Particles/ParticleSystem.h>

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

Updated on 2022-07-23 at 17:50:04 -0600
