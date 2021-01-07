
#include "TriMeshWrapper.h"

#include <igl/grad.h>
#include <igl/per_vertex_normals.h>
#include <igl/doublearea.h>

#include <set>

using namespace trimesh;

namespace shapeworks {
namespace {
static float epsilon = 1e-6;

template<class T>
inline std::string PrintValue(T value)
{
  return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " +
         std::to_string(value[2]) + ")";
}

inline void PrintTriangle(TriMesh* mesh, int face)
{
  for (int i = 0; i < 3; i++) {
    fprintf(stderr, "v[%d]=%s\n", i,
            PrintValue<point>(mesh->vertices[mesh->faces[face][i]]).c_str());
  }
}

template<class FROM, class TO>
inline TO convert(FROM& value)
{
  TO converted;
  converted[0] = value[0];
  converted[1] = value[1];
  converted[2] = value[2];
  return converted;
}
}

TriMeshWrapper::TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh)
{
  mesh_ = mesh;
  mesh_->need_neighbors();
  mesh_->need_faces();
  mesh_->need_adjacentfaces();
  mesh_->need_across_edge();
  mesh_->need_normals();
  mesh_->need_curvatures();
  ComputeMeshBounds();
  ComputeGradN();

  kd_tree_ = std::make_shared<KDtree>(mesh_->vertices);
}

double TriMeshWrapper::ComputeDistance(PointType pointa, PointType pointb) const
{
  return pointa.EuclideanDistanceTo(pointb);
}

/** start in barycentric coords of currentFace
    end in barycentric coords of currentFace
    edge is the edge we are intersecting with.
*/
point
TriMeshWrapper::GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const
{
  vec3 delta = end - start;
  if (delta[edge] == 0) {
    // If going parallel to the edge, it is allowed to go all the way to the end where it wants to go
    return end;
  }
  double ratio = -start[edge] / delta[edge];
  vec3 intersect = start + delta * vec3(ratio, ratio, ratio);

  point inter(0, 0, 0);
  for (int q = 0; q < 3; q++) {
    inter += mesh_->vertices[mesh_->faces[currentFace][q]] * intersect[q];
  }
  return inter;
}

int SlideAlongEdge(Eigen::Vector3d& point_, Eigen::Vector3d& remainingVector_, int face_, int edge_,
                   std::shared_ptr<trimesh::TriMesh> mesh)
{
  int indexa = (edge_ + 1) % 3;
  int indexb = (edge_ + 2) % 3;
  int vertexindexa = mesh->faces[face_][indexa];
  int vertexindexb = mesh->faces[face_][indexb];
  Eigen::Vector3d pointa = convert<trimesh::point, Eigen::Vector3d>(mesh->vertices[vertexindexa]);
  Eigen::Vector3d pointb = convert<trimesh::point, Eigen::Vector3d>(mesh->vertices[vertexindexb]);
  Eigen::Vector3d meshEdge(pointb[0] - pointa[0], pointb[1] - pointa[1], pointb[2] - pointa[2]);
  meshEdge.normalize();
  double dotprod = meshEdge.dot(remainingVector_);
  Eigen::Vector3d projectedVector = meshEdge.normalized() * dotprod;

  Eigen::Vector3d maxSlide = pointb - point_;
  double newDot = projectedVector.dot(meshEdge);
  int towardsEdge = indexa;
  if (newDot < 0) {
    // going in opposite direction as mesh edge
    meshEdge = -meshEdge;
    maxSlide = pointa - point_;
    towardsEdge = indexb;
  }
  if (projectedVector.norm() > maxSlide.norm()) {
    point_ += maxSlide;
    remainingVector_ = projectedVector - maxSlide;
    return mesh->across_edge[face_][towardsEdge];
  }
  else {
    point_ += projectedVector;
    remainingVector_ = Eigen::Vector3d(0, 0, 0);
    return face_;
  }
}

Eigen::Vector3d TriMeshWrapper::GeodesicWalkOnFace(Eigen::Vector3d point_a,
                                                   Eigen::Vector3d projected_vector,
                                                   int face_index) const
{

  int currentFace = face_index;
  Eigen::Vector3d currentPoint = point_a;
  Eigen::Vector3d remainingVector = projected_vector;
  double minimumUpdate = 0.0000000001;
  double barycentricEpsilon = 0.0001;
  std::vector<int> facesTraversed;
  while (remainingVector.norm() > minimumUpdate && currentFace != -1) {
    facesTraversed.push_back(currentFace);
    vec3 currentBary = ComputeBarycentricCoordinates(
      point(currentPoint[0], currentPoint[1], currentPoint[2]), currentFace);
    Eigen::Vector3d targetPoint = currentPoint + remainingVector;
    vec3 targetBary = ComputeBarycentricCoordinates(
      point(targetPoint[0], targetPoint[1], targetPoint[2]), currentFace);

    if (facesTraversed.size() >= 3 &&
        facesTraversed[facesTraversed.size() - 1] == facesTraversed[facesTraversed.size() - 3]) {
      // When at the intersection of two faces while also being at the edge of the mesh, the edge-sliding will keep alternating
      // between the two faces without actually going anywhere since it is at a corner in the mesh.
      //std::cerr << "exiting due to face repetition\n";
      break;
    }
    if (facesTraversed.size() > 100) {
      for (int i = 0; i < facesTraversed.size(); i++) {
        std::cerr << facesTraversed[i] << ": "
                  << PrintValue<TriMesh::Face>(mesh_->faces[facesTraversed[i]]) << ", ";
      }
      std::cerr << "Current point: " << PrintValue<Eigen::Vector3d>(currentPoint) << "\n";
      std::cerr << "remaining vector: " << PrintValue<Eigen::Vector3d>(remainingVector) << "\n";
      std::cerr << "currentBary: " << PrintValue<vec3>(currentBary) << "\n";
      std::cerr << "targetPoint: " << PrintValue<Eigen::Vector3d>(targetPoint) << "\n";
      std::cerr << "targetBary: " << PrintValue<vec3>(targetBary) << "\n";
      std::cerr << std::endl;
      break;
    }

    if (targetBary[0] + barycentricEpsilon >= 0 && targetBary[1] + barycentricEpsilon >= 0 &&
        targetBary[2] + barycentricEpsilon >= 0 && targetBary[0] - barycentricEpsilon <= 1 &&
        targetBary[1] - barycentricEpsilon <= 1 && targetBary[2] - barycentricEpsilon <= 1) {
      currentPoint = targetPoint;
      break;
    }
    int positiveVertex = -1;
    std::vector<int> negativeVertices;
    for (int i = 0; i < 3; i++) {
      if (targetBary[i] >= 0) {
        positiveVertex = i;
      }
      else {
        negativeVertices.push_back(i);
      }
    }

    if (negativeVertices.size() == 0 || negativeVertices.size() > 2) {
      std::cerr << "ERROR ERROR invalid number of negative vertices. Point is not on surface.\n";
      break;
    }
    int negativeEdge = negativeVertices[0];
    auto value = GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge);
    Eigen::Vector3d intersect = convert<point, Eigen::Vector3d>(value);

    // When more than 1 negative barycentric coordinate, compute both intersections and take the closest one.
    if (negativeVertices.size() == 2) {
      int negativeEdge1 = negativeVertices[1];
      auto value2 = GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge1);
      Eigen::Vector3d intersect1 = convert<point, Eigen::Vector3d>(value2);

      double length0 = (intersect - currentPoint).norm();
      double length1 = (intersect1 - currentPoint).norm();

      if (length0 < length1) {
        intersect = intersect;
        negativeEdge = negativeEdge;
      }
      else {
        intersect = intersect1;
        negativeEdge = negativeEdge1;
      }
    }

    Eigen::Vector3d remaining = targetPoint - intersect;
    int nextFace = mesh_->across_edge[currentFace][negativeEdge];
    if (nextFace == -1) {
      nextFace = SlideAlongEdge(intersect, remaining, currentFace, negativeEdge, mesh_);
    }
    remainingVector = remaining;
    if (nextFace != -1) {
      remainingVector = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(nextFace),
                                           remainingVector);
    }
    currentPoint = intersect;
    currentFace = nextFace;
  }
  return currentPoint;
}

TriMeshWrapper::PointType
TriMeshWrapper::GeodesicWalk(PointType pointa, int idx, vnl_vector_fixed<double, DIMENSION> vector) const
{

  PointType snapped = this->SnapToMesh(pointa, idx);
  vec3 bary;
  int faceIndex = GetTriangleForPoint(convert<PointType, point>(snapped), idx, bary);

  Eigen::Vector3d vectorEigen = convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(
    vector);
  Eigen::Vector3d projectedVector = this->ProjectVectorToFace(GetFaceNormal(faceIndex),
                                                              vectorEigen);

  Eigen::Vector3d snappedPoint = convert<PointType, Eigen::Vector3d>(snapped);
  Eigen::Vector3d newPoint = GeodesicWalkOnFace(snappedPoint, projectedVector, faceIndex);

  PointType newPointpt;
  newPointpt[0] = newPoint[0];
  newPointpt[1] = newPoint[1];
  newPointpt[2] = newPoint[2];
  return newPointpt;
}

vnl_vector_fixed<double, DIMENSION>
TriMeshWrapper::ProjectVectorToSurfaceTangent(const PointType& pointa, int idx,
                                              vnl_vector_fixed<double, DIMENSION>& vector) const
{
  vec3 bary;
  int faceIndex = GetTriangleForPoint(convert<const PointType, point>(pointa), idx, bary);
  const Eigen::Vector3d normal = GetFaceNormal(faceIndex);
  Eigen::Vector3d result = ProjectVectorToFace(normal,
                                               convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(
                                                 vector));
  vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
  return resultvnl;
}

Eigen::Vector3d TriMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d& normal,
                                                    const Eigen::Vector3d& vector) const
{
  return vector - normal * normal.dot(vector);
}

Eigen::Vector3d TriMeshWrapper::RotateVectorToFace(const Eigen::Vector3d& prevnormal,
                                                   const Eigen::Vector3d& nextnormal,
                                                   const Eigen::Vector3d& vector) const
{
  float dotprod = prevnormal.normalized().dot(nextnormal.normalized());
  if (dotprod >= 1) {
    return vector;
  }
  float angle = acos(dotprod);
  Eigen::Vector3d rotationAxis = prevnormal.normalized().cross(
    nextnormal.normalized()).normalized();
  Eigen::AngleAxisd transform(angle, rotationAxis);
  Eigen::Vector3d rotated = transform * vector;
  return rotated;
}

vnl_vector_fixed<float, DIMENSION> TriMeshWrapper::SampleNormalAtPoint(PointType p, int idx) const
{
  point pointa = convert<PointType, point>(p);
  vec3 bary;
  int face = GetTriangleForPoint(pointa, idx, bary);

  vnl_vector_fixed<float, DIMENSION> weightedNormal(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    vnl_vector_fixed<float, DIMENSION> normal = convert<vec3, vnl_vector_fixed<float, DIMENSION>>(
      mesh_->normals[mesh_->faces[face][i]]);
    weightedNormal += normal.normalize() * bary[i];
  }
  return weightedNormal;
}

TriMeshWrapper::HessianType TriMeshWrapper::SampleGradNAtPoint(PointType p, int idx) const
{
  point pointa = convert<PointType, point>(p);
  vec3 bary;
  const int face = GetTriangleForPoint(pointa, idx, bary);

  HessianType weighted_grad_normal = HessianType(0.0);
  for (int i = 0; i < 3; i++) {
    HessianType grad_normal = grad_normals_[mesh_->faces[face][i]];
    weighted_grad_normal += grad_normal * bary[i];
  }
  return weighted_grad_normal;
}

int TriMeshWrapper::GetNearestVertex(point pt) const
{
  const float* match = kd_tree_->closest_to_pt(pt, 99999999);
  int match_ind = (const point*) match - &(mesh_->vertices[0]);
  return match_ind;
}

std::vector<int> TriMeshWrapper::GetKNearestVertices(point pt, int k) const
{
  std::vector<const float*> knn;
  kd_tree_->find_k_closest_to_pt(knn, k, pt, 9999999);
  std::vector<int> vertices;
  for (int i = 0; i < knn.size(); i++) {
    int match_ind = (const point*) knn[i] - &(mesh_->vertices[0]);
    vertices.push_back(match_ind);
  }
  return vertices;
}

vec normalizeBary(const vec& bary)
{
  float sum = abs(bary[0]) + abs(bary[1]) + abs(bary[2]);
  return vec(bary / sum);
}

inline bool TriMeshWrapper::IsBarycentricCoordinateValid(const trimesh::vec3& bary) {
  return ((bary[0] >= -epsilon) && (bary[0] <= 1 + epsilon)) &&
         ((bary[1] >= -epsilon) && (bary[1] <= 1 + epsilon)) &&
         ((bary[2] >= -epsilon) && (bary[2] <= 1 + epsilon));
}

// Checks all of the neighbor faces of the 10 nearest vertices to pt.
// Returns index of the first face that has valid barycentric coordinates, and its
// barycentric coordinates in baryOut
int TriMeshWrapper::GetTriangleForPoint(point pt, int idx, vec& baryOut) const
{
  // given a guess, just check whether it is still valid.
  if(idx != -1) {
    // ensure that the cache has enough elements. this will never be resized to more than the number of particles,
    if(idx >= particle2tri_.size()) {
      particle2tri_.resize(idx + 1, 0);
    }

    const int guess = particle2tri_[idx];
    baryOut = this->ComputeBarycentricCoordinates(pt, guess);
    const vec norBary = normalizeBary(baryOut);
    if(IsBarycentricCoordinateValid(norBary)) {
      return guess;
    }
  }

  double closestDistance = 99999999;
  int closestFace = -1;

  std::vector<int> vertices = GetKNearestVertices(pt, 10);
  std::set<int> faceCandidatesSet;
  for (int j = 0; j < vertices.size(); j++) {
    int vert = vertices[j];
    for (int i = 0; i < mesh_->adjacentfaces[vert].size(); i++) {
      int face = mesh_->adjacentfaces[vert][i];

      // Only check each face once
      if (faceCandidatesSet.find(face) == faceCandidatesSet.end()) {
        faceCandidatesSet.insert(face);
        baryOut = this->ComputeBarycentricCoordinates(pt, face);
        const vec norBary = normalizeBary(baryOut);
        if (IsBarycentricCoordinateValid(norBary)) {
          if(idx != -1) {
            // update cache
            particle2tri_[idx] = face;
          }
          return face;
        }
        else {
          float distance = 0;
          for (int k = 0; k < 3; k++) {
            if (norBary[k] < 0) {
              distance += -norBary[k];
            }
            else if (norBary[k] > 1) {
              distance += norBary[k] - 1;
            }
          }
          if (distance < closestDistance) {
            closestFace = face;
            closestDistance = distance;
          }
        }
      }
    }
  }
  baryOut = this->ComputeBarycentricCoordinates(pt, closestFace);
  if(idx != -1) {
    // update cache
    particle2tri_[idx] = closestFace;
  }
  return closestFace;
}

vec3 TriMeshWrapper::ComputeBarycentricCoordinates(point pt, int face) const
{
  vec3 bCoords;
  bCoords.clear();
  point a, b, c;
  a = mesh_->vertices[mesh_->faces[face][0]];
  b = mesh_->vertices[mesh_->faces[face][1]];
  c = mesh_->vertices[mesh_->faces[face][2]];

  point n = (b - a) CROSS (c - a);
  normalize(n);

  float area = ((b - a) CROSS (c - a)) DOT n;
  float inv_area = 1.0f / (area + epsilon);
  bCoords[0] = (((c - b) CROSS (pt - b)) DOT n) * inv_area;
  bCoords[1] = (((a - c) CROSS (pt - c)) DOT n) * inv_area;
  bCoords[2] = (((b - a) CROSS (pt - a)) DOT n) * inv_area;

  float sum = bCoords.sum();
  bCoords[0] /= sum;
  bCoords[1] /= sum;
  bCoords[2] /= sum;

  return bCoords;
}

// snaps the point to the mesh by getting the barycentric coordinates and normalizing them to sum to 1.
TriMeshWrapper::PointType TriMeshWrapper::SnapToMesh(PointType pointtype, int idx) const
{
  point pt = convert<PointType, point>(pointtype);
  vec bary;
  int face = GetTriangleForPoint(pt, idx, bary);
  for (int i = 0; i < 3; i++) {
    if (bary[i] < -epsilon) bary[i] = 0;
    else if (bary[i] > 1 + epsilon) bary[i] = 1;
  }
  float sum = abs(bary[0]) + abs(bary[1]) + abs(bary[2]);
  bary[0] /= sum;
  bary[1] /= sum;
  bary[2] /= sum;
  point snappedPoint(0, 0, 0);
  for (int i = 0; i < 3; i++) {
    snappedPoint += mesh_->vertices[mesh_->faces[face][i]] * bary[i];
  }
  return convert<point, PointType>(snappedPoint);
}

TriMeshWrapper::PointType TriMeshWrapper::GetPointOnMesh() const
{
  int faceIndex = 0;
  vec center = mesh_->centroid(faceIndex);
  return convert<vec, PointType>(center);
}

const Eigen::Vector3d TriMeshWrapper::GetFaceNormal(int face_index) const
{
  vec n = mesh_->trinorm(face_index);
  Eigen::Vector3d normal = convert<vec, Eigen::Vector3d>(n);
  return normal.normalized();
}

void TriMeshWrapper::ComputeMeshBounds()
{
  mesh_lower_bound_ = GetPointOnMesh();
  mesh_upper_bound_ = GetPointOnMesh();
  for (auto& vertex : mesh_->vertices) {
    for (int dimension = 0; dimension < 3; dimension++) {
      mesh_lower_bound_[dimension] = std::min<double>(vertex[dimension],
                                                      mesh_lower_bound_[dimension]);
      mesh_upper_bound_[dimension] = std::max<double>(vertex[dimension],
                                                      mesh_upper_bound_[dimension]);
    }
  }
  double buffer = 1;
  for (int i = 0; i < 3; i++) {
    mesh_lower_bound_[i] = mesh_lower_bound_[i] - buffer;
    mesh_upper_bound_[i] = mesh_upper_bound_[i] + buffer;
  }
  if (TriMesh::verbose) {
    std::cerr << "Mesh bounds: " << PrintValue<PointType>(mesh_lower_bound_) << " -> "
              << PrintValue<PointType>(mesh_upper_bound_) << "\n";
  }
}

void TriMeshWrapper::ComputeGradN()
{
  const int n_verts = mesh_->vertices.size();
  const int n_faces = mesh_->faces.size();

  // Copy from trimesh to libigl data structures
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  GetIGLMesh(V, F);

  // Compute normals
  Eigen::MatrixXd N;
  igl::per_vertex_normals(V, F, N);

  // Compute gradient operator
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);

  // Compute gradient of normals per face
  const Eigen::MatrixXd GN = Eigen::Map<const Eigen::MatrixXd>((G*N).eval().data(), n_faces, 9);

  // Convert per-face values to per-vertex using face area as weight
  Eigen::MatrixXd GN_pervertex(n_verts, 9); GN_pervertex.setZero();
  Eigen::MatrixXd A_perface; igl::doublearea(V,F,A_perface);
  Eigen::VectorXd A_pervertex(n_verts); A_pervertex.setZero();
  // scatter the per-face values
  for(int i=0; i<n_faces; i++) {
    for(int j=0; j<3; j++) {
      GN_pervertex.row(F(i,j)) += A_perface(i, 0)*GN.row(i);
      A_pervertex(F(i, j)) += A_perface(i, 0);
    }
  }
  for(int i=0; i<n_verts; i++) {
    if(A_pervertex(i) != 0.0) {
      GN_pervertex.row(i) /= A_pervertex(i);
    }
  }

  // Copy back to VNL data structure
  grad_normals_.resize(n_verts);
  for(int j=0; j<n_verts; j++) {
    for(int i=0; i<3; i++) {
      grad_normals_[j].set(i, 0, GN_pervertex(j, i*3+0));
      grad_normals_[j].set(i, 1, GN_pervertex(j, i*3+1));
      grad_normals_[j].set(i, 2, GN_pervertex(j, i*3+2));
    }
  }

}

void TriMeshWrapper::GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
  const int n_verts = mesh_->vertices.size();
  const int n_faces = mesh_->faces.size();

  V.resize(n_verts, 3);
  F.resize(n_faces, 3);
  for(int i=0; i<n_verts; i++) {
    V(i, 0) = mesh_->vertices[i][0];
    V(i, 1) = mesh_->vertices[i][1];
    V(i, 2) = mesh_->vertices[i][2];
  }
  for(int i=0; i<n_faces; i++) {
    F(i, 0) = mesh_->faces[i][0];
    F(i, 1) = mesh_->faces[i][1];
    F(i, 2) = mesh_->faces[i][2];
  }
}

}
