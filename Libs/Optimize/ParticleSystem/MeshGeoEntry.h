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
  robin_hood::unordered_flat_map<int, double> data_partial;
  Eigen::VectorXd data_full;

  void clear() {
    mode = Mode::Partial;

    max_dist = 0.0;

    // calling `data_partial.clear()` doesn't free the backing memory, so we have to swap to an empty
    robin_hood::unordered_flat_map<int, double> new_data_partial;
    std::swap(new_data_partial, data_partial);

    data_full.resize(0);
  }

  bool is_full_mode() const {
    return mode == Mode::Full;
  }

  void update_max_dist() {
    assert(is_full_mode()); // the caller most likely has a more efficient way to compute this if partial mode
    max_dist = data_full.maxCoeff();
  }
};

};
