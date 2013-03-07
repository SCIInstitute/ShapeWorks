#include <vtkSmartPointer.h>

class vtkPolyData;

// comparison class for vnl_vectors (for cache)
class vnl_vector_compare
{
public:
  bool operator()( const vnl_vector<double> &x, const vnl_vector<double> &y ) const
  {
    if ( x.size() < y.size() )
    {
      return true;
    }

    for ( unsigned i = 0; i < x.size(); i++ )
    {
      if ( x[i] < y[i] )
      {
        return true;
      }
      else if ( y[i] < x[i] )
      {
        return false;
      }
    }

    return false;
  }
};

// mesh cache type
typedef std::map< const vnl_vector<double>, vtkSmartPointer<vtkPolyData>, vnl_vector_compare > CacheMap;

class ModelCache
{

public:
  vtkSmartPointer<vtkPolyData> getModel( const vnl_vector<double>& vector );

  void insertModel( const vnl_vector<double>& shape, vtkSmartPointer<vtkPolyData> model );

  void clear();

private:

  // mesh cache
  CacheMap meshCache;
};
