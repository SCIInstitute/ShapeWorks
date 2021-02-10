#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"
#include "MeshWrapper.h"
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>
#include "igl/heat_geodesics.h"
#include <unordered_map>

namespace shapeworks {

template<class T>
struct GeoCache {
  std::unordered_map<int, int> tri2entry;
  std::vector<T> entries;
};

class TriMeshWrapper : public MeshWrapper {
public:
  explicit TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh);

  ~TriMeshWrapper() = default;

  typedef typename MeshWrapper::PointType PointType;
  typedef typename MeshWrapper::GradNType GradNType;

  double ComputeDistance(PointType pointa, int idx_a,
                         PointType pointb, int idx_b,
                         vnl_vector_fixed<double, DIMENSION> *out_grad=nullptr) const override;

  PointType
  GeodesicWalk(PointType pointa, int idx, vnl_vector_fixed<double, DIMENSION> vector) const override;

  vnl_vector_fixed<double, DIMENSION>
  ProjectVectorToSurfaceTangent(const PointType& pointa, int idx,
                                vnl_vector_fixed<double, DIMENSION>& vector) const override;

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p, int idx) const override;
  GradNType SampleGradNAtPoint(PointType p, int idx) const override;

  PointType SnapToMesh(PointType pointa, int idx) const override;

  PointType GetPointOnMesh() const override;


  inline const PointType& GetMeshLowerBound() const override
  {
    return mesh_lower_bound_;
  }

  inline const PointType& GetMeshUpperBound() const override
  {
    return mesh_upper_bound_;
  }

private:

  Eigen::Vector3d
  GeodesicWalkOnFace(Eigen::Vector3d point_a,
                     Eigen::Vector3d projected_vector, int face_index) const;

  Eigen::Vector3d
  ProjectVectorToFace(const Eigen::Vector3d& normal, const Eigen::Vector3d& vector) const;

  Eigen::Vector3d
  RotateVectorToFace(const Eigen::Vector3d& prev_normal, const Eigen::Vector3d& next_normal,
                     const Eigen::Vector3d& vector) const;

  const Eigen::Vector3d GetFaceNormal(int face_index) const;

  void ComputeMeshBounds();
  void ComputeGradN(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F); // Gradient of normals

  trimesh::point GetBarycentricIntersection(trimesh::vec3 start, trimesh::vec3 end,
                                            int currentFace, int edge) const;

  int GetNearestVertex(trimesh::point pt) const;
  int GetTriangleForPoint(trimesh::point pt, int idx, trimesh::vec& bary) const;
  std::vector<int> GetKNearestVertices(trimesh::point pt, int k) const;
  trimesh::vec3 ComputeBarycentricCoordinates(trimesh::point pt, int face) const;

  static inline bool IsBarycentricCoordinateValid(const trimesh::vec3& b);

  // IGL Helper functions
  void GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const;

  std::shared_ptr<trimesh::TriMesh> mesh_;
  std::shared_ptr<trimesh::KDtree> kd_tree_;

  // Maintains a map of particle index -> triangle index
  // Has to be mutable because all of the accessor APIs are const
  mutable std::vector<int> particle2tri_;

  std::vector<GradNType> grad_normals_;

  PointType mesh_lower_bound_;
  PointType mesh_upper_bound_;

  void InvalidateBary(const PointType& p, int idx) const override;

  /////////////////////////
  // Geodesic distances

  // Precompute libigl heat data structures for faster geodesic lookups
  void PrecomputeGeodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

  // Returns 3 x (V, ) geodesic distances from a given source triangle's vertices to every other vertex
  const std::array<Eigen::VectorXd, 3>& GeodesicsFromTriangle(int f) const;

  // Returns 3 x (F, 3) gradient of geodesic distances from a given source triangle's vertices to every other face
  const std::array<Eigen::MatrixXd, 3>& GradGeodesicsFromTriangle(int f) const;

  // Returns true if face f_a is adjacent to face f_b
  bool AreFacesAdjacent(int f_a, int f_b) const;

  size_t max_cache_entries_ {256};

  // Cache to store information for geodesics
  mutable igl::HeatGeodesicsData<double> heat_data;
  // cache for geodesic distances from a triangle
  mutable GeoCache<std::array<Eigen::VectorXd, 3>> geo;
  // cache for gradient of geodesic distances from a triangle
  mutable GeoCache<std::array<Eigen::MatrixXd, 3>> grad_geo;
  // Gradient operator
  mutable Eigen::SparseMatrix<double> grad_operator_;
  mutable std::vector<trimesh::vec3> particle2bary_;
};

}
