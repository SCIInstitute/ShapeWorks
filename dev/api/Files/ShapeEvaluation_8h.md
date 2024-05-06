---
title: Libs/Particles/ShapeEvaluation.h

---

# Libs/Particles/ShapeEvaluation.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeEvaluation](../Classes/classshapeworks_1_1ShapeEvaluation.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Core>
#include <string>

#include "ParticleSystemEvaluation.h"

namespace shapeworks {

class ShapeEvaluation {
 public:
  ShapeEvaluation();

  static double compute_compactness(const ParticleSystemEvaluation& particle_system, int num_modes,
                                    const std::string& save_to = "");

  static Eigen::VectorXd compute_full_compactness(const ParticleSystemEvaluation& particle_system,
                                                  std::function<void(float)> progress_callback = nullptr);

  static double compute_generalization(const ParticleSystemEvaluation& particle_system, int num_modes,
                                       const std::string& save_to = "");

  static Eigen::VectorXd compute_full_generalization(const ParticleSystemEvaluation& particle_system,
                                                     std::function<void(float)> progress_callback = nullptr);

  static double compute_specificity(const ParticleSystemEvaluation& particle_system, int num_mode,
                                    const std::string& save_to = "");

  static Eigen::VectorXd compute_full_specificity(const ParticleSystemEvaluation& particle_system,
                                                  std::function<void(float)> progress_callback = nullptr);
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-06 at 07:59:19 +0000
