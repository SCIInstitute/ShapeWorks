---
title: Libs/Mesh/Mesh.h

---

# Libs/Mesh/Mesh.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md)**  |




## Source code

```cpp
#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"

class vtkCellLocator;
#include <vtkPointData.h>

namespace shapeworks {

class Mesh
{
public:
  enum AlignmentType { Rigid, Similarity, Affine };
  enum DistanceMethod { PointToPoint, PointToCell };
  enum CurvatureType { Principal, Gaussian, Mean };

  using MeshType = vtkSmartPointer<vtkPolyData>;

  Mesh(const std::string& pathname) : mesh(read(pathname)) {}
  Mesh(MeshType meshPtr) : mesh(meshPtr) { if (!mesh) throw std::invalid_argument("null meshPtr"); }
  Mesh(const Mesh& orig) : mesh(MeshType::New()) { mesh->DeepCopy(orig.mesh); }
  Mesh(Mesh&& orig) : mesh(orig.mesh) { orig.mesh = nullptr; }
  Mesh& operator=(const Mesh& orig) { mesh = MeshType::New(); mesh->DeepCopy(orig.mesh); return *this; }
  Mesh& operator=(Mesh&& orig) { mesh = orig.mesh; orig.mesh = nullptr; return *this; }

  Mesh& operator+=(const Mesh& otherMesh);

  MeshType getVTKMesh() const { return this->mesh; }

  Mesh& write(const std::string &pathname);

  Mesh& coverage(const Mesh& otherMesh, bool allowBackIntersections = true,
                 double angleThreshold = 0, double backSearchRadius = 0);

  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  Mesh& smoothSinc(int iterations = 0, double passband = 0.0);

  Mesh& decimate(double reduction = 0.5, double angle = 15.0, bool preserveTopology = true);

  Mesh& cvdDecimate(double percentage = 0.5);

  Mesh& invertNormals();

  Mesh& reflect(const Axis &axis, const Vector3 &origin = makeVector({ 0.0, 0.0, 0.0 }));

  MeshTransform createTransform(const Mesh &target, AlignmentType align = Similarity, unsigned iterations = 10);

  Mesh& applyTransform(const MeshTransform transform);

  Mesh& fillHoles();

  Mesh& probeVolume(const Image &image);

  Mesh& clip(const Plane plane);

  Mesh& translate(const Vector3 &v);

  Mesh& scale(const Vector3 &v);

  PhysicalRegion boundingBox() const;

  Mesh& fixElement();

  Mesh& distance(const Mesh &target, const DistanceMethod method = PointToPoint);

  Mesh& clipClosedSurface(const Plane plane);

  Mesh& computeNormals();

  Point3 closestPoint(const Point3 point);

  int closestPointId(const Point3 point);

  double geodesicDistance(int source, int target);

  Field geodesicDistance(const Point3 landmark);

  Field geodesicDistance(const std::vector<Point3> curve);

  Field curvature(const CurvatureType type = Principal);

  Image toImage(PhysicalRegion region = PhysicalRegion(), Point spacing = Point({1., 1., 1.})) const;

  Image toDistanceTransform(PhysicalRegion region = PhysicalRegion(), Point spacing = Point({1., 1., 1.})) const;

  // query functions //

  Point3 center() const;

  Point3 centerOfMass() const;

  int numPoints() const { return mesh->GetNumberOfPoints(); }

  int numFaces() const { return mesh->GetNumberOfCells(); }

  Eigen::MatrixXd points() const;

  Eigen::MatrixXi faces() const;

  Point3 getPoint(int id) const;

  IPoint3 getFace(int id) const;

  // fields of mesh points //

  std::vector<std::string> getFieldNames() const;

  Mesh& setField(std::string name, Array array);

  Mesh& setFieldForFaces(std::string name, Array array);

  template<typename T>
  vtkSmartPointer<T> getField(const std::string& name) const
  {
    if (mesh->GetPointData()->GetNumberOfArrays() < 1)
      throw std::invalid_argument("Mesh has no fields.");

    auto rawarr = mesh->GetPointData()->GetArray(name.c_str());
    return rawarr;
  }

  void setFieldValue(const std::string& name, int idx, double value);

  double getFieldValue(const std::string& name, int idx) const;

  Eigen::VectorXd getMultiFieldValue(const std::string& name, int idx) const;

  std::vector<double> getFieldRange(const std::string& name) const;

  double getFieldMean(const std::string& name) const;

  double getFieldStd(const std::string& name) const;

  // fields of mesh faces //
  // todo: add support for fields of mesh faces (ex: their normals)

  // mesh comparison //

  bool compareAllPoints(const Mesh& other_mesh) const;

  bool compareAllFaces(const Mesh& other_mesh) const;

  bool compareAllFields(const Mesh& other_mesh, const double eps=-1.0) const;

  bool compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2="", const double eps=-1.0) const;

  // todo: add support for comparison of fields of mesh faces (ex: their normals)

  bool compare(const Mesh& other_mesh, const double eps=-1.0) const;

  bool operator==(const Mesh& other) const { return compare(other); }

  // public static functions //

  static std::vector<std::string> getSupportedTypes() { return {"vtk", "vtp", "ply", "stl", "obj"}; }

  bool splitMesh(std::vector< std::vector< Eigen::Vector3d > > boundaries, Eigen::Vector3d query, size_t dom, size_t num);

  double getFFCValue(Eigen::Vector3d query);
  Eigen::Vector3d getFFCGradient(Eigen::Vector3d query);

  vtkSmartPointer<vtkPoints> getIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const; // Copied directly from VtkMeshWrapper. this->poly_data_ becomes this->mesh. // WARNING: Copied directly from Meshwrapper. TODO: When refactoring, take this into account.

private:
  friend struct SharedCommandData;
  Mesh() : mesh(nullptr) {} // only for use by SharedCommandData since a Mesh should always be valid, never "empty"

  static MeshType read(const std::string& pathname);

  MeshTransform createRegistrationTransform(const Mesh &target, AlignmentType align = Similarity, unsigned iterations = 10);

  MeshType mesh;

  vtkSmartPointer<vtkCellLocator> locator;

  std::vector<Eigen::Matrix3d> setGradientFieldForFFCs(vtkSmartPointer<vtkDoubleArray> absvalues, Eigen::MatrixXd V, Eigen::MatrixXi F);

  vtkSmartPointer<vtkDoubleArray> setDistanceToBoundaryValueFieldForFFCs(vtkSmartPointer<vtkDoubleArray> values, vtkSmartPointer<vtkPoints> points, std::vector<size_t> boundaryVerts, vtkSmartPointer<vtkDoubleArray> inout, Eigen::MatrixXd V, Eigen::MatrixXi F, size_t dom);

  vtkSmartPointer<vtkDoubleArray> computeInOutForFFCs(Eigen::Vector3d query, MeshType halfmesh);

  Eigen::Vector3d computeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const; // // WARNING: Copied directly from Meshwrapper. TODO: When refactoring, take this into account.


};

std::ostream& operator<<(std::ostream &os, const Mesh& mesh);



} // shapeworks
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000
