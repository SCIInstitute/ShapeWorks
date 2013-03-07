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

#include <ModelCache.h>
#include <Preferences.h>
#include <vtkPolyData.h>

long long ModelCache::getTotalPhysicalMemory()
{
#ifdef _WIN32
  MEMORYSTATUSEX memInfo;
  memInfo.dwLength = sizeof( MEMORYSTATUSEX );
  GlobalMemoryStatusEx( &memInfo );
  return memInfo.ullTotalPhys;
#else
#ifdef __APPLE__
  unsigned long long total_physical_memory;
  int name[ 2 ];
  name[ 0 ] = CTL_HW;
  name[ 1 ] = HW_MEMSIZE;
  size_t length = sizeof( unsigned long long );
  sysctl( name, 2, &total_physical_memory, &length, NULL, 0 );
  return total_physical_memory;
#else
  struct sysinfo info;

  sysinfo( &info );
  long long total_physical_memory = info.totalram;
  total_physical_memory *= info.mem_unit;

  return total_physical_memory;
#endif // ifdef __APPLE__
#endif // ifdef _WIN32
}

long long ModelCache::getTotalAddressibleMemory()
{
  if ( sizeof ( void* ) == 8 )
  {
    return 1ULL << 62;
  }
  else
  {
    return 1ULL << 31;
  }
}

long long ModelCache::getTotalAddressiblePhysicalMemory()
{
  long long addressable = ModelCache::getTotalAddressibleMemory();
  long long physical = ModelCache::getTotalPhysicalMemory();
  if ( physical > addressable ) {return addressable; } else {return physical; }
}

ModelCache::ModelCache()
{
  this->maxMemory = ModelCache::getTotalAddressiblePhysicalMemory();
  this->memorySize = 0;
}

vtkSmartPointer<vtkPolyData> ModelCache::getModel( const vnl_vector<double>& shape )
{
  if ( !Preferences::Instance().cacheEnabled() )
  {
    return NULL;
  }

  // search the cache for this shape
  CacheMap::iterator it = this->meshCache.find( shape );
  if ( it == this->meshCache.end() )
  {
    return NULL;
  }

  return meshCache[shape];
}

void ModelCache::insertModel( const vnl_vector<double>& shape, vtkSmartPointer<vtkPolyData> model )
{
  if ( !Preferences::Instance().cacheEnabled() )
  {
    return;
  }

  // compute the memory size of this shape
  size_t shapeSize = shape.size() * sizeof( double );
  size_t modelSize = model->GetActualMemorySize() * 1024; // given in kb
  size_t combinedSize = ( shapeSize * 2 ) + modelSize;

  this->freeSpaceForAmount( combinedSize );

  this->meshCache[shape] = model;
  this->memorySize += combinedSize;
  std::cerr << "Cache now holds " << this->meshCache.size() << " items\n";

  // add to LRU list
  CacheListItem item;
  item.key = shape;
  item.memorySize = combinedSize;
  this->cacheList.push_back( item );
}

void ModelCache::clear()
{
  this->meshCache.clear();
  this->memorySize = 0;
}

void ModelCache::freeSpaceForAmount( size_t allocation )
{
  size_t memoryLimit = ( Preferences::Instance().cacheMemory() / 100.0 ) * this->maxMemory;

  while ( !this->cacheList.empty() && this->memorySize + allocation > memoryLimit )
  {
    CacheListItem item = this->cacheList.back();
    this->memorySize -= item.memorySize;
    this->cacheList.pop_back();
    std::cerr << "erasing item for " << item.memorySize / 1024 << " kb savings\n";
    this->meshCache.erase( item.key );
  }
}
