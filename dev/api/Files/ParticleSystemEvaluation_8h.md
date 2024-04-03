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

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  ParticleSystemEvaluation(const std::vector<std::string>& paths);

  // Initialize particle system from eigen matrix (rows=dimensions, cols=num_samples)
  ParticleSystemEvaluation(const Eigen::MatrixXd& matrix);

  const Eigen::MatrixXd& Particles() const { return matrix_; };

  const std::vector<std::string>& Paths() const { return paths_; }

  int N() const { return matrix_.cols(); }

  int D() const { return matrix_.rows(); }

  bool ExactCompare(const ParticleSystemEvaluation& other) const;

  bool EvaluationCompare(const ParticleSystemEvaluation& other) const;

  static bool ReadParticleFile(std::string filename, Eigen::VectorXd& points);

 private:
  friend struct SharedCommandData;

  ParticleSystemEvaluation() {
  }  // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd matrix_;
  std::vector<std::string> paths_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-04-03 at 19:47:52 +0000
