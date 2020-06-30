#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include <meshFIM.h>

#include "MeshWrapper.h"

namespace shapeworks
{
class TriMeshWrapper : public MeshWrapper
{
public:

  typedef typename MeshWrapper::PointType PointType;

  double ComputeDistance(PointType pointa, PointType pointb) const override;

  PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<float, DIMENSION> vector) const override;

  vnl_vector_fixed<float, DIMENSION> ProjectVectorToSurfaceTangent(const PointType &pointa, vnl_vector_fixed<float, DIMENSION> &vector) const override;

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
  Eigen::Vector3f GeodesicWalkOnFace(Eigen::Vector3f pointa__, Eigen::Vector3f projectedVector__, int faceIndex__, int prevFace__) const;
  Eigen::Vector3f ProjectVectorToFace(const Eigen::Vector3f &normal, const Eigen::Vector3f &vector) const;
  Eigen::Vector3f RotateVectorToFace(const Eigen::Vector3f &prevnormal, const Eigen::Vector3f &nextnormal, const Eigen::Vector3f &vector) const;
  const Eigen::Vector3f GetFaceNormal(int faceIndex) const;
  void ComputeMeshBounds();

  point TriMeshWrapper::GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const;

  TriMesh* mesh;
  meshFIM* fim;


  PointType meshLowerBound;
  PointType meshUpperBound;
};

}
