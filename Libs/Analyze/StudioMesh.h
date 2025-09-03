#pragma once

#include <Libs/Mesh/Mesh.h>
#include <itkImage.h>
#include <vnl/vnl_vector.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

using PixelType = float;
using ImageType = itk::Image<PixelType, 3>;
class vtkFloatArray;
class vtkStaticPointLocator;

namespace shapeworks {

class StudioMesh;
using MeshHandle = std::shared_ptr<StudioMesh>;
using MeshList = std::vector<MeshHandle>;

//! Representation of a single mesh.
/*!
 * The Mesh class represents a single mesh generated from an image file or set of particles.
 * It is responsible for loading the image and generating a mesh from it.
 *
 * TODO: Merge this functionality with Libs/Mesh/Mesh class
 */
class StudioMesh {
 public:
  //! Constructor
  StudioMesh();

  //! Destructor
  ~StudioMesh();

  //! Get the mesh polydata
  vtkSmartPointer<vtkPolyData> get_poly_data();

  //! Set the poly data directly
  void set_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

  //! Set the error message
  void set_error_message(std::string error_message);

  //! Return the error message
  std::string get_error_message();

  //! Apply a feature map
  void apply_feature_map(std::string name, ImageType::Pointer image);

  //! Apply scalars from another mesh, with a transform
  void apply_scalars(MeshHandle mesh);

  //! Interpolation scalars at positions to this mesh
  void interpolate_scalars_to_mesh(std::string name, Eigen::VectorXd positions, Eigen::VectorXd scalar_values);

  //! Return the range of largest axis (e.g. 200 for an object that sits in 100x200x100)
  double get_largest_dimension_size();

  //! Get or create and return an array with a given name
  vtkFloatArray* get_or_create_array(std::string name, float default_value);

  //! Paint free form constraint
  void paint_ffc(double world_pos[], double radius, bool inclusive);

  //! Does this mesh have free form constraint paint?
  bool has_ffc_paint();

  //! Return a shapeworks Mesh library class mesh
  Mesh get_sw_mesh();

  static constexpr const char* const FFC_PAINT = "ffc_paint";

 private:
  vtkSmartPointer<vtkPolyData> poly_data_;

  vtkSmartPointer<vtkStaticPointLocator> locator_;

  // error message if the polydata didn't load
  std::string error_message_;
};
}  // namespace shapeworks
