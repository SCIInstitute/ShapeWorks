#pragma once

#include <robin_hood.h>

template<class Key, class Value>
struct FixedSizeCache {
  /*
   * The idea behind this cache is to avoid creating/destroying "Value" objects since they are extremely expensive. Instead,
   * we maintain a vector<T> of entries that is allocated(see VtkMeshWrapper::PrecomputeGeodesics for example) at startup.
   * Later, we store for each key the index into this vector, and any cache reset operation simply touches the map and not
   * the backing storage.
   *
   * This class is pretty bare-bones because a lot of operations like "resizing entries to correct size" are application-specific
   */

  robin_hood::unordered_map<Key, int> key2entry;
  std::vector<Value> entries;

  // clear everything in the cache except what is required by the active_keys. What is "active" is determined by the
  // caller. This method **may be extremely inefficient if the Value is expensive to copy + doesn't have move c'tors**
  void clear_except_active(const std::vector<Key>& active_keys) {

    // figure out which (unique) active_keys we need to keep
    robin_hood::unordered_map<Key, int> to_keep;
    robin_hood::unordered_set<int> taken; // keep track of the corresponding entry locations
    for(const Key& key : active_keys) {
      const auto entry = key2entry.find(key);
      if(entry != key2entry.end()) {
        to_keep[key] = entry->second;
        taken.insert(entry->second);
      }
    }

    // unsure if there's anything more meaningful we can do if the cache size is too small relative to the number of
    // `taken` items.
    if(to_keep.size() >= key2entry.size()) {
      key2entry.clear();
      return;
    }

    // clear old cache entries
    key2entry.clear();

    int next_idx = 0;
    for(auto& it : to_keep) {
      // we don't need to std::swap for this entry
      if(it.second < to_keep.size()) {
        key2entry[it.first] = it.second;
        continue;
      }

      // is this spot already taken by a key we wish to keep?
      while(taken.find(next_idx) != taken.end()) {
        next_idx++;
      }

      // Move the cache entry to earlier and update key2entry
      std::swap(entries[next_idx], entries[it.second]);
      key2entry[it.first] = next_idx;
      next_idx++;
    }
  }
};

