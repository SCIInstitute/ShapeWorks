// Includes for platform specific functions
#ifdef _WIN32
#include <shlobj.h>
#include <tlhelp32.h>
#include <windows.h>
#include <LMCons.h>
#include <psapi.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#ifndef __APPLE__
#include <unistd.h>
#include <sys/sysinfo.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif // ifndef __APPLE__
#endif // ifdef _WIN32

#include <Data/MeshCache.h>

#include <vtkPolyData.h>

Preferences* MeshCache::pref_ref_ = nullptr;

//-----------------------------------------------------------------------------
long long MeshCache::getTotalPhysicalMemory()
{
#ifdef _WIN32
  MEMORYSTATUSEX memInfo;
  memInfo.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&memInfo);
  return memInfo.ullTotalPhys;
#else
#ifdef __APPLE__
  unsigned long long total_physical_memory;
  int name[ 2 ];
  name[ 0 ] = CTL_HW;
  name[ 1 ] = HW_MEMSIZE;
  size_t length = sizeof(unsigned long long);
  sysctl(name, 2, &total_physical_memory, &length, NULL, 0);
  return total_physical_memory;
#else
  struct sysinfo info;

  sysinfo(&info);
  long long total_physical_memory = info.totalram;
  total_physical_memory *= info.mem_unit;

  return total_physical_memory;
#endif // ifdef __APPLE__
#endif // ifdef _WIN32
}

//-----------------------------------------------------------------------------
long long MeshCache::getTotalAddressibleMemory()
{
  if (sizeof (void*) == 8) {
    return 1ULL << 62;
  }
  else {
    return 1ULL << 31;
  }
}

//-----------------------------------------------------------------------------
long long MeshCache::getTotalAddressiblePhysicalMemory()
{
  long long addressable = MeshCache::getTotalAddressibleMemory();
  long long physical = MeshCache::getTotalPhysicalMemory();
  if (physical > addressable) {return addressable; } else {return physical; }
}

//-----------------------------------------------------------------------------
MeshCache::MeshCache(Preferences& prefs) : preferences_(prefs)
{
  this->max_memory_ = MeshCache::getTotalAddressiblePhysicalMemory();
  this->memory_size_ = 0;
  this->pref_ref_ = &this->preferences_;
}

//-----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshCache::get_mesh(const MeshWorkItem &shape)
{
  QMutexLocker locker(&mutex_);

  if (!preferences_.get_cache_enabled()) {
    return nullptr;
  }

  // search the cache for this shape
  CacheMap::iterator it = this->mesh_cache_.find(shape);
  if (it == this->mesh_cache_.end()) {
    return nullptr;
  }

  return this->mesh_cache_[shape];
}

//-----------------------------------------------------------------------------
void MeshCache::insert_mesh(const MeshWorkItem& item, vtkSmartPointer<vtkPolyData> mesh)
{
  if (!preferences_.get_cache_enabled()) {
    return;
  }

  QMutexLocker locker(&mutex_);

  if (mesh) {
    // compute the memory size of this shape
    size_t shape_size = item.points.size() * sizeof(double);
    size_t mesh_size = mesh->GetActualMemorySize() * 1024; // given in kb
    size_t combined_size = (shape_size * 2) + mesh_size;
    this->memory_size_ += combined_size;
  }

  this->freeSpaceForAmount(item.memory_size);

  this->mesh_cache_[item] = mesh;

  //std::cerr << "Cache now holds " << this->mesh_cache_.size() << " items\n";

  // add to LRC list
  this->cache_list_.push_back(item);
}

//-----------------------------------------------------------------------------
void MeshCache::clear()
{
  QMutexLocker locker(&mutex_);

  this->mesh_cache_.clear();
  this->memory_size_ = 0;
}

//-----------------------------------------------------------------------------
void MeshCache::freeSpaceForAmount(size_t allocation)
{
  size_t memoryLimit = (preferences_.get_memory_cache_percent() / 100.0) * this->max_memory_;

  while (!this->cache_list_.empty() && this->memory_size_ + allocation > memoryLimit) {
    auto item = this->cache_list_.back();
    this->memory_size_ -= item.memory_size;
    this->cache_list_.pop_back();
    std::cerr << "erasing item for " << item.memory_size / 1024 << " kb savings\n";
    this->mesh_cache_.erase(item);
  }
}
