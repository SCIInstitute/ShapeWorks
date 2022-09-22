#pragma once

#include <StudioMesh.h>

namespace shapeworks {

//! Representation of a group of meshes
/*!
 * The MeshGroup class encapsulates a group of meshes (e.g. from a single subject)
 *
 */
class MeshGroup {
 public:
  MeshGroup();

  MeshGroup(unsigned long num_meshes);

  ~MeshGroup();

  void set_number_of_meshes(int n);

  bool valid();

  void set_mesh(int i, MeshHandle mesh);

  //! Return a reference to the list of meshes
  MeshList& meshes();

  //! Return polydata of all meshes, combined
  vtkSmartPointer<vtkPolyData> get_combined_poly_data();

 private:
  MeshList meshes_;
};
}  // namespace shapeworks
