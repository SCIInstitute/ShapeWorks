#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

class Mesh
{

public:

  Mesh();
  ~Mesh();

  void load_file( std::string filename );

  std::string get_dimension_string();

  std::string get_filename();

  vtkSmartPointer<vtkPolyData> get_poly_data();

private:

  std::string filename_;
  vtkSmartPointer<vtkPolyData> poly_data_;
  int dimensions_[3];
};
