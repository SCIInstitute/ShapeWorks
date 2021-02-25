#pragma once

#include <unordered_map>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <Eigen/Dense>

typedef std::pair<int, int> pair_ii;

struct GeoEntry {
  double distances[3][3];
  bool have_grad{false};
  Eigen::Vector3d gradients[3];
};

//todo rename
struct FixedSizeCache {
  std::unordered_map<pair_ii, GeoEntry, boost::hash<pair_ii>> key2entry; //todo robinhood hash!
  size_t next_idx{0};

  void clear() {
    next_idx = 0;
    key2entry.clear();
  }
};

