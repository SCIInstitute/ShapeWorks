#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"

#include <math.h>
#include <algorithm>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <string>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDataSetAttributes.h>
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkKdTreePointLocator.h>
#include <vtkCellLocator.h>

#include <igl/grad.h>
#include <igl/per_vertex_normals.h>
#include <geometrycentral/surface/surface_mesh_factories.h>
#include <geometrycentral/surface/surface_mesh.h>
#include "geometrycentral/surface/heat_method_distance.h"
#include "geometrycentral/surface/meshio.h"

#include <vtkLookupTable.h>
#include <vtkArrowSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkDijkstraGraphGeodesicPath.h>

namespace shapeworks {

class Mesh
{
public:
  enum AlignmentType { Rigid, Similarity, Affine };
  enum DistanceMethod { POINT_TO_POINT, POINT_TO_CELL };

  using MeshType = vtkSmartPointer<vtkPolyData>;

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(MeshType meshPtr) : mesh(meshPtr) { if (!mesh) throw std::invalid_argument("null meshPtr"); }
  Mesh(const Mesh& orig) : mesh(MeshType::New()) { mesh->DeepCopy(orig.mesh); }
  Mesh(Mesh&& orig) : mesh(orig.mesh) { orig.mesh = nullptr; }
  Mesh& operator=(const Mesh& orig) { mesh = MeshType::New(); mesh->DeepCopy(orig.mesh); return *this; }
  Mesh& operator=(Mesh&& orig) { mesh = orig.mesh; orig.mesh = nullptr; return *this; }

  /// return the current mesh
  MeshType getVTKMesh() const { return this->mesh; }

  /// writes mesh, format specified by filename extension
  Mesh& write(const std::string &pathname);

  /// determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)
  Mesh& coverage(const Mesh& otherMesh, bool allowBackIntersections = true,
                 double angleThreshold = 0, double backSearchRadius = 0);

  /// applies laplacian smoothing
  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  /// applies filter to reduce number of triangles in mesh
  Mesh& decimate(double reduction = 0.0, double angle = 0.0, bool preserveTopology = true);

  /// handle flipping normals
  Mesh& invertNormals();

  /// reflect meshes with respect to a specified center and specific axis
  Mesh& reflect(const Axis &axis, const Vector3 &origin = makeVector({ 0.0, 0.0, 0.0 }));

  /// creates a transform based on transform type
  MeshTransform createTransform(const Mesh &target, XFormType type = IterativeClosestPoint, AlignmentType align = Similarity, unsigned iterations = 10);

  /// applies the given transformation to the mesh
  Mesh& applyTransform(const MeshTransform transform);

  /// finds holes in a mesh and closes them
  Mesh& fillHoles();

  /// samples data values at specified point locations
  Mesh& probeVolume(const Image &image);

  /// clips a mesh using a cutting plane
  Mesh& clip(const Plane plane);

  /// helper to translate mesh
  Mesh& translate(const Vector3 &v);

  /// helper to scale mesh
  Mesh& scale(const Vector3 &v);

  /// computes bounding box of current mesh
  Region boundingBox(bool center=false) const;

  /// quality control mesh
  Mesh& fix(bool smoothBefore = true, bool smoothAfter = true, double lambda = 0.5, int iterations = 1, bool decimate = true, double percentage = 0.5);

  /// computes surface to surface distance, compute method: POINT_TO_POINT (default) or POINT_TO_CELL
  Mesh& distance(const Mesh &target, const DistanceMethod method = POINT_TO_POINT);

  /// clips a mesh using a cutting plane resulting in a closed surface
  Mesh& clipClosedSurface(const Plane plane);

  /// computes cell normals and orients them such that they point in the same direction
  Mesh& generateNormals();

  /// rasterizes mesh to create binary images, automatically computing size and origin if necessary
  Image toImage(Vector3 spacing = makeVector({1.0, 1.0, 1.0}), Dims size = {0, 0, 0}, Point3 origin = Point3({-1.0, -1.0, -1.0})) const;

  /// converts mesh to distance transform, automatically computing size and origin if necessary
  Image toDistanceTransform(Vector3 spacing = makeVector({1.0, 1.0, 1.0}), Dims size = {0, 0, 0}, Point3 origin = Point3({-1.0, -1.0, -1.0})) const;

  // query functions //

  /// center of mesh
  Point3 center() const;

  /// center of mass of mesh
  Point3 centerOfMass() const;

  /// number of points
  vtkIdType numPoints() const { return mesh->GetNumberOfPoints(); }

  /// number of faces
  vtkIdType numFaces() const { return mesh->GetNumberOfCells(); }

  /// return (x,y,z) coordinates of vertex at given index
  Point3 getPoint(int p) const;

  // fields of mesh points //

  /// print all field names in mesh
  std::vector<std::string> getFieldNames() const;

  /// sets the given field for points with array (*does not copy array's values)
  Mesh& setField(std::string name, Array array);

  /// sets the given field for faces with array (*does not copy array's values)
  Mesh& setFieldForFaces(std::string name, Array array);

  /// gets the field (*does not copy array's values)
  template<typename T>
  vtkSmartPointer<T> getField(const std::string& name) const
  {
    if (mesh->GetPointData()->GetNumberOfArrays() < 1)
      throw std::invalid_argument("Mesh has no fields.");

    auto rawarr = mesh->GetPointData()->GetArray(name.c_str());
    return rawarr;
  }

  /// sets the given index of field to value
  void setFieldValue(const std::string& name, int idx, double value);

  /// gets the value at the given index of field
  double getFieldValue(const std::string& name, int idx) const;

  /// returns the range of the given field
  std::vector<double> getFieldRange(const std::string& name) const;

  /// returns the mean the given field
  double getFieldMean(const std::string& name) const;

  /// returns the standard deviation of the given field
  double getFieldStd(const std::string& name) const;

  // fields of mesh faces //
  // todo: add support for fields of mesh faces (ex: their normals)

  // mesh comparison //

  /// compare if values of the points in two (corresponding) meshes are (eps)equal
  bool compareAllPoints(const Mesh& other_mesh) const;

  /// compare if face indices in two (corresponding) meshes are equal
  bool compareAllFaces(const Mesh& other_mesh) const;

  /// compare if all fields in two meshes are (eps)equal
  bool compareAllFields(const Mesh& other_mesh) const;

  /// compare field of meshes to be (eps)equal (same field for both if only one specified)
  bool compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2="") const;

  // todo: add support for comparison of fields of mesh faces (ex: their normals)

  /// compare meshes
  bool compare(const Mesh& other_mesh) const;

  /// compare meshes
  bool operator==(const Mesh& other) const { return compare(other); }

  // public static functions //

  /// getSupportedTypes
  static std::vector<std::string> getSupportedTypes() { return {"vtk", "vtp", "ply", "stl", "obj"}; }

  bool SplitMesh(std::vector< std::vector< Eigen::Vector3d > > boundaries, Eigen::Vector3d query, size_t dom, size_t num);

  double GetFFCValue(Eigen::Vector3d query);
  Eigen::Vector3d GetFFCGradient(Eigen::Vector3d query);

  //Debug
  vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);

private:
  friend struct SharedCommandData;
  Mesh() : mesh(nullptr) {} // only for use by SharedCommandData since a Mesh should always be valid, never "empty"

  /// reads mesh (used only by constructor)
  static MeshType read(const std::string& pathname);

  /// Creates transform from source mesh to target using ICP registration
  MeshTransform createRegistrationTransform(const Mesh &target, AlignmentType align = Similarity, unsigned iterations = 10);

  MeshType mesh;

  void visualizeVectorFieldForFFCs(vtkSmartPointer<vtkDoubleArray> values, std::vector<Eigen::Matrix3d> face_grad_, Eigen::MatrixXd V, Eigen::MatrixXi F);

  std::vector<Eigen::Matrix3d> setGradientFieldForFFCs(vtkSmartPointer<vtkDoubleArray> absvalues, Eigen::MatrixXd V, Eigen::MatrixXi F);

  vtkSmartPointer<vtkDoubleArray> setDistanceToBoundaryValueFieldForFFCs(vtkSmartPointer<vtkDoubleArray> values, vtkSmartPointer<vtkPoints> points, std::vector<size_t> boundaryVerts, vtkSmartPointer<vtkDoubleArray> inout, Eigen::MatrixXd V, Eigen::MatrixXi F, size_t dom);

  vtkSmartPointer<vtkDoubleArray> computeInOutForFFCs(Eigen::Vector3d query, MeshType halfmesh);

  vtkSmartPointer<vtkPoints> GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const; // Copied directly from VtkMeshWrapper. this->poly_data_ becomes this->mesh

  Eigen::Vector3d ComputeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const;

};

/// stream insertion operators for Mesh
std::ostream& operator<<(std::ostream &os, const Mesh& mesh);

} // shapeworks
