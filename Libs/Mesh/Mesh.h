#pragma once

#include "Shapeworks.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>;

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(vtkSmartPointer<vtkPolyData>&& rhs) : mesh(std::move(rhs)) {}

  /// Write mesh to path, filename based on extension
  bool write(const std::string &pathname);

  /// returns center of mass of mesh
  Point3 centerOfMass() const;

  /// translate mesh by a given vector
  Mesh& translate_mesh(const Vector3 &v);

  /// determine the coverage between this mesh and another one (e.g. acetabular cup / femoral head)
  Mesh& coverage(const Mesh& other_mesh, bool allow_back_intersections = false,
                 double angle_threshold = 0, double back_search_radius = 0);

  /// Not yet implemented
  Mesh& smooth(unsigned iterations = 1);
  /// Not yet implemented
  Mesh& decimate(float reduction = 0.01, float angle = 30, bool preserve_topology = false);

  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

  // query functions //

  /// number of vertices
  vtkIdType numVertices() const { return mesh->GetNumberOfVerts(); }

  /// number of faces
  vtkIdType numFaces() const { return mesh->GetNumberOfCells(); }

  /// get bounds
  double* bounds() const {
    double bb[6];
    mesh->GetBounds(bb);
    return bb;
  }

  static std::vector<std::string> get_supported_types()
  {
    return {"vtk", "vtp", "ply", "stl", "obj"};
  }

  MeshType get_poly_data();

private:
  Mesh() {}
  MeshType read(const std::string& pathname);

  MeshType mesh;
};

/// stream insertion operators for Mesh
std::ostream& operator<<(std::ostream &os, const Mesh& mesh);

} // shapeworks
