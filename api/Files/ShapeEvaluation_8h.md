---
title: Libs/Particles/ShapeEvaluation.h

---

# Libs/Particles/ShapeEvaluation.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeEvaluation](../Classes/classshapeworks_1_1ShapeEvaluation.md)**  |




## Source code

```cpp
#pragma once

#include <string>
#include <Eigen/Core>
#include "ParticleShapeStatistics.h"
#include "ParticleSystem.h"

namespace shapeworks {

class ShapeEvaluation
{
public:
  static const unsigned VDimension = 3;
  ShapeEvaluation();

  static double ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullCompactness(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static double ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullGeneralization(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);

  static double ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo = "");

  static Eigen::VectorXd ComputeFullSpecificity(const ParticleSystem &particleSystem, std::function<void(float)> progress_callback = nullptr);
};
}
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000
