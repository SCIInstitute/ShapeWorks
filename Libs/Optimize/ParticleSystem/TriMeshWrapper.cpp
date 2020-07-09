
#include "TriMeshWrapper.h"

#include <set>
#include <random>

using namespace trimesh;

namespace shapeworks
{
  namespace {
    static float epsilon = 1e-6;
    static std::mt19937 m_rand{42};

    template<class T>
    inline std::string PrintValue(T value) {
      return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " + std::to_string(value[2]) + ")";
    }

    inline void PrintTriangle(TriMesh *mesh, int face) {
      for (int i = 0; i < 3; i++) {
        fprintf(stderr, "v[%d]=%s\n", i, PrintValue<point>(mesh->vertices[mesh->faces[face][i]]).c_str());
      }
    }

    template<class FROM, class TO>
    inline TO convert(FROM &value) {
      TO converted;
      converted[0] = value[0];
      converted[1] = value[1];
      converted[2] = value[2];
      return converted;
    }
  }

  double TriMeshWrapper::ComputeDistance(PointType pointa, PointType pointb) const {

    double eucDistance = pointa.EuclideanDistanceTo(pointb);

    //point pta = convert<PointType, point>(pointa);
    //int facea = GetTriangleForPoint(pta);
    //vec barya = ComputeBarycentricCoordinates(pta, facea);

    //point ptb = convert<PointType, point>(pointb);
    //int faceb = GetTriangleForPoint(ptb);
    //vec baryb = ComputeBarycentricCoordinates(ptb, faceb);

    //double geoDistance = cgal->ComputeDistance(facea, faceb, barya, baryb);
    //std::cerr << "Computing distance from " << PrintValue<PointType>(pointa) << " to " << PrintValue<PointType>(pointb) << "\n";
    //std::cerr << "Euclidean Distance = " << eucDistance << ", geoDistance = " << geoDistance << "\n";

    double distance = eucDistance;
    return distance;
  }

  /** start in barycentric coords of currentFace
      end in barycentric coords of currentFace
      edge is the edge we are intersecting with.
  */
  point TriMeshWrapper::GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const {
    vec3 delta = end - start;
    if (delta[edge] == 0) {
      // If going parallel to the edge, it is allowed to go all the way to the end where it wants to go
      return end;
    }
    double ratio = -start[edge] / delta[edge];
    vec3 intersect = start + delta * vec3(ratio, ratio, ratio);

    point inter(0, 0, 0);
    for (int q = 0; q < 3; q++) {
      inter += mesh->vertices[mesh->faces[currentFace][q]] * intersect[q];
    }
    return inter;
  }

  int SlideAlongEdge(Eigen::Vector3d &point_, Eigen::Vector3d &remainingVector_, int face_, int edge_, TriMesh *mesh) {
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

  Eigen::Vector3d TriMeshWrapper::GeodesicWalkOnFace(Eigen::Vector3d pointa__, Eigen::Vector3d projectedVector__, int faceIndex__, int prevFace__) const {

    int currentFace = faceIndex__;
    Eigen::Vector3d currentPoint = pointa__;
    Eigen::Vector3d remainingVector = projectedVector__;
    double minimumUpdate = 0.0000000001;
    double barycentricEpsilon = 0.0001;
    std::vector<int> facesTraversed;
    while (remainingVector.norm() > minimumUpdate && currentFace != -1) {
      facesTraversed.push_back(currentFace);
      vec3 currentBary = ComputeBarycentricCoordinates(point(currentPoint[0], currentPoint[1], currentPoint[2]), currentFace);
      Eigen::Vector3d targetPoint = currentPoint + remainingVector;
      vec3 targetBary = ComputeBarycentricCoordinates(point(targetPoint[0], targetPoint[1], targetPoint[2]), currentFace);

      if (facesTraversed.size() >= 3 && facesTraversed[facesTraversed.size() - 1] == facesTraversed[facesTraversed.size() - 3]) {
        // When at the intersection of two faces while also being at the edge of the mesh, the edge-sliding will keep alternating 
        // between the two faces without actually going anywhere since it is at a corner in the mesh.
        std::cerr << "exiting due to face repetition\n";
        break;
      }
      if (facesTraversed.size() > 10) {
        for (int i = 0; i < facesTraversed.size(); i++) {
          std::cerr << facesTraversed[i] << ": " << PrintValue<TriMesh::Face>(mesh->faces[facesTraversed[i]]) << ", ";
        }
        std::cerr << "Current point: " << PrintValue<Eigen::Vector3d>(currentPoint) << "\n";
        std::cerr << "remaining vector: " << PrintValue<Eigen::Vector3d>(remainingVector) << "\n";
        std::cerr << "currentBary: " << PrintValue<vec3>(currentBary) << "\n";
        std::cerr << "targetPoint: " << PrintValue<Eigen::Vector3d>(targetPoint) << "\n";
        std::cerr << "targetBary: " << PrintValue<vec3>(targetBary) << "\n";
        std::cerr << std::endl;
        break;
      }

      // TODO, use a small epsilon to soften the requirements (in trimesh function ComputeBarycentricCoordinates)
      if (targetBary[0] + barycentricEpsilon >= 0 && targetBary[1] + barycentricEpsilon >= 0 && targetBary[2] + barycentricEpsilon >= 0 && targetBary[0] - barycentricEpsilon <= 1 && targetBary[1] - barycentricEpsilon <= 1 && targetBary[2] - barycentricEpsilon <= 1) {
        currentPoint = targetPoint;
        //std::cerr << "same triangle " << PrintValue<Eigen::Vector3d>(currentPoint) << ")\n";
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
      Eigen::Vector3d intersect = convert<point, Eigen::Vector3d>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge));

      if (negativeVertices.size() == 2) {
        int negativeEdge1 = negativeVertices[1];
        Eigen::Vector3d intersect1 = convert<point, Eigen::Vector3d>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge1));

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
      int nextFace = mesh->across_edge[currentFace][negativeEdge];
      if (nextFace == -1) {
        nextFace = SlideAlongEdge(intersect, remaining, currentFace, negativeEdge, mesh);
      }
      if ((intersect - currentPoint).norm() > remainingVector.norm()) {
        std::cerr << "ASDFASDF moved further than the vector\n";
      }
      remainingVector = remaining;
      if (nextFace != -1) {
        remainingVector = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(nextFace), remainingVector);
      }
      currentPoint = intersect;
      currentFace = nextFace;
    }
    return currentPoint;
  }

  TriMeshWrapper::PointType TriMeshWrapper::GeodesicWalk(PointType pointa, vnl_vector_fixed<double, DIMENSION> vector) const {

    PointType snapped = this->SnapToMesh(pointa);
    int faceIndex = GetTriangleForPoint(convert<PointType, point>(snapped));

    Eigen::Vector3d vectorEigen = convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(vector);
    Eigen::Vector3d projectedVector = this->ProjectVectorToFace(GetFaceNormal(faceIndex), vectorEigen);

    Eigen::Vector3d snappedPoint = convert<PointType, Eigen::Vector3d>(snapped);
    Eigen::Vector3d newPoint = GeodesicWalkOnFace(snappedPoint, projectedVector, faceIndex, -1);

    PointType newPointpt;
    newPointpt[0] = newPoint[0]; newPointpt[1] = newPoint[1]; newPointpt[2] = newPoint[2];
    return newPointpt;
  }

  vnl_vector_fixed<double, DIMENSION> TriMeshWrapper::ProjectVectorToSurfaceTangent(const PointType &pointa, vnl_vector_fixed<double, DIMENSION> &vector) const {
    //std::cerr << "Projecting vector " << PrintValue< vnl_vector_fixed<double, DIMENSION>>(vector) << " at point " << PrintValue<PointType>(pointa) << "\n";
    int faceIndex = GetTriangleForPoint(convert<const PointType, point>(pointa));
    const Eigen::Vector3d normal = GetFaceNormal(faceIndex);
    Eigen::Vector3d result = ProjectVectorToFace(normal, convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(vector));
    vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
    //std::cerr << "normal: " << PrintValue<Eigen::Vector3d>(normal) << ", projected vector: " << PrintValue<Eigen::Vector3d>(result) << "\n";
    return resultvnl;
  }

  Eigen::Vector3d TriMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d &normal, const Eigen::Vector3d &vector) const {
    return vector - normal * normal.dot(vector);
  }

  Eigen::Vector3d TriMeshWrapper::RotateVectorToFace(const Eigen::Vector3d &prevnormal, const Eigen::Vector3d &nextnormal, const Eigen::Vector3d &vector) const {
    float dotprod = prevnormal.normalized().dot(nextnormal.normalized());
    if (dotprod >= 1) {
      return vector;
    }
    float angle = acos(dotprod);
    Eigen::Vector3d rotationAxis = prevnormal.normalized().cross(nextnormal.normalized()).normalized();
    Eigen::AngleAxisd transform(angle, rotationAxis);
    Eigen::Vector3d rotated = transform * vector;
    return rotated;
  }

  vnl_vector_fixed<float, DIMENSION> TriMeshWrapper::SampleNormalAtPoint(PointType p) const {
    int face = GetTriangleForPoint(convert<PointType, point>(p));
    const Eigen::Vector3d vecnormal = GetFaceNormal(face);
    vnl_vector_fixed<float, DIMENSION> normal(vecnormal[0], vecnormal[1], vecnormal[2]);
    return normal;
  }

  int TriMeshWrapper::GetNearestVertex(point pt) const {
    const float * match = kdTree->closest_to_pt(pt, 99999999);
    int match_ind = ( const point *) match - &(mesh->vertices[0]);
    //point nearest = mesh->vertices[match_ind];
    //std::cerr << "Nearest vs query " << PrintValue<point>(nearest) << ", " << PrintValue<point>(pt) << "\n";
    return match_ind;
  }
  std::vector<int> TriMeshWrapper::GetKNearestVertices(point pt, int k) const {
    std::vector<const float *> knn;
    kdTree->find_k_closest_to_pt(knn, k, pt, 9999999);
    std::vector<int> vertices;
    for (int i = 0; i < knn.size(); i++) {
      int match_ind = ( const point *) knn[i] - &(mesh->vertices[0]);
      vertices.push_back(match_ind);
    }
    return vertices;
  }

  vec normalizeBary(const vec &bary) {
    float sum =abs(bary[0]) + abs(bary[1]) + abs(bary[2]);
    return vec(bary / sum);
    //bary[0] /= sum; bary[1] /= sum; bary[2] /= sum;
  }

  int TriMeshWrapper::GetTriangleForPoint(point pt) const {

    double closestDistance = 99999999;
    int closestFace = -1;

    std::vector<int> vertices = GetKNearestVertices(pt, 10);
    std::set<int> faceCandidatesSet;
    for (int j = 0; j < vertices.size(); j++) {
      int vert = vertices[j];
      for (int i = 0; i < mesh->adjacentfaces[vert].size(); i++) {
        int face = mesh->adjacentfaces[vert][i];

        // Only check each face once
        if (faceCandidatesSet.find(face) == faceCandidatesSet.end()) {
          faceCandidatesSet.insert(face);
          vec bary = this->ComputeBarycentricCoordinates(pt, face);
          bary = normalizeBary(bary);
          if (((bary[0] >= -epsilon) && (bary[0] <= 1 + epsilon)) &&
            ((bary[1] >= -epsilon) && (bary[1] <= 1 + epsilon)) &&
              ((bary[2] >= -epsilon) && (bary[2] <= 1 + epsilon))) {
            return face;
          }
          else {
            float distance = 0;
            for (int j = 0; j < 3; j++) {
              if (bary[j] < 0) {
                distance += -bary[j];
              }
              else if (bary[j] > 1) {
                distance += bary[j] - 1;
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
    //double closestDistance = 99999999;
    //int closestFace = -1;

    //int vertex = GetNearestVertex(pt);
    //for (int i = 0; i < mesh->adjacentfaces[vertex].size(); i++) {
    //  int face = mesh->adjacentfaces[vertex][i];
    //  vec bary = this->ComputeBarycentricCoordinates(pt, face);
    //  if (((bary[0] >= 0) && (bary[0] <= 1)) &&
    //    ((bary[1] >= 0) && (bary[1] <= 1)) &&
    //      ((bary[2] >= 0) && (bary[2] <= 1))) {
    //    return face;
    //  }
    //  else {
    //    float distance = 0;
    //    for (int j = 0; j < 3; j++) {
    //      if (bary[j] < 0) {
    //        distance += -bary[j];
    //      }
    //      else if (bary[j] > 1) {
    //        distance += bary[j] - 1;
    //      }
    //    }
    //    if (distance < closestDistance) {
    //      closestFace = face;
    //      closestDistance = distance;
    //    }
    //  }
    //}
    //std::cerr << "ERROR ERROR reached end of GetTriangleForPoint!\n";

    vec bary = this->ComputeBarycentricCoordinates(pt, closestFace);
    //std::cerr << "bary: " << PrintValue<vec>(bary) << "\n";
    return closestFace;
  }
  vec3 TriMeshWrapper::ComputeBarycentricCoordinates(point pt, int face) const {
    vec3 bCoords; bCoords.clear();
    point a, b, c;
    a = mesh->vertices[mesh->faces[face][0]];
    b = mesh->vertices[mesh->faces[face][1]];
    c = mesh->vertices[mesh->faces[face][2]];

    point n = (b - a) CROSS(c - a);
    normalize(n);

    float area = ((b - a) CROSS(c - a)) DOT n;
    float inv_area = 1.0f / (area + epsilon);
    bCoords[0] = (((c - b) CROSS(pt - b)) DOT n) * inv_area;
    bCoords[1] = (((a - c) CROSS(pt - c)) DOT n) * inv_area;
    bCoords[2] = (((b - a) CROSS(pt - a)) DOT n) * inv_area;

    float sum = bCoords.sum();
    bCoords[0] /= sum;
    bCoords[1] /= sum;
    bCoords[2] /= sum;

    return bCoords;
  }

  TriMeshWrapper::PointType TriMeshWrapper::SnapToMesh(PointType pointtype) const {
    point pt = convert<PointType, point>(pointtype);
    int face = GetTriangleForPoint(pt);
    vec bary = ComputeBarycentricCoordinates(pt, face);
    for (int i = 0; i < 3; i++) {
      if (bary[i] < -epsilon) bary[i] = 0;
      else if (bary[i] > 1 + epsilon) bary[i] = 1;
    }
    float sum = abs(bary[0]) + abs(bary[1]) + abs(bary[2]);
    bary[0] /= sum; bary[1] /= sum; bary[2] /= sum;
    point snappedPoint(0, 0, 0);
    for (int i = 0; i < 3; i++) {
      snappedPoint += mesh->vertices[mesh->faces[face][i]] * bary[i];
    }
    return convert<point, PointType>(snappedPoint);
  }

  TriMeshWrapper::TriMeshWrapper(TriMesh *_mesh) {
    mesh = _mesh;
    mesh->need_faces();
    mesh->need_adjacentfaces();
    mesh->need_across_edge();
    ComputeMeshBounds();

    kdTree = new KDtree(mesh->vertices);

    cgal = new CGALGeodesicDistance(mesh);
  }

  TriMeshWrapper::PointType TriMeshWrapper::GetPointOnMesh() const {
    int faceIndex = m_rand() % mesh->faces.size();
    vec center = mesh->centroid(faceIndex);
    vec bary = ComputeBarycentricCoordinates(center, faceIndex);
    //std::cerr << "Getting point on mesh: " << PrintValue<vec>(center) << "bary: " << PrintValue<vec>(bary) << "\n";
    return convert<vec, PointType>(center);
  }

  const Eigen::Vector3d TriMeshWrapper::GetFaceNormal(int faceIndex) const {
    vec n = mesh->trinorm(faceIndex);
    Eigen::Vector3d normal = convert<vec, Eigen::Vector3d>(n);
    return normal.normalized();
  }

  void TriMeshWrapper::ComputeMeshBounds() {
    meshLowerBound = GetPointOnMesh();
    meshUpperBound = GetPointOnMesh();
    for (int index = 0; index < mesh->vertices.size(); index++) {
      for (int dimension = 0; dimension < 3; dimension++) {
        meshLowerBound[dimension] = mesh->vertices[index][dimension] < meshLowerBound[dimension]
          ? mesh->vertices[index][dimension] : meshLowerBound[dimension];
        meshUpperBound[dimension] = mesh->vertices[index][dimension] > meshUpperBound[dimension]
          ? mesh->vertices[index][dimension] : meshUpperBound[dimension];
      }
    }
    double buffer = 1;
    for (int i = 0; i < 3; i++) {
      meshLowerBound[i] = meshLowerBound[i] - buffer;
      meshUpperBound[i] = meshUpperBound[i] + buffer;
    }
    std::cerr << "Mesh bounds: " << PrintValue<PointType>(meshLowerBound) << " -> " << PrintValue<PointType>(meshUpperBound) << "\n";
  }
}
