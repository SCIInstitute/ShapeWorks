---
title: Libs/Mesh/Mesh.h

---

# Libs/Mesh/Mesh.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Mesh](../Classes/classshapeworks_1_1Mesh.md)**  |
| class | **[shapeworks::MeshReader](../Classes/classshapeworks_1_1MeshReader.md)** <br>reads mesh (used only by one of the [Mesh](../Classes/classshapeworks_1_1Mesh.md) constructors)  |




## Source code

```cpp
#pragma once

#include <Image/ImageUtils.h>

#include "Shapeworks.h"

class vtkStaticCellLocator;
class vtkKdTreePointLocator;

namespace shapeworks {

class Mesh {
 public:
  enum FieldType { Point, Face };
  enum AlignmentType { Rigid, Similarity, Affine };
  enum DistanceMethod { PointToPoint, PointToCell };
  enum CurvatureType { Principal, Gaussian, Mean };
  enum SubdivisionType { Butterfly, Loop };

  Mesh(const std::string& pathname);

  void set_id(int id) { id_ = id; }
  int get_id() const { return id_; }

  Mesh(vtkSmartPointer<vtkPolyData> meshPtr) : poly_data_(meshPtr) {
    if (!poly_data_) throw std::invalid_argument("null meshPtr");
    invalidateLocators();
  }

  Mesh(const Mesh& orig) : poly_data_(vtkSmartPointer<vtkPolyData>::New()) {
    poly_data_->DeepCopy(orig.poly_data_);
    id_ = orig.id_;
    invalidateLocators();
  }

  Mesh(Mesh&& orig) : poly_data_(orig.poly_data_), id_{orig.id_} { orig.poly_data_ = nullptr; }

  Mesh& operator=(const Mesh& orig) {
    poly_data_ = vtkSmartPointer<vtkPolyData>::New();
    poly_data_->DeepCopy(orig.poly_data_);
    id_ = orig.id_;
    invalidateLocators();
    return *this;
  }

  Mesh(const Eigen::MatrixXd& points, const Eigen::MatrixXi& faces);

  Mesh& operator=(Mesh&& orig) {
    poly_data_ = orig.poly_data_;
    id_ = orig.id_;
    orig.poly_data_ = nullptr;
    orig.id_ = -1;
    return *this;
  }

  Mesh& operator+=(const Mesh& otherMesh);

  vtkSmartPointer<vtkPolyData> getVTKMesh() const { return this->poly_data_; }

  Mesh& write(const std::string& pathname, bool binaryFile = false);

  Mesh& coverage(const Mesh& otherMesh, bool allowBackIntersections = true, double angleThreshold = 0,
                 double backSearchRadius = 0);

  Mesh& smooth(int iterations = 0, double relaxation = 0.0);

  Mesh& smoothSinc(int iterations = 0, double passband = 0.0);

  Mesh& remesh(int numVertices, double adaptivity = 1.0);

  Mesh& remeshPercent(double percentage, double adaptivity = 1.0);

  Mesh& invertNormals();

  Mesh& reflect(const Axis& axis, const Vector3& origin = makeVector({0.0, 0.0, 0.0}));

  MeshTransform createTransform(const Mesh& target, AlignmentType align = Similarity, unsigned iterations = 10);

  Mesh& applyTransform(const MeshTransform transform);

  Mesh& rotate(const double angle, const Axis axis);

  Mesh& fillHoles(double hole_size = 1000.0);

  Mesh& clean();

  Mesh& probeVolume(const Image& image);

  Mesh& clip(const Plane plane);

  Mesh& translate(const Vector3& v);

  Mesh& scale(const Vector3& v);

  PhysicalRegion boundingBox() const;

  Mesh& fixElement();

  Mesh& fixNonManifold();

  bool detectNonManifold();

  bool detectTriangular();

  std::vector<Field> distance(const Mesh& target, const DistanceMethod method = PointToCell) const;

  Mesh& clipClosedSurface(const Plane plane);

  Mesh& computeNormals();

  Point3 closestPoint(const Point3 point, double& distance, vtkIdType& face_id) const;

  int closestPointId(const Point3 point) const;

  bool isPointInside(const Point3 point) const;

  double geodesicDistance(int source, int target) const;

  Field geodesicDistance(const Point3 landmark) const;

  Field geodesicDistance(const std::vector<Point3> curve) const;

  Field curvature(const CurvatureType type = Principal) const;

  void computeFieldGradient(const std::string& field) const;

  Eigen::Vector3d computeFieldGradientAtPoint(const std::string& field, const Point3& query) const;

  double interpolateFieldAtPoint(const std::string& field, const Point3& query) const;

  Mesh& applySubdivisionFilter(const SubdivisionType type = Butterfly, int subdivision = 1);

  Image toImage(PhysicalRegion region = PhysicalRegion(), Point3 spacing = Point3({1., 1., 1.})) const;

  Image toDistanceTransform(PhysicalRegion region = PhysicalRegion(), const Point3 spacing = Point3({1., 1., 1.}),
                            const Dims padding = Dims({1, 1, 1})) const;

  Mesh& computeThickness(Image& image, Image* dt = nullptr, double max_dist = 10000, double median_radius = 5.0,
                         std::string distance_mesh = "");

  Mesh& computeLandmarkGeodesics(const std::vector<Point3>& landmarks);

  // query functions //

  Point3 center() const;

  Point3 centerOfMass() const;

  int numPoints() const { return poly_data_->GetNumberOfPoints(); }

  int numFaces() const { return poly_data_->GetNumberOfCells(); }

  Eigen::MatrixXd points() const;

  Eigen::MatrixXi faces() const;

  Point3 getPoint(int id) const;

  IPoint3 getFace(int id) const;

  // fields of mesh points //

  std::vector<std::string> getFieldNames() const;

  Mesh& setField(const std::string name, Array array, const FieldType type);

  Field getField(const std::string& name, const FieldType type) const;

  void setFieldValue(const std::string& name, int idx, double value);

  double getFieldValue(const std::string& name, int idx) const;

  Eigen::VectorXd getMultiFieldValue(const std::string& name, int idx) const;

  // mesh comparison //

  bool compareAllPoints(const Mesh& other_mesh) const;

  bool compareAllFaces(const Mesh& other_mesh) const;

  bool compareAllFields(const Mesh& other_mesh, const double eps = 1e4) const;

  bool compareField(const Mesh& other_mesh, const std::string& name1, const std::string& name2 = "",
                    const double eps = -1.0) const;

  // todo: add support for comparison of fields of mesh faces (ex: their normals)

  bool compare(const Mesh& other_mesh, const double eps = -1.0) const;

  bool operator==(const Mesh& other) const { return compare(other); }

  // public static functions //

  static std::vector<std::string> getSupportedTypes() { return {"vtk", "vtp", "ply", "stl", "obj"}; }

  double getFFCValue(Eigen::Vector3d query) const;

  Eigen::Vector3d getFFCGradient(Eigen::Vector3d query) const;

  vtkSmartPointer<vtkPoints> getIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F)
      const;  // Copied directly from Surface. this->poly_data_ becomes this->mesh. // WARNING: Copied directly
              // from Surface. TODO: When refactoring, take this into account.

  vtkSmartPointer<vtkPolyData> clipByField(const std::string& name, double value);

  vtkSmartPointer<vtkStaticCellLocator> getCellLocator() const {
    updateCellLocator();
    return cellLocator;
  }

  int getClosestFace(const Point3& point) const;

  Eigen::Vector3d computeBarycentricCoordinates(const Eigen::Vector3d& pt, int face)
      const;  // // WARNING: Copied directly from Surface. TODO: When refactoring, take this into account.

  void interpolate_scalars_to_mesh(std::string name, Eigen::VectorXd positions, Eigen::VectorXd scalar_values);

 private:
  friend struct SharedCommandData;
  Mesh()
      : poly_data_(nullptr) {}  // only for use by SharedCommandData since a Mesh should always be valid, never "empty"

  MeshTransform createRegistrationTransform(const Mesh& target, AlignmentType align = Similarity,
                                            unsigned iterations = 10) const;

  Mesh& setFieldForFaces(const std::string name, Array array);

  Field getFieldForFaces(const std::string& name) const;

  void invalidateLocators() const;

  mutable vtkSmartPointer<vtkStaticCellLocator> cellLocator;
  void updateCellLocator() const;

  mutable vtkSmartPointer<vtkKdTreePointLocator> pointLocator;
  void updatePointLocator() const;

  vtkSmartPointer<vtkPolyData> poly_data_;

  int id_{-1};
};

std::ostream& operator<<(std::ostream& os, const Mesh& mesh);

class MeshReader {
  static vtkSmartPointer<vtkPolyData> read(const std::string& pathname);
  friend Mesh::Mesh(const std::string& pathname);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-01-28 at 22:52:41 +0000
