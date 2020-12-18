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

  /// Logical region of a mesh
  struct Region
  {
    Coord min = Coord({ 1000000000, 1000000000, 1000000000 });
    Coord max = Coord({ -1000000000, -1000000000, -1000000000 });
    Region(const Dims &dims) : min({0, 0, 0}) {
      if (0 != (dims[0] + dims[1] + dims[2]))
        max = Coord({static_cast<long>(dims[0])-1,
                     static_cast<long>(dims[1])-1,
                     static_cast<long>(dims[2])-1});
    }
    Region(const Coord &_min, const Coord &_max) : min(_min), max(_max) {}
    Region() = default;
    bool operator==(const Region &other) const { return min == other.min && max == other.max; }

    /// verified min/max do not create an inverted or an empty region
    bool valid() const { return max[0] > min[0] && max[1] > min[1] && max[2] > min[2]; }

    Coord origin() const { return min; }
    Dims size() const {
      return Dims({static_cast<unsigned long>(max[0]-min[0]),
                   static_cast<unsigned long>(max[1]-min[1]),
                   static_cast<unsigned long>(max[2]-min[2])});
    }

    /// grows or shrinks the region by the specified amount
    void pad(int padding) {
      for (auto i=0; i<3; i++) {
        min[i] -= padding;
        max[i] += padding;
      }
    }

    /// shrink this region down to the smallest portions of both
    void shrink(const Region &other) {
      for (auto i=0; i<3; i++) {
        min[i] = std::max(min[i], other.min[i]);
        max[i] = std::min(max[i], other.max[i]);
      }
    }

    /// grow this region up to the largest portions of both
    void grow(const Region &other) {
      for (auto i=0; i<3; i++) {
        min[i] = std::min(min[i], other.min[i]);
        max[i] = std::max(max[i], other.max[i]);
      }
    }
  };

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(MeshType meshPtr) : mesh(meshPtr) { if (!mesh) throw std::invalid_argument("null meshPtr"); }
  Mesh& operator=(const Mesh& mesh); /// lvalue assignment operator
  Mesh& operator=(std::unique_ptr<Mesh>);      /// rvalue assignment operator

  // return the current mesh
  MeshType getMesh() const { return this->mesh; }

  /// writes mesh, format specified by filename extension
  Mesh& write(const std::string &pathname);

  /// creates mesh of coverage between two meshes
  Mesh& coverage(const Mesh& other_mesh);

  /// applies laplacian smoothing
  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  /// applies filter to reduce number of triangles in mesh
  Mesh& decimate(double reduction = 0.0, double angle = 0.0, bool preservetopology = false);

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
  Mesh::Region boundingBox(bool center=false) const;

  /// compute surface to surface distance using a filter
  vtkSmartPointer<swHausdorffDistancePointSetFilter> computeDistance(const Mesh &other_mesh, bool target=false);

  /// returns surface to surface distance or hausdorff distance
  double hausdorffDistance(const Mesh &other_mesh, bool target=false);

  /// returns relative distance from mesh A to mesh B
  double relativeDistanceAtoB(const Mesh &other_mesh, bool target=false);

  /// returns relative distance from mesh B to mesh A
  double relativeDistanceBtoA(const Mesh &other_mesh, bool target=false);

  /// compute origin of volume that would contain the rasterization of each mesh
  Point3 rasterizationOrigin(Mesh::Region region, Vector3 spacing=makeVector({1.0,1.0,1.0}), int padding=0);

  /// compute size of volume that would contain the rasterization of each mesh
  Dims rasterizationSize(Mesh::Region region, Vector3 spacing=makeVector({1.0,1.0,1.0}), int padding=0);

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

/// stream insertion operators for Mesh and Mesh::Region
std::ostream& operator<<(std::ostream &os, const Mesh& mesh);
std::ostream& operator<<(std::ostream &os, const Mesh::Region &region);

} // shapeworks
