#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"

#include <vtkPolyData.h>
#include <vtkPlane.h>
#include <swHausdorffDistancePointSetFilter.h>
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
  Mesh& coverage(const Mesh& other_mesh);

  Mesh& march(double levelset = 0.0);

  /// applies laplacian smoothing
  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  Mesh& decimate(double reduction = 0.0, double angle = 0.0, bool preservetopology = false);

  /// reflect meshes with respect to a specified center and specific axis
  Mesh& reflect(const Axis &axis, const Vector3 &origin = makeVector({ 0.0, 0.0, 0.0 }));

  /// applies the given transformation to the mesh
  Mesh& applyTransform(const vtkTransform transform);

  /// finds holes in a mesh and closes them
  Mesh& fillHoles();

  Mesh& probeVolume(const Image &img);

  /// clips a mesh using a cutting plane
  Mesh& clip(const vtkSmartPointer<vtkPlane> plane);

  /// helper to translate mesh
  Mesh& translate(const Vector3 &v);

  /// helper to scale mesh
  Mesh& scale(const Vector3 &v);

  vtkSmartPointer<swHausdorffDistancePointSetFilter> computeDistance(const Mesh &other_mesh, bool target=false);

  Vector getHausdorffDistance(const Mesh &other_mesh, bool target=false);

  Vector getRelativeDistanceAtoB(const Mesh &other_mesh, bool target=false);

  Vector getRelativeDistanceBtoA(const Mesh &other_mesh, bool target=false);

  /// compare if points in two meshes are equal
  bool compare_points_equal(const Mesh& other_mesh);

  /// compare if scalars in two meshes are equal
  bool compare_scalars_equal(const Mesh& other_mesh);

  // query functions //

  /// center of mesh
  double* center() const { return mesh->GetCenter(); }

  /// bounds of mesh
  double* bounds() const { return mesh->GetBounds(); }

private:
  Mesh() {}

  /// reads mesh (used only by constructor)
  MeshType read(const std::string& pathname);

  MeshType mesh;
};

} // shapeworks
