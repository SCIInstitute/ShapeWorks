---
title: Libs/Particles/ParticleSystem.h

---

# Libs/Particles/ParticleSystem.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md)**  |




## Source code

```cpp
#pragma once

#include <vector>
#include <Eigen/Core>

#include "itkParticlePositionReader.h"
#include "Shapeworks.h"

namespace shapeworks {

class ParticleSystem 
{
public:
  ParticleSystem(const std::vector<std::string> &paths);

  ParticleSystem(const Eigen::MatrixXd &matrix);

  const Eigen::MatrixXd &Particles() const
  {
    return P;
  };

  const std::vector<std::string> &Paths() const
  {
    return paths;
  }

  int N() const
  {
    return P.cols();
  }

  int D() const
  {
    return P.rows();
  }

  bool ExactCompare(const ParticleSystem& other) const;

  bool EvaluationCompare(const ParticleSystem& other) const;

private:
  friend struct SharedCommandData;

  ParticleSystem() {} // only for use by SharedCommandData since a ParticleSystem should always be valid, never "empty"

  Eigen::MatrixXd P;
  std::vector<std::string> paths;
};
}
```


-------------------------------

Updated on 2021-12-13 at 17:42:54 +0000
