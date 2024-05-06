---
title: Libs/Optimize/Domain/MeshWrapper.h

---

# Libs/Optimize/Domain/MeshWrapper.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md)**  |




## Source code

```cpp
#pragma once

#include <geometrycentral/surface/heat_method_distance.h>
#include <geometrycentral/surface/meshio.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <unordered_map>
#include <unordered_set>

#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "MeshGeoEntry.h"
#include "MeshWrapper.h"

class vtkCellLocator;

namespace shapeworks {

class MeshWrapper {
 public:
  using PointType = ParticleDomain::PointType;
  using GradNType = ParticleDomain::GradNType;

  using NormalType = vnl_vector_fixed<float, 3>;
  using VectorType = vnl_vector_fixed<double, 3>;

  explicit MeshWrapper(vtkSmartPointer<vtkPolyData> mesh, bool geodesics_enabled = false,
                       size_t geodesics_cache_multiplier_size = 0);  // 0 => MeshWrapper will choose a heuristic

  ~MeshWrapper() = default;

  double ComputeDistance(const PointType& pointa, int idxa, const PointType& pointb, int idxb,
                         VectorType* out_grad = nullptr) const;

  bool IsWithinDistance(const PointType& pointa, int idxa, const PointType& pointb, int idxb, double test_dist,
                        double& dist) const;

  PointType GeodesicWalk(PointType p, int idx, VectorType vector) const;

  VectorType ProjectVectorToSurfaceTangent(const PointType& pointa, int idx, VectorType& vector) const;

  NormalType SampleNormalAtPoint(PointType p, int idx = -1) const;
  GradNType SampleGradNAtPoint(PointType p, int idx) const;

  PointType SnapToMesh(PointType pointa, int idx) const;

  PointType GetPointOnMesh() const;

  inline const PointType& GetMeshLowerBound() const { return mesh_lower_bound_; }

  inline const PointType& GetMeshUpperBound() const { return mesh_upper_bound_; }

  virtual void InvalidateParticle(int idx);

  inline vtkSmartPointer<vtkPolyData> GetPolydata() const { return original_mesh_; }

  bool IsGeodesicsEnabled() const { return this->is_geodesics_enabled_; }

 private:
  void ComputeMeshBounds();
  void ComputeGradN(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

  int GetTriangleForPoint(const double pt[3], int idx, double closest_point[3]) const;

  Eigen::Vector3d ProjectVectorToFace(const Eigen::Vector3d& normal, const Eigen::Vector3d& vector) const;

  const Eigen::Vector3d GetFaceNormal(int face_index) const;

  bool IsInTriangle(const double pt[3], int face_index) const;

  Eigen::Vector3d ComputeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const;

  int ComputeFaceAndWeights(const PointType& p, int idx, Eigen::Vector3d& weights) const;

  Eigen::Vector3d GeodesicWalkOnFace(Eigen::Vector3d point_a, Eigen::Vector3d projected_vector, int face_index,
                                     int& ending_face) const;

  Eigen::Vector3d GetBarycentricIntersection(Eigen::Vector3d start, Eigen::Vector3d end, int currentFace,
                                             int edge) const;

  int GetAcrossEdge(int face, int edge) const;

  int GetFacePointID(int face, int point_id) const;

  int SlideAlongEdge(Eigen::Vector3d& point, Eigen::Vector3d& remainingVector_, int face_, int edge_) const;

  Eigen::Vector3d GetVertexCoords(int vertex_id) const;

  Eigen::Vector3d RotateVectorToFace(const Eigen::Vector3d& prev_normal, const Eigen::Vector3d& next_normal,
                                     const Eigen::Vector3d& vector) const;

  vtkSmartPointer<vtkPolyData> poly_data_;
  vtkSmartPointer<vtkPolyData> original_mesh_;

  NormalType CalculateNormalAtPoint(MeshWrapper::PointType p, int idx) const;

  // Caches of triangle, normal and position
  // Has to be mutable because all of the accessor APIs are const
  mutable std::vector<int> particle_triangles_;
  mutable std::vector<NormalType> particle_normals_;
  mutable std::vector<PointType> particle_positions_;
  mutable std::vector<double> particle_neighboorhood_;

  std::vector<GradNType> grad_normals_;

  // cache of specialized cells for direct access
  std::vector<vtkSmartPointer<vtkTriangle>> triangles_;

  // bounds of the mesh plus some buffer
  PointType mesh_lower_bound_;
  PointType mesh_upper_bound_;

  // cell locator to find closest point on mesh
  vtkSmartPointer<vtkCellLocator> cell_locator_;

  // Geodesic distances

  bool is_geodesics_enabled_{false};

  // Geometry Central data structures
  std::unique_ptr<geometrycentral::surface::SurfaceMesh> gc_mesh_;
  std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> gc_geometry_;
  std::unique_ptr<geometrycentral::surface::HeatMethodDistanceSolver> gc_heatsolver_;

  size_t geo_max_cache_entries_{0};
  mutable size_t geo_cache_size_{0};

  // Flattened version of libigl's gradient operator
  std::vector<Eigen::Matrix3d> face_grad_;

  std::vector<std::unordered_set<int>> face_kring_;

  // Cache for geodesic distances from a triangle
  mutable std::vector<MeshGeoEntry> geo_dist_cache_;

  // Returns true if face f_a is in the K-ring of face f_b
  bool AreFacesInKRing(int f_a, int f_b) const;
  const size_t kring_{1};

  // Convert the mesh to libigl data structures
  void GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const;

  // Precompute heat data structures for faster geodesic lookups
  void PrecomputeGeodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

  void ComputeKRing(int f, int k, std::unordered_set<int>& ring) const;

  const MeshGeoEntry& GeodesicsFromTriangle(int f, double max_dist = std::numeric_limits<double>::max(),
                                            int req_target_f = -1) const;
  const Eigen::Matrix3d GeodesicsFromTriangleToTriangle(int f_a, int f_b) const;
  void ClearGeodesicCache() const;

  // Store some info about the last query. This accelerates the computation
  // because the optimizer generally asks for the distances _from_ the same
  // point as the previous query.
  mutable bool geo_lq_cached_{false};
  mutable PointType geo_lq_pt_a_{-1};
  mutable int geo_lq_face_{-1};
  mutable Eigen::Vector3d geo_lq_bary_;
  void FetchAndCacheFirstPoint(const PointType pt_a, int idx_a, int& face_a, Eigen::Vector3d& bary_a) const;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-06 at 23:55:36 +0000
