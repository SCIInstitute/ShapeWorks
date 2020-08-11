#pragma once

#include "Shapeworks.h"

#include <vtkPolyData.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>; // TODO: we need to support multiple mesh types, such as vtkPolyData and vtkPLYData; probably use vtk mesh base class (if one exists)

  Mesh(vtkSmartPointer<vtkPolyData>&& rhs) : mesh(std::move(rhs)) {}
  Mesh(const std::string& pathname) : mesh(read(pathname)) {}

  // return the current mesh
  MeshType getMesh() const { return this->mesh; }

  /// writes mesh, format specified by filename extension
  Mesh& write(const std::string &pathname);

  /// creates mesh of coverage between two meshes
  Mesh& coverage(const Mesh& other_mesh); // TODO: everything should be like this and return reference to self

  Mesh& march(double levelset = 0.0);
  Mesh& smooth(int iterations = 0, double relaxation = 0.0);
  Mesh& decimate(double reduction = 0.0, double angle = 0.0, bool preservetopology = false);
  Mesh& reflect(const Axis &axis, const Vector3 &origin = makeVector({ 0.0, 0.0, 0.0 }));
  Mesh& applyTransform(const vtkTransform transform);

  /// compare if points in two meshes are equal
  bool compare_points_equal(const Mesh& other_mesh);

  /// compare if scalars in two meshes are equal
  bool compare_scalars_equal(const Mesh& other_mesh);

private:
  Mesh() {}

  /// reads mesh (used only by constructor)
  MeshType read(const std::string& pathname);

  MeshType mesh;
};

} // shapeworks
