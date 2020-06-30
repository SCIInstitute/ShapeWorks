
#include "TriMeshWrapper.h"

namespace shapeworks
{
  namespace {

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
    point a = convert<PointType, point>(pointa);
    point b = convert<PointType, point>(pointb);
    return mesh->GetGeodesicDistance(a, b);
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

  Eigen::Vector3f TriMeshWrapper::GeodesicWalkOnFace(Eigen::Vector3f pointa__, Eigen::Vector3f projectedVector__, int faceIndex__, int prevFace__) const {

    int currentFace = faceIndex__;
    Eigen::Vector3f currentPoint = pointa__;
    Eigen::Vector3f remainingVector = projectedVector__;
    float epsilon = 0.00001;
    double epsilon2 = 0.00001;
    std::vector<int> faces;
    while (remainingVector.norm() > epsilon) {
      faces.push_back(currentFace);
      vec3 currentBary = mesh->ComputeBarycentricCoordinates(point(currentPoint[0], currentPoint[1], currentPoint[2]), mesh->faces[currentFace]);
      Eigen::Vector3f targetPoint = currentPoint + remainingVector;
      vec3 targetBary = mesh->ComputeBarycentricCoordinates(point(targetPoint[0], targetPoint[1], targetPoint[2]), mesh->faces[currentFace]);

      if (faces.size() > 10) {
        for (int i = 0; i < faces.size(); i++) {
          std::cerr << faces[i] << ": " << PrintValue<TriMesh::Face>(mesh->faces[faces[i]]) << ", ";
          std::cerr << PrintValue<vec>(mesh->GetFaceNormal(faces[i])) << std::endl;
        }
        std::cerr << "Current point: " << PrintValue<Eigen::Vector3f>(currentPoint) << "\n";
        std::cerr << "remaining vector: " << PrintValue<Eigen::Vector3f>(remainingVector) << "\n";
        std::cerr << "currentBary: " << PrintValue<vec3>(currentBary) << "\n";
        std::cerr << "targetPoint: " << PrintValue<Eigen::Vector3f>(targetPoint) << "\n";
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
        Eigen::Vector3f intersect = convert<point, Eigen::Vector3f>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeEdge));
        Eigen::Vector3f remaining = targetPoint - intersect;

        int nextFace = mesh->across_edge[currentFace][negativeEdge];
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
        Eigen::Vector3f intersect0 = convert<point, Eigen::Vector3f>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeVertices[1]));
        Eigen::Vector3f intersect1 = convert<point, Eigen::Vector3f>(GetBarycentricIntersection(currentBary, targetBary, currentFace, negativeVertices[0]));

        int first = -1;
        Eigen::Vector3f remaining;
        Eigen::Vector3f firstIntersect;
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

  TriMeshWrapper::PointType TriMeshWrapper::GeodesicWalk(PointType pointa, vnl_vector_fixed<float, DIMENSION> vector) const {

    PointType snapped = this->SnapToMesh(pointa);
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(convert<PointType, point>(snapped), face, a, b, c);

    Eigen::Vector3f vectorEigen = convert<vnl_vector_fixed<float, DIMENSION>, Eigen::Vector3f>(vector);
    Eigen::Vector3f projectedVector = this->ProjectVectorToFace(GetFaceNormal(faceIndex), vectorEigen);

    Eigen::Vector3f snappedPoint = convert<PointType, Eigen::Vector3f>(snapped);
    Eigen::Vector3f newPoint = GeodesicWalkOnFace(snappedPoint, projectedVector, faceIndex, -1);

    PointType newPointpt;
    newPointpt[0] = newPoint[0]; newPointpt[1] = newPoint[1]; newPointpt[2] = newPoint[2];
    return newPointpt;
  }

  vnl_vector_fixed<float, DIMENSION> TriMeshWrapper::ProjectVectorToSurfaceTangent(const PointType &pointa, vnl_vector_fixed<float, DIMENSION> &vector) const {
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(convert<const PointType, point>(pointa), face, a, b, c);
    const Eigen::Vector3f normal = GetFaceNormal(faceIndex);
    Eigen::Vector3f result = ProjectVectorToFace(normal, convert<vnl_vector_fixed<float, DIMENSION>, Eigen::Vector3f>(vector));
    vnl_vector_fixed<float, DIMENSION> resultvnl(result[0], result[1], result[2]);
    return resultvnl;
  }

  Eigen::Vector3f TriMeshWrapper::ProjectVectorToFace(const Eigen::Vector3f &normal, const Eigen::Vector3f &vector) const {
    return vector - normal * normal.dot(vector);
  }

  Eigen::Vector3f TriMeshWrapper::RotateVectorToFace(const Eigen::Vector3f &prevnormal, const Eigen::Vector3f &nextnormal, const Eigen::Vector3f &vector) const {
    float dotprod = prevnormal.normalized().dot(nextnormal.normalized());
    if (dotprod >= 1) {
      return vector;
    }
    float angle = acos(dotprod);
    Eigen::Vector3f rotationAxis = prevnormal.normalized().cross(nextnormal.normalized()).normalized();
    Eigen::AngleAxisf transform(angle, rotationAxis);
    Eigen::Vector3f rotated = transform * vector;
    return rotated;
  }

  vnl_vector_fixed<float, DIMENSION> TriMeshWrapper::SampleNormalAtPoint(PointType p) const {
    int faceID = mesh->GetClosestFaceToPoint(convert<PointType, point>(p));
    const Eigen::Vector3f vecnormal = GetFaceNormal(faceID);
    vnl_vector_fixed<float, DIMENSION> normal(vecnormal[0], vecnormal[1], vecnormal[2]);
    return normal;
  }

  TriMeshWrapper::PointType TriMeshWrapper::SnapToMesh(PointType pointa) const {
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(convert<PointType, point>(pointa), face, a, b, c);
    if (a < 0) a = 0;
    if (a > 1) a = 1;
    if (b < 0) b = 0;
    if (b > 1) b = 1;
    if (c < 0) c = 0;
    if (c > 1) c = 1;
    float sumBarycentric = a + b + c;
    a = a / sumBarycentric;
    b = b / sumBarycentric;
    c = c / sumBarycentric;
    point snappedPoint = mesh->vertices[face[0]] * a + mesh->vertices[face[1]] * b + mesh->vertices[face[2]] * c;
    return convert<point, PointType>(snappedPoint);
  }

  TriMeshWrapper::TriMeshWrapper(TriMesh *_mesh) {
    mesh = _mesh;
    mesh->need_adjacentfaces();
    mesh->need_faces();
    mesh->need_across_edge();
    fim = new meshFIM;
    fim->SetMesh(mesh);
    fim->GenerateReducedData();
    ComputeMeshBounds();
  }

  TriMeshWrapper::PointType TriMeshWrapper::GetPointOnMesh() const {
    point firstVertex = mesh->vertices[0];
    return convert<point, PointType>(firstVertex);
  }

  const Eigen::Vector3f TriMeshWrapper::GetFaceNormal(int faceIndex) const {
    vec vecNormal = mesh->GetFaceNormal(faceIndex);
    Eigen::Vector3f normal = convert<vec, Eigen::Vector3f>(vecNormal);
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
    for (int i = 0; i < 3; i++) {
      meshLowerBound[i] = meshLowerBound[i] - 0.1;
      meshUpperBound[i] = meshUpperBound[i] + 0.1;
    }
  }
}
