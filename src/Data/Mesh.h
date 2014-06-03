#ifndef STUDIO_DATA_MESH_H
#define STUDIO_DATA_MESH_H

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vnl/vnl_vector.h>


class Mesh;
typedef QSharedPointer< Mesh > MeshHandle;

//! Representation of a single mesh.
/*!
 * The Mesh class represents a single mesh generated from an image file.
 * It is responsible for loading the image and generating a mesh from it.
 *
 */
class Mesh
{

public:

  /// Constructor
  Mesh();

  /// Destructor
  ~Mesh();

  /// Create a mesh from an image
  void create_from_image( QString filename );

  /// Create a mesh from a point set
  bool create_from_pointset( const vnl_vector<double>& vnl_points );

  /// Get the dimensions as a string for display (if loaded from an image)
  QString get_dimension_string();

  /// Get the mesh polydata
  vtkSmartPointer<vtkPolyData> get_poly_data();

private:

  vtkSmartPointer<vtkPolyData> poly_data_;
  int dimensions_[3];
};

#endif /* STUDIO_DATA_MESH_H */
