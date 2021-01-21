#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPlane.h>
#include <swHausdorffDistancePointSetFilter.h>
#include <string>

namespace shapeworks {

class Mesh
{
public:
  using MeshType = vtkSmartPointer<vtkPolyData>;

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(MeshType meshPtr) : mesh(meshPtr) { if (!mesh) throw std::invalid_argument("null meshPtr"); }
  Mesh& operator=(const Mesh& mesh); /// lvalue assignment operator
  Mesh& operator=(std::unique_ptr<Mesh> mesh);      /// rvalue assignment operator

  // return the current mesh
  MeshType getMesh() const { return this->mesh; }

  /// writes mesh, format specified by filename extension
  Mesh& write(const std::string &pathname);

  /// determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)
  Mesh& coverage(const Mesh& other_mesh, bool allow_back_intersections = false,
                 double angle_threshold = 0, double back_search_radius = 0);

  /// applies laplacian smoothing
  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  /// applies filter to reduce number of triangles in mesh
  Mesh& decimate(double reduction = 0.0, double angle = 0.0, bool preservetopology = false);

  /// handle flipping normals
  Mesh& invertNormal();

  /// reflect meshes with respect to a specified center and specific axis
  Mesh& reflect(const Axis &axis, const Vector3 &origin = makeVector({ 0.0, 0.0, 0.0 }));

  /// applies the given transformation to the mesh
  Mesh& applyTransform(const vtkTransform transform);

  /// finds holes in a mesh and closes them
  Mesh& fillHoles();

  /// samples data values at specified point locations
  Mesh& probeVolume(const Image &img);

  /// clips a mesh using a cutting plane
  Mesh& clip(const vtkSmartPointer<vtkPlane> plane);

  /// helper to translate mesh
  Mesh& translate(const Vector3 &v);

  /// helper to scale mesh
  Mesh& scale(const Vector3 &v);

  /// computes bounding box of current mesh
  Region boundingBox(bool center=false) const;

  /// compute surface to surface distance using a filter
  vtkSmartPointer<swHausdorffDistancePointSetFilter> computeDistance(const Mesh &other_mesh, bool target=false);

  /// returns surface to surface distance or hausdorff distance
  double hausdorffDistance(const Mesh &other_mesh, bool target=false);

  /// returns relative distance from mesh A to mesh B
  double relativeDistanceAtoB(const Mesh &other_mesh, bool target=false);

  /// returns relative distance from mesh B to mesh A
  double relativeDistanceBtoA(const Mesh &other_mesh, bool target=false);

  /// compute origin of volume that would contain the rasterization of each mesh
  Point3 rasterizationOrigin(Region region, Vector3 spacing=makeVector({1.0,1.0,1.0}), int padding=0);

  /// compute size of volume that would contain the rasterization of each mesh
  Dims rasterizationSize(Region region, Vector3 spacing=makeVector({1.0,1.0,1.0}), int padding=0);

  /// rasterizes mesh to create binary images rasterizationOrigin and rasterizationSize
  static Image rasterize(const Mesh &mesh, Vector3 spacing, Dims size, Point3 origin);

  /// converts mesh to image
  Image toImage(Vector3 spacing, Dims size, Point3 origin) const;

  /// converts mesh to distance transform
  Image toDistanceTransform(Vector3 spacing, Dims size, Point3 origin) const;

  /// quality control mesh
  Mesh& fix(bool wind = true, bool smoothBefore = true, bool smoothAfter = true, double lambda = 0.5, int iterations = 1, bool decimate = true, double percentage = 0.5);

  // query functions //

  /// center of mesh
  Point3 center() const;

  /// center of mass of mesh
  Point3 centerOfMass() const;

  /// number of vertices
  vtkIdType numVertices() const { return mesh->GetNumberOfVerts(); }

  /// number of faces
  vtkIdType numFaces() const { return mesh->GetNumberOfCells(); }

  /// compare if points in two meshes are equal
  bool compare_points_equal(const Mesh& other_mesh) const;

  /// compare if scalars in two meshes are equal
  bool compare_scalars_equal(const Mesh& other_mesh) const;

  /// compares this with another mesh by comparing points
  bool operator==(const Mesh& other) const { return compare_points_equal(other); }

  static std::vector<std::string> get_supported_types() { return {"vtk", "vtp", "ply", "stl", "obj"}; }

private:
  friend struct SharedCommandData;
  Mesh() : mesh(nullptr) {} // only for use by SharedCommandData since a Mesh should always be valid, never "empty"

  /// reads mesh (used only by constructor)
  static MeshType read(const std::string& pathname);

  MeshType mesh;
};

/// stream insertion operators for Mesh
std::ostream& operator<<(std::ostream &os, const Mesh& mesh);

} // shapeworks
