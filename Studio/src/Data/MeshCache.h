#pragma once

#include <Data/MeshWorkQueue.h>
#include <Data/StudioMesh.h>

// qt
#include <QMutex>

// std
#include <list>
#include <map>

namespace shapeworks {

// mesh cache type
using CacheMap = std::map<MeshWorkItem, MeshHandle>;

// LRU list
using CacheList = std::list<MeshWorkItem>;

/**
 * @brief Thread safe cache for meshes index by shape
 *
 * The MeshCache implements a std::map keyed by shape (list of points) with MeshHandle values.
 * It is thread-safe and can be used from any thread.
 */
class MeshCache {
 public:
  MeshCache();

  void set_cache_enabled(bool enabled) { cache_enabled_ = enabled; }

  void set_memory_percent(int percent) { cache_memory_percent_ = percent; }

  MeshHandle get_mesh(const MeshWorkItem& vector);

  void insert_mesh(const MeshWorkItem& item, MeshHandle mesh);

  void clear();

 private:
  void freeSpaceForAmount(size_t allocation);

  static long long get_total_physical_memory();
  static long long get_total_addressable_memory();
  static long long get_total_addressable_physical_memory();

  // mesh cache
  CacheMap mesh_cache_;

  // lrc list
  CacheList cache_list_;

  // size of memory in use by the cache
  size_t current_memory_size_ = 0;

  // maximum memory
  long long max_memory_ = 0;

  // for concurrent access
  QMutex mutex_;

  bool cache_enabled_ = true;
  int cache_memory_percent_ = 0;
};
}  // namespace shapeworks
