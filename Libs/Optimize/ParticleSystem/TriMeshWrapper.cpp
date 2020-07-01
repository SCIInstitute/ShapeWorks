
#include "TriMeshWrapper.h"

#include <set>

using namespace trimesh;

namespace shapeworks
{
  namespace {
    static float epsilon = 1e-6;

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
    //point a = convert<PointType, point>(pointa);
    //point b = convert<PointType, point>(pointb);
    //return mesh->GetGeodesicDistance(a, b);
    return 1;
  }

  /** start in barycentric coords of currentFace
      end in barycentric coords of currentFace
      edge is the edge we are intersecting with.
  */
  point TriMeshWrapper::GetBarycentricIntersection(vec3 start, vec3 end, int currentFace, int edge) const {
    vec3 delta = end - start;
    double ratio = -start[edge] / delta[edge];
    vec3 intersect = start + delta * vec3(ratio, ratio, ratio);

    point inter(0, 0, 0);
    for (int q = 0; q < 3; q++) {
      inter += mesh->vertices[mesh->faces[currentFace][q]] * intersect[q];
    }
    return inter;
  }

  Eigen::Vector3d TriMeshWrapper::GeodesicWalkOnFace(Eigen::Vector3d pointa__, Eigen::Vector3d projectedVector__, int faceIndex__, int prevFace__) const {

    int currentFace = faceIndex__;
    Eigen::Vector3d currentPoint = pointa__;
    Eigen::Vector3d remainingVector = projectedVector__;
    float epsilon = 0.0001;
    double epsilon2 = 0.0001;
    std::vector<int> faces;
    while (remainingVector.norm() > epsilon) {
      if (currentFace == -1) {
        break;
      }
      faces.push_back(currentFace);
      vec3 currentBary(1, 0, 0);
      vec3 targetBary(1, 0, 0);
      //vec3 currentBary = mesh->ComputeBarycentricCoordinates(point(currentPoint[0], currentPoint[1], currentPoint[2]), mesh->faces[currentFace]);
      Eigen::Vector3d targetPoint = currentPoint + remainingVector;
      //vec3 targetBary = mesh->ComputeBarycentricCoordinates(point(targetPoint[0], targetPoint[1], targetPoint[2]), mesh->faces[currentFace]);

      if (faces.size() > 10) {
        for (int i = 0; i < faces.size(); i++) {
          std::cerr << faces[i] << ": " << PrintValue<TriMesh::Face>(mesh->faces[faces[i]]) << ", ";
          //std::cerr << PrintValue<vec>(mesh->GetFaceNormal(faces[i])) << std::endl;
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
      if (targetBary[0] + epsilon2 >= 0 && targetBary[1] + epsilon2 >= 0 && targetBary[2] + epsilon2 >= 0 && targetBary[0] - epsilon2 <= 1 && targetBary[1] - epsilon2 <= 1 && targetBary[2] - epsilon2 <= 1) {
        currentPoint = targetPoint;
        break;
      }
      int numNegative = 0;
      for (int i = 0; i < 3; i++) {
        if (targetBary[i] < 0) {
          numNegative++;
        }
      }
      // Number of negative Bary coords indicates if we went over edge or corner
      if (numNegative == 1) {
        // Figure out which edge we are going over
        int negativeEdge = -1;
        for (int i = 0; i < 3; i++) {
          if (targetBary[i] < 0) {
            negativeEdge = i;
            break;
          }
        }
        Eigen::Vector3d intersect = convert<point, Eigen::Vector3d>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge));
        Eigen::Vector3d remaining = targetPoint - intersect;

        int nextFace = mesh->across_edge[currentFace][negativeEdge];
        if (currentFace == -1) {
          break;
        }
        remainingVector = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(nextFace), remaining);
        currentPoint = intersect;
        currentFace = nextFace;
        continue;
      }
      else if (numNegative == 2) {
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
        if (negativeVertices.size() != 2 || positiveVertex == -1) {
          std::cerr << "ERROR ERROR incorrect number of negative vertices or positiveVertex is still -1!\n";
        }
        Eigen::Vector3d intersect0 = convert<point, Eigen::Vector3d>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeVertices[1]));
        Eigen::Vector3d intersect1 = convert<point, Eigen::Vector3d>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeVertices[0]));

        int first = -1;
        Eigen::Vector3d remaining;
        Eigen::Vector3d firstIntersect;
        double length0 = (intersect0 - currentPoint).norm();
        double length1 = (intersect1 - currentPoint).norm();

        if (length0 <= length1) {
          remaining = targetPoint - intersect0;
          firstIntersect = intersect0;
        }
        else {
          remaining = targetPoint - intersect1;
          firstIntersect = intersect1;
        }


        //if (false  && abs(length1 - length0) < 0.00000000001) {
        //  int crossingVertex = mesh->faces[currentFace][positiveVertex];
        //  // Check target pos against all neighbors of the vertex.
        //  // Pick the one with 2 positive coords (not including the crossing vertex)
        //  std::vector<int> neighbors = mesh->adjacentfaces[mesh->faces[currentFace][positiveVertex]];
        //  int numNotBad = 0;
        //  int notBadNeighbor = -1;
        //  for (int i = 0; i < neighbors.size(); i++) {

        //    Eigen::Vector3f rotated = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(neighbors[i]), remaining);
        //    Eigen::Vector3f potentialPoint = firstIntersect + rotated;
        //    vec3 potentialBary = mesh->ComputeBarycentricCoordinates(point(potentialPoint[0], potentialPoint[1], potentialPoint[2]), mesh->faces[neighbors[i]]);
        //    bool bad = false;
        //    for (int j = 0; j < 3; j++) {
        //      if (mesh->faces[neighbors[i]][j] != crossingVertex) {
        //        // must be pos
        //        if (potentialBary[j] < 0) {
        //          bad = true;
        //          break;
        //        }
        //      }
        //    }
        //    //std::cerr << "Neighbor " << neighbors[i] << ": " << PrintValue<vec3>(potentialBary) << ", BAD=" << bad << std::endl;
        //    if (!bad) {
        //      notBadNeighbor = neighbors[i];
        //      numNotBad++;
        //    }
        //  }
        //  if (numNotBad == 1) {
        //    int nextFace = notBadNeighbor;
        //    remainingVector = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(nextFace), remaining);
        //    currentPoint = firstIntersect;
        //    currentFace = nextFace;
        //    continue;
        //  }
        //  std::cerr << "ERRORERROR numNotBad = " << numNotBad << " crossingVertex: " << crossingVertex  << "currentface: " << currentFace << std::endl;
        //  for (int i = 0; i < neighbors.size(); i++) {

        //    Eigen::Vector3f rotated = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(neighbors[i]), remaining);
        //    Eigen::Vector3f potentialPoint = firstIntersect + rotated;
        //    vec3 potentialBary = mesh->ComputeBarycentricCoordinates(point(potentialPoint[0], potentialPoint[1], potentialPoint[2]), mesh->faces[neighbors[i]]);
        //    bool bad = false;
        //    for (int j = 0; j < 3; j++) {
        //      if (mesh->faces[neighbors[i]][j] != crossingVertex) {
        //        // must be pos
        //        if (potentialBary[j] < 0) {
        //          bad = true;
        //          break;
        //        }
        //      }
        //    }
        //    std::cerr << "Neighbor " << neighbors[i] << ": " << PrintValue<vec3>(potentialBary) << ", BAD=" << bad << " vertices: " << PrintValue<TriMesh::Face>(mesh->faces[neighbors[i]]) << std::endl;
        //    if (!bad) {
        //      notBadNeighbor = neighbors[i];
        //      numNotBad++;
        //    }
        //  }
        //}
        int nextFace = (length0 <= length1) ? mesh->across_edge[currentFace][negativeVertices[1]] : mesh->across_edge[currentFace][negativeVertices[0]];
        if (currentFace == -1) {
          break;
        }
        remainingVector = RotateVectorToFace(GetFaceNormal(currentFace), GetFaceNormal(nextFace), remaining);
        currentPoint = firstIntersect;
        currentFace = nextFace;
        continue;
      }
      else {
        std::cerr << "ERROR ERROR numNegative was " << numNegative << "!!!! Cancelling Geodesic Walk\n";
        break;
      }
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
    int faceIndex = GetTriangleForPoint(convert<const PointType, point>(pointa));
    const Eigen::Vector3d normal = GetFaceNormal(faceIndex);
    Eigen::Vector3d result = ProjectVectorToFace(normal, convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(vector));
    vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
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
    std::cerr << "ERROR ERROR reached end of GetTriangleForPoint!\n";

    vec bary = this->ComputeBarycentricCoordinates(pt, closestFace);
    std::cerr << "bary: " << PrintValue<vec>(bary) << "\n";
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
  }

  TriMeshWrapper::PointType TriMeshWrapper::GetPointOnMesh() const {
    int faceIndex = 0;
    //for (int i = 0; i < mesh->faces.size(); i++) {
    //  bool isEdge = false;
    //  for (int j = 0; j < 3; j++) {
    //    if (mesh->across_edge[i][j] == -1) {
    //      isEdge = true;
    //      break;
    //    }
    //  }
    //  if (!isEdge) {
    //    faceIndex = i;
    //  }
    //}
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
    double buffer = .1;
    for (int i = 0; i < 3; i++) {
      meshLowerBound[i] = meshLowerBound[i] - buffer;
      meshUpperBound[i] = meshUpperBound[i] + buffer;
    }
    std::cerr << "Mesh bounds: " << PrintValue<PointType>(meshLowerBound) << " -> " << PrintValue<PointType>(meshUpperBound) << "\n";
  }
}
