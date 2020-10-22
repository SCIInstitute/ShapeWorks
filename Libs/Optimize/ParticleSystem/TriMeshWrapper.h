#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"

#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>


namespace shapeworks
{

class TriMeshWrapper : public MeshWrapper
{
public:

  TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> _mesh);
  ~TriMeshWrapper() {}

  typedef typename MeshWrapper::PointType PointType;

  double ComputeDistance(PointType pointa, PointType pointb) const override;

  PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<double, DIMENSION> vector) const override;

  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(const PointType &pointa, vnl_vector_fixed<double, DIMENSION> &vector) const override;

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p) const override;

  PointType SnapToMesh(PointType pointa) const override;


  PointType GetPointOnMesh() const override;

  inline const PointType &GetMeshLowerBound() const override {
    return meshLowerBound;
  }
  inline const PointType &GetMeshUpperBound() const override {
    return meshUpperBound;
  }

private:
  Eigen::Vector3d GeodesicWalkOnFace(Eigen::Vector3d pointa__, Eigen::Vector3d projectedVector__, int faceIndex__, int prevFace__) const;
  Eigen::Vector3d ProjectVectorToFace(const Eigen::Vector3d &normal, const Eigen::Vector3d &vector) const;
  Eigen::Vector3d RotateVectorToFace(const Eigen::Vector3d &prevnormal, const Eigen::Vector3d &nextnormal, const Eigen::Vector3d &vector) const;
  const Eigen::Vector3d GetFaceNormal(int faceIndex) const;
  void ComputeMeshBounds();

  trimesh::point GetBarycentricIntersection(trimesh::vec3 start, trimesh::vec3 end, int currentFace, int edge) const;

  int GetNearestVertex(trimesh::point pt) const;
  int GetTriangleForPoint(trimesh::point pt) const;
  std::vector<int> GetKNearestVertices(trimesh::point pt, int k) const;
  trimesh::vec3 ComputeBarycentricCoordinates(trimesh::point pt, int face) const;

  std::shared_ptr<trimesh::TriMesh> mesh;
  std::shared_ptr<trimesh::KDtree> kdTree;

  PointType meshLowerBound;
  PointType meshUpperBound;
};

}
