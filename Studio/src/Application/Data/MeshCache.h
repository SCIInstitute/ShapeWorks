#pragma once

/*
 * Shapeworks license
 */

/**
 * @file MeshCache.h
 * @brief Thread safe cache for meshes index by shape
 *
 * The MeshCache implements a std::map keyed by shape (list of points) with vtkPolyData values.
 * It is thread-safe and can be used from any thread.
 */

#include <list>
#include <map>

#include <QMutex>

#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>

#include <Data/MeshWorkQueue.h>

#include <Data/Preferences.h>

class vtkPolyData;

// mesh cache type
using CacheMap = std::map<MeshWorkItem, vtkSmartPointer<vtkPolyData>>;

// LRC list
using CacheList = std::list<MeshWorkItem>;

class MeshCache
{

public:

  MeshCache(Preferences& prefs);

  vtkSmartPointer<vtkPolyData> get_mesh(const MeshWorkItem& vector);

  void insert_mesh(const MeshWorkItem& item, vtkSmartPointer<vtkPolyData> mesh);

  void clear();

  static Preferences* pref_ref_;

private:

  void freeSpaceForAmount(size_t allocation);

  static long long getTotalPhysicalMemory();
  static long long getTotalAddressibleMemory();
  static long long getTotalAddressiblePhysicalMemory();

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
