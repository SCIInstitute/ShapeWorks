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

#include <Libs/Mesh/Mesh.h>

#include <Eigen/Core>
#include <vector>

namespace shapeworks {

class ParticleSystemEvaluation {
 public:
  explicit ParticleSystemEvaluation(const std::vector<std::string>& paths);

  explicit ParticleSystemEvaluation(const Eigen::MatrixXd& matrix, int num_values_per_particle = 3);

  const Eigen::MatrixXd& get_matrix() const { return matrix_; };

  int get_num_values_per_particle() const { return num_values_per_particle_; }

  const std::vector<std::string>& get_paths() const { return paths_; }

  int num_samples() const { return matrix_.cols(); }

  int num_dims() const { return matrix_.rows(); }

  bool exact_compare(const ParticleSystemEvaluation& other) const;

  bool evaluation_compare(const ParticleSystemEvaluation& other) const;

  static bool read_particle_file(std::string filename, Eigen::VectorXd& points);

  void set_meshes(const std::vector<Mesh>& meshes);

  const std::vector<Mesh>& get_meshes() const { return meshes_; }

 private:
  friend struct SharedCommandData;

  ParticleSystemEvaluation() {}

  Eigen::MatrixXd matrix_;
  std::vector<std::string> paths_;
  int num_values_per_particle_ = 3;  // e.g. 3 for x/y/z, 4 for x/y/z/scalar, 1 for scalar-only
  std::vector<Mesh> meshes_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-04 at 08:01:27 +0000
