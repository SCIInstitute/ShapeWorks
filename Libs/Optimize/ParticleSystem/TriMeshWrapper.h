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

  inline double ComputeDistance(PointType pointa, PointType pointb) const override {
    point a = ConvertPointTypeToPoint(pointa);
    point b = ConvertPointTypeToPoint(pointb);
    return mesh->GetGeodesicDistance(a, b);
  }

  inline Eigen::Vector3f GeodesicWalkOnFace(Eigen::Vector3f pointa__, Eigen::Vector3f projectedVector__, int faceIndex__, int prevFace__) const {

    int currentFace = faceIndex__;
    Eigen::Vector3f currentPoint = pointa__;
    Eigen::Vector3f remainingVector = projectedVector__;
    float epsilon = 0.00001;
    std::vector<int> faces;
    while (remainingVector.norm() > epsilon) {
      if (faces.size() > 10) {
        for (int i = 0; i < faces.size(); i++) {
          std::cerr << faces[i] << ", ";
        }
        std::cerr << std::endl;
        break;
      }
      faces.push_back(currentFace);
      vec3 currentBary = mesh->ComputeBarycentricCoordinates(point(currentPoint[0], currentPoint[1], currentPoint[2]), mesh->faces[currentFace]);
      Eigen::Vector3f targetPoint = currentPoint + remainingVector;
      vec3 targetBary = mesh->ComputeBarycentricCoordinates(point(targetPoint[0], targetPoint[1], targetPoint[2]), mesh->faces[currentFace]);

      // TODO, use a small epsilon to soften the requirements (in trimesh function ComputeBarycentricCoordinates)
      if (targetBary[0] >= 0 && targetBary[1] >= 0 && targetBary[2] >= 0 && targetBary[0] <= 1 && targetBary[1] <= 1 && targetBary[2] <= 1) {
        currentPoint = targetPoint;
        break;
      }
      int numNegative = 0;
      for (int i = 0; i < 3; i++) {
        if (targetBary[i] < 0) {
          numNegative++;
        }
      }

      if (numNegative == 1) {
        std::vector<int> positiveVertices;
        int neg = 0;
        for (int i = 0; i < 3; i++) {
          if (targetBary[i] >= 0) {
            positiveVertices.push_back(i);
          }
        }

        if (positiveVertices.size() != 2) {
          std::cerr << "ERROR ERROR incorrect number of positive vertices!\n";
        }
        Eigen::Vector3f intersect = intersectLines(currentPoint, targetPoint, convert<point>(mesh->vertices[mesh->faces[currentFace][positiveVertices[0]]]), convert<point>(mesh->vertices[mesh->faces[currentFace][positiveVertices[1]]]));

        std::vector<int> neighbors0 = mesh->adjacentfaces[mesh->faces[currentFace][positiveVertices[0]]];
        std::vector<int> neighbors1 = mesh->adjacentfaces[mesh->faces[currentFace][positiveVertices[1]]];

        // TODO Does TriMesh have a get edge neighbor function?

        std::vector<int> intersection(2);
        std::set_intersection(&neighbors0[0], &neighbors0[neighbors0.size()], &neighbors1[0], &neighbors1[neighbors1.size()], intersection.begin());
        int nextFace = currentFace == intersection[0] ? intersection[1] : intersection[0];
        Eigen::Vector3f remaining = targetPoint - intersect;

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
        Eigen::Vector3f intersect0 = intersectLines(currentPoint, targetPoint, convert<point>(mesh->vertices[mesh->faces[currentFace][positiveVertex]]), convert<point>(mesh->vertices[mesh->faces[currentFace][negativeVertices[0]]]));
        Eigen::Vector3f intersect1 = intersectLines(currentPoint, targetPoint, convert<point>(mesh->vertices[mesh->faces[currentFace][positiveVertex]]), convert<point>(mesh->vertices[mesh->faces[currentFace][negativeVertices[1]]]));

        int first = -1;
        Eigen::Vector3f remaining;
        Eigen::Vector3f firstIntersect;
        if ((intersect0 - currentPoint).norm() < (intersect1 - currentPoint).norm()) {
          first = negativeVertices[0];
          remaining = targetPoint - intersect0;
          firstIntersect = intersect0;
        }
        else {
          first = negativeVertices[1];
          remaining = targetPoint - intersect1;
          firstIntersect = intersect1;
        }
        // TODO might need to check if its going right over the vertex here

        std::vector<int> neighbors0 = mesh->adjacentfaces[mesh->faces[currentFace][positiveVertex]];
        std::vector<int> neighbors1 = mesh->adjacentfaces[mesh->faces[currentFace][first]];

        std::vector<int> intersection(2);
        std::set_intersection(&neighbors0[0], &neighbors0[neighbors0.size()], &neighbors1[0], &neighbors1[neighbors1.size()], intersection.begin());
        int nextFace = currentFace == intersection[0] ? intersection[1] : intersection[0];

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

  inline PointType GeodesicWalk(PointType pointa, vnl_vector_fixed<float, 3> vector) const override {

    PointType snapped = this->SnapToMesh(pointa);
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(ConvertPointTypeToPoint(snapped), face, a, b, c);

    Eigen::Vector3f vectorEigen = convert<vnl_vector_fixed<float, 3>>(vector);
    Eigen::Vector3f projectedVector = this->ProjectVectorToFace(GetFaceNormal(faceIndex), vectorEigen);

    Eigen::Vector3f snappedPoint = convert<PointType>(snapped);
    Eigen::Vector3f newPoint = GeodesicWalkOnFace(snappedPoint, projectedVector, faceIndex, -1);

    PointType newPointpt;
    newPointpt[0] = newPoint[0]; newPointpt[1] = newPoint[1]; newPointpt[2] = newPoint[2];
    return newPointpt;
  }

  inline vnl_vector_fixed<float, 3> ProjectVectorToSurfaceTangent(const PointType & pointa, vnl_vector_fixed<float, 3> & vector) const override {
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(ConvertPointTypeToPoint(pointa), face, a, b, c);
    const Eigen::Vector3f normal = GetFaceNormal(faceIndex);
    Eigen::Vector3f result = ProjectVectorToFace(normal, convert<vnl_vector_fixed<float, 3>>(vector));
    vnl_vector_fixed<float, 3> resultvnl(result[0], result[1], result[2]);
    return resultvnl;
  }

  inline Eigen::Vector3f ProjectVectorToFace(const Eigen::Vector3f & normal, const Eigen::Vector3f & vector) const {
    return vector - normal * normal.dot(vector);
  }

  inline Eigen::Vector3f RotateVectorToFace(const Eigen::Vector3f & prevnormal, const Eigen::Vector3f & nextnormal, const Eigen::Vector3f & vector) const {
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

  inline vnl_vector_fixed<float, 3> SampleNormalAtPoint(PointType p) const override {
    int faceID = mesh->GetClosestFaceToPoint(ConvertPointTypeToPoint(p));
    const Eigen::Vector3f vecnormal = GetFaceNormal(faceID);
    vnl_vector_fixed<float, 3> normal(vecnormal[0], vecnormal[1], vecnormal[2]);
    return normal;
  }

  inline PointType SnapToMesh(PointType pointa) const override {
    float a, b, c;
    TriMesh::Face face;
    int faceIndex = mesh->GetTriangleInfoForPoint(ConvertPointTypeToPoint(pointa), face, a, b, c);
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
    return ConvertPointToPointType(snappedPoint);
  }

  TriMeshWrapper(TriMesh* _mesh) {
    mesh = _mesh;
    mesh->need_adjacentfaces();
    mesh->need_faces();
    // TODO issues with linking meshFIM.cpp are causing unresolved external symbol errors.
    fim = new meshFIM;
    fim->SetMesh(mesh);
    fim->GenerateReducedData();
    ComputeMeshBounds();
  }
  ~TriMeshWrapper() {}

  inline PointType GetPointOnMesh() const override {
    point firstVertex = mesh->vertices[0];
    return ConvertPointToPointType(firstVertex);
  }

  inline const PointType &GetMeshLowerBound() const override {
    return meshLowerBound;
  }
  inline const PointType &GetMeshUpperBound() const override {
    return meshUpperBound;
  }

private:

  inline Eigen::Vector3f intersectLines(Eigen::Vector3f A1, Eigen::Vector3f A2, Eigen::Vector3f B1, Eigen::Vector3f B2) const {
    //https://github.com/sheryjoe/MeshWorks/blob/master/source/optimization/findIntersection.m

    Eigen::Vector3f AcrossB = (A2 - A1).cross(B2 - B1);
    double nA = (B2 - B1).cross(A1 - B1).dot(AcrossB); //nA = dot(cross(B2-B1, A1-B1), cross(A2-A1, B2-B1));
    double nB = (A2 - A1).cross(A1 - B1).dot(AcrossB); //nB = dot(cross(A2-A1, A1-B1), cross(A2-A1, B2-B1));
    double d = AcrossB.dot(AcrossB); //d = dot(cross(A2-A1, B2-B1), cross(A2-A1, B2-B1));

    Eigen::Vector3f A0 = A1 + (nA / d) * (A2 - A1); //A0 = A1 + (nA/d)*(A2-A1);
    Eigen::Vector3f B0 = B1 + (nB / d) * (B2 - B1); //B0 = B1 + (nB/d)*(B2-B1);

    float nA1 = (A0 - A1).norm(); //nA1 = norm(A0 - A1);
    float nA2 = (A0 - A2).norm(); //nA2 = norm(A0 - A2);
    float nB1 = (B0 - B1).norm(); //nB1 = norm(B0 - B1);
    float nB2 = (B0 - B2).norm(); //nB2 = norm(B0 - B2);
    float nrmA = (A1 - A2).norm(); //nrmA = norm(A1 - A2);
    float nrmB = (B1 - B2).norm(); //nrmB = norm(B1 - B2);

    //if norm(A0-B0) < atol
    if ((A0 - B0).norm() < 0.0001) {
      // TODO add in the extra code from the git repo
      //std::cout << "Intersection! = " << PrintValue<itk::Vector<double, 3>>(A0) << std::endl;
    }
    else {
      std::cout << "Warning! intersections did not match up!\n";
    }
    return A0;
  }

  const Eigen::Vector3f GetFaceNormal(int faceIndex) const {
    vec vecNormal = mesh->GetFaceNormal(faceIndex);
    Eigen::Vector3f normal = convert<vec>(vecNormal);
    return normal.normalized();
  }

  inline void ComputeMeshBounds() {
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


  template<class T>
  inline std::string PrintValue(T value) const {
    return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " + std::to_string(value[2]) + ")";
  }

  template<class T>
  inline Eigen::Vector3f convert(T value) const {
    Eigen::Vector3f converted(value[0], value[1], value[2]);
    return converted;
  }

  template<class T>
  inline std::string PrintVector(std::vector<T> &vec) const {
    std::string str = "(";
    for (T i : vec) {
      str += std::to_string(i) + ", ";
    }
    return str + ")";
  }

  inline void PrintTriangle(int face) const {
    for (int i = 0; i < 3; i++) {
      fprintf(stderr, "v[%d]=%s\n", i, PrintValue<point>(mesh->vertices[mesh->faces[face][i]]).c_str());
    }
  }

  inline point ConvertPointTypeToPoint(const PointType &pt) const {
    point p(pt[0], pt[1], pt[2]);
    //p[0] = pt[0];
    //p[1] = pt[1];
    //p[2] = pt[2];
    return p;
  }
  inline PointType ConvertPointToPointType(point& p) const {
    PointType pt;
    pt[0] = p[0];
    pt[1] = p[1];
    pt[2] = p[2];
    return pt;
  }

  TriMesh* mesh;
  meshFIM* fim;

  PointType meshLowerBound;
  PointType meshUpperBound;
};

}
