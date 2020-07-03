#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"

#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "CGALGeodesicDistance.h"

using namespace trimesh;

namespace shapeworks
{

class TriMeshWrapper : public MeshWrapper
{
public:

  typedef typename MeshWrapper::PointType PointType;

  double ComputeDistance(PointType pointa, PointType pointb) const override;

  PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<double, DIMENSION> vector) const override;

  vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(const PointType &pointa, vnl_vector_fixed<double, DIMENSION> &vector) const override;

  vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(PointType p) const override;

  PointType SnapToMesh(PointType pointa) const override;

  TriMeshWrapper(TriMesh *_mesh);
  ~TriMeshWrapper() {}

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

  point TriMeshWrapper::GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const;

  int GetNearestVertex(point pt) const;
  int GetTriangleForPoint(point pt) const;
  std::vector<int> GetKNearestVertices(point pt, int k) const;
  vec3 ComputeBarycentricCoordinates(point pt, int face) const;

  TriMesh* mesh;
  KDtree *kdTree;

  CGALGeodesicDistance *cgal;

  PointType meshLowerBound;
  PointType meshUpperBound;
};

}
