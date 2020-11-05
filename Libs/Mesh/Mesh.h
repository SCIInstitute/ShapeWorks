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

  Mesh(vtkSmartPointer<vtkPolyData>&& rhs) : mesh(std::move(rhs)) {}
  Mesh(const std::string& pathname) : mesh(read(pathname)) {}

  bool write(const std::string &pathname);

  /// returns center of mass of mesh
  Point3 centerOfMass() const;

  /// translate mesh by a given vector
  Mesh& translate(const Vector3 &v);

  Mesh& coverage(const Mesh& other_mesh); // TODO: everything should be like this and return reference to self
  bool smooth(unsigned iterations = 1);
  bool decimate(float reduction = 0.01, float angle = 30, bool preserve_topology = false);

  bool compare_points_equal(const Mesh& other_mesh);
  bool compare_scalars_equal(const Mesh& other_mesh);

  static std::vector<std::string> get_supported_types() { return {"vtk", "vtp", "ply", "stl", "obj"}; }

  MeshType get_poly_data();

private:
  Mesh() {}
  MeshType read(const std::string& pathname);

  MeshType mesh;
};

} // shapeworks
