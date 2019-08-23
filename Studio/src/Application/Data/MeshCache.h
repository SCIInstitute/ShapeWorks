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

#ifndef MESH_CACHE_H
#define MESH_CACHE_H

#include <list>
#include <map>

#include <QMutex>

#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>

#include "Data/Preferences.h"

//#include "itkParticleShapeStatistics.h"

class vtkPolyData;

class CacheListItem
{
public:
  vnl_vector<double> key;
  size_t memorySize;
};

// comparison class for vnl_vectors (for cache)
class vnl_vector_compare
{
public:
  bool operator()( const vnl_vector<double> &a, const vnl_vector<double> &b) const;
};

// mesh cache type
typedef std::map< const vnl_vector<double>, vtkSmartPointer<vtkPolyData>, vnl_vector_compare > CacheMap;

// LRC list
typedef std::list< CacheListItem > CacheList;

class MeshCache
{

public:

  MeshCache(Preferences& prefs);

  vtkSmartPointer<vtkPolyData> getMesh( const vnl_vector<double>& vector );

  void insertMesh( const vnl_vector<double>& shape, vtkSmartPointer<vtkPolyData> mesh );

  void clear();

  static Preferences * pref_ref_;

private:

  void freeSpaceForAmount( size_t allocation );

  static long long getTotalPhysicalMemory();
  static long long getTotalAddressibleMemory();
  static long long getTotalAddressiblePhysicalMemory();

  Preferences &preferences_;
  // mesh cache
  CacheMap meshCache;

  // lrc list
  CacheList cacheList;

  // size of memory in use by the cache
  size_t memorySize;

  // maximum memory
  long long maxMemory;

  // for concurrent access
  QMutex mutex;
};

#endif // ifndef MESH_CACHE_H
