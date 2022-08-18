// Includes for platform specific functions
#ifdef _WIN32
#include <LMCons.h>
#include <psapi.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <windows.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#ifndef __APPLE__
#include <sys/sysinfo.h>
#include <unistd.h>
#else
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif  // ifndef __APPLE__
#endif  // ifdef _WIN32

#include <MeshCache.h>
#include <Logging.h>
#include <vtkPolyData.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
long long MeshCache::get_total_physical_memory() {
#ifdef _WIN32
  MEMORYSTATUSEX memInfo;
  memInfo.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&memInfo);
  return memInfo.ullTotalPhys;
#else
#ifdef __APPLE__
  unsigned long long total_physical_memory;
  int name[2];
  name[0] = CTL_HW;
  name[1] = HW_MEMSIZE;
  size_t length = sizeof(unsigned long long);
  sysctl(name, 2, &total_physical_memory, &length, NULL, 0);
  return total_physical_memory;
#else
  struct sysinfo info;

  sysinfo(&info);
  long long total_physical_memory = info.totalram;
  total_physical_memory *= info.mem_unit;

  return total_physical_memory;
#endif  // ifdef __APPLE__
#endif  // ifdef _WIN32
}

//-----------------------------------------------------------------------------
long long MeshCache::get_total_addressable_memory() {
  if (sizeof(void*) == 8) {
    return 1ULL << 62;
  } else {
    return 1ULL << 31;
  }
}

//-----------------------------------------------------------------------------
long long MeshCache::get_total_addressable_physical_memory() {
  long long addressable = MeshCache::get_total_addressable_memory();
  long long physical = MeshCache::get_total_physical_memory();
  if (physical > addressable) {
    return addressable;
  } else {
    return physical;
  }
}

//-----------------------------------------------------------------------------
MeshCache::MeshCache() {
  max_memory_ = MeshCache::get_total_addressable_physical_memory();
  current_memory_size_ = 0;
}

//-----------------------------------------------------------------------------
MeshHandle MeshCache::get_mesh(const MeshWorkItem& shape) {
  QMutexLocker locker(&mutex_);

  if (!cache_enabled_) {
    return nullptr;
  }

  // search the cache for this shape
  CacheMap::iterator it = mesh_cache_.find(shape);
  if (it == mesh_cache_.end()) {
    return nullptr;
  }

  return mesh_cache_[shape];
}

//-----------------------------------------------------------------------------
void MeshCache::insert_mesh(const MeshWorkItem& item, MeshHandle mesh) {
  if (!cache_enabled_) {
    return;
  }

  QMutexLocker locker(&mutex_);

  if (mesh->get_poly_data()) {
    // compute the memory size of this shape
    size_t shape_size = item.points.size() * sizeof(double);
    size_t mesh_size = mesh->get_poly_data()->GetActualMemorySize() * 1024;  // given in kb
    size_t combined_size = (shape_size * 2) + mesh_size;
    current_memory_size_ += combined_size;
  }

  freeSpaceForAmount(item.memory_size);

  mesh_cache_[item] = mesh;

  // std::cerr << "Cache now holds " << mesh_cache_.size() << " items\n";

  // add to LRC list
  cache_list_.push_back(item);
}

//-----------------------------------------------------------------------------
void MeshCache::clear() {
  QMutexLocker locker(&mutex_);

  mesh_cache_.clear();
  current_memory_size_ = 0;
}

//-----------------------------------------------------------------------------
void MeshCache::freeSpaceForAmount(size_t allocation) {
  size_t memory_limit = (cache_memory_percent_ / 100.0) * max_memory_;

  while (!cache_list_.empty() && current_memory_size_ + allocation > memory_limit) {
    auto item = cache_list_.back();
    current_memory_size_ -= item.memory_size;
    cache_list_.pop_back();
    SW_LOG("erasing item for {}kb savings", item.memory_size / 1024);
    mesh_cache_.erase(item);
  }
}
}  // namespace shapeworks
