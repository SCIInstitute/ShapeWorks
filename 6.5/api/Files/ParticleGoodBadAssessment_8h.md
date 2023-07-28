---
title: Libs/Optimize/Utils/ParticleGoodBadAssessment.h

---

# Libs/Optimize/Utils/ParticleGoodBadAssessment.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleGoodBadAssessment](../Classes/classshapeworks_1_1ParticleGoodBadAssessment.md)** <br>Performs good/bad points assessment and reports the bad positions of the particle system.  |




## Source code

```cpp
#pragma once

#include "Libs/Optimize/Container/MeanCurvatureContainer.h"
#include "ParticleSystem.h"

namespace shapeworks {

class ParticleGoodBadAssessment {
 public:
  using MeanCurvatureCacheType = MeanCurvatureContainer<float, 3>;

  void set_domains_per_shape(int i) { domains_per_shape_ = i; }

  void set_criterion_angle(double a) { criterion_angle_ = a; }

  std::vector<std::vector<int>> run_assessment(const ParticleSystem* ps,
                                               MeanCurvatureCacheType* mean_curvature_cache);

 private:
  vnl_matrix<double> compute_particles_normals(int d, const ParticleSystem* ps);
  int domains_per_shape_ = 1;
  double criterion_angle_ = 90.0;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-07-28 at 04:11:56 +0000
