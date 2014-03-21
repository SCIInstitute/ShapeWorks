#include <Data/Mesh.h>

class Shape
{

public:
  Shape();
  ~Shape();

  void import_initial_file( std::string filename );
  Mesh get_initial_mesh();

private:

  Mesh initial_mesh_;
  Mesh groomed_mesh_;
  Mesh reconstructed_mesh_;
};
