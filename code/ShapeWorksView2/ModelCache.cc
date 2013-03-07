#include <ModelCache.h>

vtkSmartPointer<vtkPolyData> ModelCache::getModel( const vnl_vector<double>& shape )
{
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
  this->meshCache[shape] = model;
  std::cerr << "Cache now holds " << this->meshCache.size() << " items\n";
}

void ModelCache::clear()
{
  this->meshCache.clear();
}
