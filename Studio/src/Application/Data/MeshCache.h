#pragma once

/*
 * Shapeworks license
 */

/**
 * @file MeshCache.h
 * @brief Thread safe cache for meshes index by shape
 *
 * The MeshCache implements a std::map keyed by shape (list of points) with MeshHandle values.
 * It is thread-safe and can be used from any thread.
 */

#include <list>
#include <map>

#include <QMutex>

#include <vnl/vnl_vector.h>

#include <Data/Mesh.h>
#include <Data/MeshWorkQueue.h>
#include <Data/Preferences.h>

// mesh cache type
using CacheMap = std::map<MeshWorkItem, MeshHandle>;

// LRC list
using CacheList = std::list<MeshWorkItem>;

class MeshCache
{

public:

  MeshCache(Preferences& prefs);

  MeshHandle get_mesh(const MeshWorkItem& vector);

  void insert_mesh(const MeshWorkItem& item, MeshHandle mesh);

  void clear();

  static Preferences* pref_ref_;

private:

  void freeSpaceForAmount(size_t allocation);

  static long long get_total_physical_memory();
  static long long get_total_addressible_memory();
  static long long get_total_addressible_physical_memory();

  Preferences &preferences_;

  // mesh cache
  CacheMap mesh_cache_;

  // lrc list
  CacheList cache_list_;

  // size of memory in use by the cache
  size_t memory_size_;

  // maximum memory
  long long max_memory_;

  // for concurrent access
  QMutex mutex_;
};
