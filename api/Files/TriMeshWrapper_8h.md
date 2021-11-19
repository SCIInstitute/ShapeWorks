---
title: Libs/Optimize/ParticleSystem/TriMeshWrapper.h

---

# Libs/Optimize/ParticleSystem/TriMeshWrapper.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::TriMeshWrapper](../Classes/classshapeworks_1_1TriMeshWrapper.md)**  |




## Source code

```cpp
#pragma once

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"
#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <vector>

namespace shapeworks {

class TriMeshWrapper : public MeshWrapper {
public:
  explicit TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh);

  ~TriMeshWrapper() = default;

  typedef typename MeshWrapper::PointType PointType;
  typedef typename MeshWrapper::GradNType GradNType;

  double ComputeDistance(const PointType& pointa, int idx_a, const PointType& pointb, int idx_b, vnl_vector_fixed<double, 3>* out_grad) const override;

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

  bool IsGeodesicsEnabled() const override
  {
    return false;
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
  void ComputeGradN(); // Gradient of normals

  trimesh::point GetBarycentricIntersection(trimesh::vec3 start, trimesh::vec3 end,
                                            int currentFace, int edge) const;

  int GetNearestVertex(trimesh::point pt) const;
  int GetTriangleForPoint(trimesh::point pt, int idx, trimesh::vec& bary) const;
  std::vector<int> GetKNearestVertices(trimesh::point pt, int k) const;
  trimesh::vec3 ComputeBarycentricCoordinates(trimesh::point pt, int face) const;

  static inline bool IsBarycentricCoordinateValid(const trimesh::vec3& b);

  // IGL Helper functions
  void GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F);

  std::shared_ptr<trimesh::TriMesh> mesh_;
  std::shared_ptr<trimesh::KDtree> kd_tree_;

  // Maintains a map of particle index -> triangle index
  // Has to be mutable because all of the accessor APIs are const
  mutable std::vector<int> particle2tri_;

  std::vector<GradNType> grad_normals_;

  PointType mesh_lower_bound_;
  PointType mesh_upper_bound_;
};

}
```


-------------------------------

Updated on 2021-11-19 at 20:38:59 +0000
