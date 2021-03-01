#pragma once

#include "MeshWrapper.h"
#include "FixedSizeCache.h"

#include <unordered_map>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <geometrycentral/surface/meshio.h>
#include <geometrycentral/surface/heat_method_distance.h>

class vtkCellLocator;

namespace shapeworks {

class VtkMeshWrapper : public MeshWrapper {

public:

  using PointType = MeshWrapper::PointType;
  using GradNType = MeshWrapper::GradNType;
  using NormalType = vnl_vector_fixed<float, DIMENSION>;
  using VectorType = vnl_vector_fixed<double, DIMENSION>;

  explicit VtkMeshWrapper(vtkSmartPointer<vtkPolyData> mesh);

  ~VtkMeshWrapper() = default;

  double ComputeDistance(const PointType& pointa, int idxa,
                         const PointType& pointb, int idxb, VectorType* out_grad) const override;

  bool IsWithinDistance(const PointType& pointa, int idxa,
                        const PointType& pointb, int idxb, double test_dist) const override;

  PointType GeodesicWalk(PointType p, int idx, VectorType vector) const override;

  VectorType ProjectVectorToSurfaceTangent(const PointType &pointa, int idx,
                                           VectorType &vector) const override;

  NormalType SampleNormalAtPoint(PointType p, int idx) const override;
  GradNType SampleGradNAtPoint(PointType p, int idx) const override;
  GradNType SampleGradPAtPoint(PointType p, int idx) const override;

  PointType SnapToMesh(PointType pointa, int idx) const override;

  PointType GetPointOnMesh() const override;

  inline const PointType &GetMeshLowerBound() const override
  {
    return mesh_lower_bound_;
  }

  inline const PointType &GetMeshUpperBound() const override
  {
    return mesh_upper_bound_;
  }

  virtual void InvalidateParticle(int idx) override;

private:

  void ComputeMeshBounds();
  void ComputeGradN(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);
  void ComputeGradP(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);


  int GetTriangleForPoint(const double pt[3], int idx, double closest_point[3]) const;

  Eigen::Vector3d ProjectVectorToFace(const Eigen::Vector3d &normal,
                                      const Eigen::Vector3d &vector) const;

  const Eigen::Vector3d GetFaceNormal(int face_index) const;

  bool IsInTriangle(const double pt[3], int face_index) const;

  Eigen::Vector3d ComputeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const;

  int ComputeFaceAndWeights(const PointType& p, int idx, Eigen::Vector3d &weights) const;

  Eigen::Vector3d
  GeodesicWalkOnFace(Eigen::Vector3d point_a,
                     Eigen::Vector3d projected_vector, int face_index, int &ending_face) const;

  Eigen::Vector3d GetBarycentricIntersection(Eigen::Vector3d start, Eigen::Vector3d end,
                                             int currentFace, int edge) const;

  int GetAcrossEdge(int face, int edge) const;

  int GetFacePointID(int face, int point_id) const;

  int SlideAlongEdge(Eigen::Vector3d &point, Eigen::Vector3d &remainingVector_, int face_,
                     int edge_) const;

  Eigen::Vector3d GetVertexCoords(int vertex_id) const;

  Eigen::Vector3d
  RotateVectorToFace(const Eigen::Vector3d &prev_normal, const Eigen::Vector3d &next_normal,
                     const Eigen::Vector3d &vector) const;

  vtkSmartPointer<vtkPolyData> poly_data_;

  NormalType CalculateNormalAtPoint(VtkMeshWrapper::PointType p, int idx) const;

  // Caches of triangle, normal and position
  // Has to be mutable because all of the accessor APIs are const
  mutable std::vector<int> particle_triangles_;
  mutable std::vector<NormalType> particle_normals_;
  mutable std::vector<PointType> particle_positions_;

  std::vector<GradNType> grad_normals_;
  std::vector<GradNType> grad_positions_;

  // cache of specialized cells for direct access
  std::vector<vtkSmartPointer<vtkTriangle>> triangles_;

  // bounds of the mesh plus some buffer
  PointType mesh_lower_bound_;
  PointType mesh_upper_bound_;

  // cell locator to find closest point on mesh
  vtkSmartPointer<vtkCellLocator> cell_locator_;

  bool IsGeodesicsEnabled() const override
  {
    return this->is_geodesics_enabled_;
  }

  /////////////////////////
  // Geodesic distances

  bool is_geodesics_enabled_{true};

  std::unique_ptr<geometrycentral::surface::SurfaceMesh> gc_mesh_;
  std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> gc_geometry_;
  std::unique_ptr<geometrycentral::surface::HeatMethodDistanceSolver> gc_heatsolver_;

  // Each cache entry stores 3*V double precision numbers
  size_t max_cache_entries_{512*8};

  // Flattened version of libigl's gradient operator
  std::vector<Eigen::Matrix3d> face_grad_;

  // Cache for geodesic distances from a triangle
  using GeodesicsType = geometrycentral::surface::VertexData<double>;
  mutable FixedSizeCache<int, std::array<GeodesicsType, 3>> geo_dist_cache_;

  // Returns true if face f_a is adjacent to face f_b. This uses a non-standard definition of adjacency: return true
  // if f_a and f_b share atleast one vertex
  bool AreFacesAdjacent(int f_a, int f_b) const;

  // Convert the mesh to libigl data structures
  void GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const;

  // Precompute heat data structures for faster geodesic lookups
  void PrecomputeGeodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

  // Returns 3 x (V, ) geodesic distances from a given source triangle's vertices to every other vertex.
  // This reference is invalid once this function is called again
  const std::array<GeodesicsType, 3>& GeodesicsFromTriangle(int f) const;

  // Store some info about the last query. This accelerates the computation because the optimizer generally asks for the
  // distances _from_ the same point as the previous query.
  mutable int geo_lq_pidx_{-1};
  mutable int geo_lq_face_{-1};
  mutable Eigen::Vector3d geo_lq_bary_;
};
}