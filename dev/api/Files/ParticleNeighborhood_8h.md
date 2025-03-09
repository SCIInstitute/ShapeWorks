---
title: Libs/Optimize/Neighborhood/ParticleNeighborhood.h

---

# Libs/Optimize/Neighborhood/ParticleNeighborhood.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleNeighborhood](../Classes/classshapeworks_1_1ParticleNeighborhood.md)**  |




## Source code

```cpp
#pragma once

#include <vector>

#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "ParticlePointIndexPair.h"

namespace shapeworks {
class ParticleSystem;

class ParticleNeighborhood {
 public:
  explicit ParticleNeighborhood(ParticleSystem* ps, int domain_id = -1) : ps_(ps), domain_id_(domain_id) {}

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights,
                                                               std::vector<double>& distances, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               std::vector<double>& weights, double radius);

  std::vector<ParticlePointIndexPair> find_neighborhood_points(const itk::Point<double, 3>& position, int id,
                                                               double radius);

  void set_weighting_enabled(bool is_enabled) { weighting_enabled_ = is_enabled; }

  bool is_weighting_enabled() const { return weighting_enabled_; }

  void set_force_euclidean(bool is_enabled) { force_euclidean_ = is_enabled; }

  bool is_force_euclidean() const { return force_euclidean_; }

  void set_domain(ParticleDomain::Pointer domain) { domain_ = domain; };
  ParticleDomain::Pointer get_domain() const { return domain_; };

  void set_domain_id(int id) { domain_id_ = id; }

 private:
  std::pair<std::vector<ParticlePointIndexPair>, std::vector<double>> get_points_in_sphere(
      const itk::Point<double, 3>& position, int id, double radius);

  ParticleSystem* ps_;
  ParticleDomain::Pointer domain_;
  int domain_id_{-1};
  double flat_cutoff_{0.3};
  bool weighting_enabled_{true};
  bool force_euclidean_{false};
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2025-03-09 at 20:10:15 +0000
