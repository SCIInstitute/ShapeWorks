#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"

#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace shapeworks {

class TriMeshWrapper : public MeshWrapper {
public:

  explicit TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh);

  ~TriMeshWrapper() = default;

  typedef typename MeshWrapper::PointType PointType;

  double ComputeDistance(const PointType pointTypeA,
                         const PointType pointTypeB) const override;

  PointType GeodesicWalk(const PointType pointType,
                         const vnl_vector_fixed<double, DIMENSION>& vector) const override;

  vnl_vector_fixed<double, DIMENSION>
  ProjectVectorToSurfaceTangent(const PointType& pointType,
                                vnl_vector_fixed<double, DIMENSION>& vector) const override;

  vnl_vector_fixed<float, DIMENSION>
  SampleNormalAtPoint(const PointType pointType) const override;

  PointType GetPointOnMesh() const override;

  PointType ClosestPointOnMesh(const PointType pointType) const override;

  inline const PointType& GetMeshLowerBound() const override
  {
    return m_mesh_lower_bound;
  }

  inline const PointType& GetMeshUpperBound() const override
  {
    return m_mesh_upper_bound;
  }

private:
  void ComputeMeshBounds();

  Eigen::Vector3d
  GeodesicWalkOnFace(const Eigen::Vector3d pt,
                     const int face,
                     const Eigen::Vector3d projectedVector) const;

  int
  GeodesicWalkTowardsEdge(      Eigen::Vector3d & current,
                          const Eigen::Vector3d target,
                          const int face ) const;

  int
  GeodesicWalkOnEdge(      Eigen::Vector3d & current,
                     const int face,
                     const int edge,
                           Eigen::Vector3d& remainingVector) const;


  Eigen::Vector3d
  ProjectVectorToFace(const Eigen::Vector3d& normal,
                      const Eigen::Vector3d& vector) const;

  Eigen::Vector3d
  RotateVectorToFace(const Eigen::Vector3d& prevNormal,
                     const Eigen::Vector3d& nextNormal,
                     const Eigen::Vector3d& vector) const;

  const Eigen::Vector3d GetFaceNormal(const int face) const;


  int GetNearestVertex   (const trimesh::point pt) const;
  int GetTriangleForPoint(const trimesh::point pt) const;

  std::vector<int> GetNearestVertices(const trimesh::point pt,
                                      const int nVerts) const;

  trimesh::point ProjectPointOnToFace         (const trimesh::point pt,
                                               const int face) const;

  trimesh::point ClosestPointOnFace           (const trimesh::point pt,
                                               const int face) const;

  trimesh::vec3  ComputeBarycentricCoordinates(const trimesh::point pt,
                                               const int face) const;

  trimesh::point GetBarycentricIntersection(const trimesh::vec3 start,
                                            const trimesh::vec3 end,
                                            const int face,
                                            const int edge) const;

  // Data
  std::shared_ptr<trimesh::TriMesh> m_mesh;
  std::shared_ptr<trimesh::KDtree> m_kd_tree;

  PointType m_mesh_lower_bound;
  PointType m_mesh_upper_bound;
};

}
