#pragma once

#include <iostream>
#include <fstream>
#include <limits>

#include "vnl/vnl_vector_fixed.h"
#include "TriMesh.h"
#include "KDtree.h"

#include "MeshWrapper.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <unordered_map>
#include <utility>
#include <hash_map>

struct pair_hash {
public:
  size_t operator()(std::pair<int, int> x) const throw() {
    size_t h = x.first * x.second;
    return h;
  }
};

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
  void StartLogging(const std::string& filepath) {
    typedef std::numeric_limits< double > dbl;
    logFile.open(filepath);
    logFile.precision(dbl::max_digits10);
  }
  void AddLineToLogFile(const std::string& line) {
    logFile << line;
  }
  void WriteCountsAndClear() {
    for (auto it : counts) {
      logFile << it.first.first << ' ' << it.first.second<< ' '  << it.second << std::endl;
    }
    counts.clear();
  }

private:
  Eigen::Vector3d GeodesicWalkOnFace(Eigen::Vector3d pointa__, Eigen::Vector3d projectedVector__, int faceIndex__, int prevFace__) const;
  Eigen::Vector3d ProjectVectorToFace(const Eigen::Vector3d &normal, const Eigen::Vector3d &vector) const;
  Eigen::Vector3d RotateVectorToFace(const Eigen::Vector3d &prevnormal, const Eigen::Vector3d &nextnormal, const Eigen::Vector3d &vector) const;
  const Eigen::Vector3d GetFaceNormal(int faceIndex) const;
  void ComputeMeshBounds();

  point GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const;

  int GetNearestVertex(point pt) const;
  int GetTriangleForPoint(point pt) const;
  std::vector<int> GetKNearestVertices(point pt, int k) const;
  vec3 ComputeBarycentricCoordinates(point pt, int face) const;

  TriMesh* mesh;
  KDtree *kdTree;

  PointType meshLowerBound;
  PointType meshUpperBound;
  mutable std::ofstream logFile;
  mutable std::unordered_map<
  std::pair<int, int>,
  unsigned long, pair_hash > counts;
};

}
