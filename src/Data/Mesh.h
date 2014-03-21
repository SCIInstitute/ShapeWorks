#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class Mesh
{

public:

  void load_file( std::string filename );

  std::string get_dimension_string();

  std::string filename_;
  vtkSmartPointer<vtkPolyData> poly_data_;
  int dimensions_[3];
};
