---
title: Libs/Optimize/ParticleSystem/MeshGeoEntry.h

---

# Libs/Optimize/ParticleSystem/MeshGeoEntry.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[shapeworks::MeshGeoEntry](../Classes/structshapeworks_1_1MeshGeoEntry.md)**  |




## Source code

```cpp
#pragma once

#include "robin_hood.h"
#include <Eigen/Dense>

namespace shapeworks {
struct MeshGeoEntry {
  // in "Full" mode, the entry contains geodesics to every vertex. Access via `data_full`
  // in "Partial" mode, the entry only contains geodesics upto max_dist. Access via `data_partial`
  enum Mode {
    Full,
    Partial,
  };

  Mode mode{Mode::Partial};

  double max_dist{0.0};
  robin_hood::unordered_flat_map<int, Eigen::Vector3d> data_partial;
  std::array<Eigen::VectorXd, 3> data_full;

  void clear() {
    mode = Mode::Partial;

    max_dist = 0.0;

    // calling `data_partial.clear()` doesn't free the backing memory, so we have to swap to an empty
    robin_hood::unordered_flat_map<int, Eigen::Vector3d> new_data_partial;
    std::swap(new_data_partial, data_partial);

    data_full[0].resize(0);
    data_full[1].resize(0);
    data_full[2].resize(0);
  }

  bool is_full_mode() const {
    return mode == Mode::Full;
  }

  void update_max_dist() {
    assert(is_full_mode()); // the caller most likely has a more efficient way to compute this if partial mode
    const auto max0 = data_full[0].maxCoeff();
    const auto max1 = data_full[1].maxCoeff();
    const auto max2 = data_full[2].maxCoeff();
    max_dist = std::max({max0, max1, max2});
  }

  bool has_entry(int target) {
    return is_full_mode() || data_partial.find(target) != data_partial.end();
  }
};

};
```


-------------------------------

Updated on 2021-11-17 at 00:54:21 +0000
