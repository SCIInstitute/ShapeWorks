#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

//! Representation of a single mesh.
/*!
 * The Mesh class represents a single mesh generated from an image file.
 * It is responsible for loading the image and generating a mesh from it.
 *
 */
class Mesh
{

public:

  /// Load an image into this mesh
  Mesh( std::string filename );
  ~Mesh();

  /// Get the dimensions as a string for display
  std::string get_dimension_string();

  /// Get the original filename that this mesh was derived from
  std::string get_filename();

  /// Get the mesh polydata
  vtkSmartPointer<vtkPolyData> get_poly_data();

private:

  std::string filename_;
  vtkSmartPointer<vtkPolyData> poly_data_;
  int dimensions_[3];
};
