---
title: Libs/Particles/ParticleSystemEvaluation.h

---

# Libs/Particles/ParticleSystemEvaluation.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Core>
#include <vector>

#include "Shapeworks.h"
#include "itkParticlePositionReader.h"

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  ParticleSystemEvaluation(const std::vector<std::string>& paths);

  // Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  ParticleSystemEvaluation(const Eigen::MatrixXd& matrix);

  const Eigen::MatrixXd& Particles() const { return P; };

  const std::vector<std::string>& Paths() const { return paths; }

  int N() const { return P.cols(); }

  int D() const { return P.rows(); }

  bool ExactCompare(const ParticleSystemEvaluation& other) const;

  bool EvaluationCompare(const ParticleSystemEvaluation& other) const;

  static bool ReadParticleFile(std::string filename, Eigen::VectorXd& points);

 private:
  friend struct SharedCommandData;

  ParticleSystemEvaluation() {
  }  // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd P;
  std::vector<std::string> paths;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-05-04 at 20:03:05 +0000
