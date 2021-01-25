#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"
#include <unordered_map>

#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>
#include "igl/heat_geodesics.h"

namespace shapeworks {

class TriMeshWrapper : public MeshWrapper {
public:
  explicit TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh);

  ~TriMeshWrapper() = default;

  typedef typename MeshWrapper::PointType PointType;
  typedef typename MeshWrapper::HessianType HessianType;

  double ComputeDistance(PointType pointa, int idx_a,
                         PointType pointb, int idx_b,
                         vnl_vector_fixed<double, DIMENSION> *out_grad=nullptr) const override;

  PointType
  GeodesicWalk(PointType pointa, int idx, vnl_vector_fixed<double, DIMENSION> vector) const override;

  vnl_vector_fixed<double, DIMENSION>
  ProjectVectorToSurfaceTangent(const PointType& pointa, int idx,
                                vnl_vector_fixed<double, DIMENSION>& vector) const override;

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p, int idx) const override;
  HessianType SampleGradNAtPoint(PointType p, int idx) const override;

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


  std::vector<HessianType> grad_normals_;

  PointType mesh_lower_bound_;
  PointType mesh_upper_bound_;

  struct ParticleCacheEntry {
    int tri_idx{0};

    bool is_geo_dist_valid{false};
    Eigen::VectorXd dist[3];

    bool is_geo_grad_valid{false};
    Eigen::MatrixXd grad_dist[3];

    void invalidate_geodesics() {
      is_geo_dist_valid = is_geo_grad_valid = false;
      std::cout << "invalidate: " << tri_idx <<"\n";
    }
  };
  // Maintains a map of particle index -> triangle index
  // Has to be mutable because all of the accessor APIs are const
  mutable std::vector<ParticleCacheEntry> particle_cache_;

  // Precompute libigl heat data structures for faster geodesic lookups
  void PrecomputeGeodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

  // Returns (V, ) geodesic distances from a given source vertex to every other vertex
  void GeodesicsFromVertex(int v, Eigen::VectorXd& D) const;

  // Returns (V, 3) gradient of geodesics given the geodesics to every vertex
  //todo can we rename this
  void GradGeodesicsFromGeodesics(const Eigen::VectorXd& D, Eigen::MatrixXd& GD) const;

  // Ensure particle with idx has geodesic distances in cache
  void EnsureHasGeodesics(int idx) const;

  // Ensure particle with idx has gradient of geodesics in cache
  void EnsureHasGradGeodesics(int idx) const;

  // Returns true if face f_a is adjacent to face f_b
  bool AreFacesAdjacent(int f_a, int f_b) const;

  // Cache to store information for geodesics
  mutable struct {
    igl::HeatGeodesicsData<double> heat_data;
    Eigen::SparseMatrix<double> G; // Gradient operator
  } geodesic_cache_;
};

}
