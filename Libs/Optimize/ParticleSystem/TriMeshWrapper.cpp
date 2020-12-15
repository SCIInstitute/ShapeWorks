#include "TriMeshWrapper.h"

#include <float.h>
#include <set>

using namespace trimesh;

#define CLAMP(t)                                \
  (t < 0 ? 0 : (t>1 ? 1 : t))

namespace shapeworks {

namespace {

template<class T>
inline std::string PrintValue(T value)
{
  return "(" +
    std::to_string(value[0]) + ", " +
    std::to_string(value[1]) + ", " +
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
} // end namespace


//////////////////////////////////////////////////////////////////////
TriMeshWrapper::TriMeshWrapper(std::shared_ptr<trimesh::TriMesh> mesh)
{
  m_mesh = mesh;
  m_mesh->need_neighbors();
  m_mesh->need_faces();
  m_mesh->need_adjacentfaces();
  m_mesh->need_across_edge();
  m_mesh->need_normals();
  m_mesh->need_curvatures();
  ComputeMeshBounds();

  m_kd_tree = std::make_shared<KDtree>(m_mesh->vertices);
}


//////////////////////////////////////////////////////////////////////
// Compute the Euclidean distance between two points.
double
TriMeshWrapper::ComputeDistance(const PointType pointTypeA,
                                const PointType pointTypeB) const
{
  return pointTypeA.EuclideanDistanceTo(pointTypeB);
}

//////////////////////////////////////////////////////////////////////
vnl_vector_fixed<double, DIMENSION>
TriMeshWrapper::ProjectVectorToSurfaceTangent(const PointType& pointType,
                                              vnl_vector_fixed<double, DIMENSION>& vector) const
{
  // Get the face for this point.
  int face =
    GetTriangleForPoint(convert<const PointType, point>(pointType));

  // Get the normal for this face.
  const Eigen::Vector3d normal = GetFaceNormal(face);

  // Now project the input vector on to the face.
  Eigen::Vector3d result =
    ProjectVectorToFace(normal,
                        convert<vnl_vector_fixed<double, DIMENSION>,
                        Eigen::Vector3d>(vector));

  vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
  return resultvnl;
}

//////////////////////////////////////////////////////////////////////
vnl_vector_fixed<float, DIMENSION>
TriMeshWrapper::SampleNormalAtPoint(PointType pointType) const
{
  point pt = convert<PointType, point>(pointType);
  int face = GetTriangleForPoint(pt);

  vec3 bary = ComputeBarycentricCoordinates(pt, face);

  vnl_vector_fixed<float, DIMENSION> weightedNormal(0, 0, 0);

  for (int i = 0; i < 3; i++) {
    vnl_vector_fixed<float, DIMENSION> normal =
      convert<vec3, vnl_vector_fixed<float, DIMENSION>>(
          m_mesh->normals[m_mesh->faces[face][i]]);

    weightedNormal += normal.normalize() * bary[i];
  }

  return weightedNormal;
}


//////////////////////////////////////////////////////////////////////
// Return a point on the mesh
TriMeshWrapper::PointType
TriMeshWrapper::GetPointOnMesh() const
{
  int face = 0;
  vec center = m_mesh->centroid(face);

  return convert<vec, PointType>(center);
}


//////////////////////////////////////////////////////////////////////
// Given a 3D point return the closest point on the mesh.
TriMeshWrapper::PointType
TriMeshWrapper::ClosestPointOnMesh(const PointType pointType) const
{
  point pt = convert<const PointType, point>(pointType);

  // Get the face closest to the point.
  int face = GetTriangleForPoint(pt);

  // Get the closest point on the face.
  point closestPoint = ClosestPointOnFace(pt, face);

  return convert<point, PointType>(closestPoint);
}

//////////////////////////////////////////////////////////////////////
// Compute the bounds.
void
TriMeshWrapper::ComputeMeshBounds()
{
  m_mesh_lower_bound = GetPointOnMesh();
  m_mesh_upper_bound = GetPointOnMesh();

  // Loop through all of the vertices on the mesh.
  for (auto& vertex : m_mesh->vertices) {
    for (int dimension = 0; dimension < 3; dimension++) {
      m_mesh_lower_bound[dimension] =
        std::min<double>(vertex[dimension], m_mesh_lower_bound[dimension]);
      m_mesh_upper_bound[dimension] =
        std::max<double>(vertex[dimension], m_mesh_upper_bound[dimension]);
    }
  }

  // Add a buffer - Oleks why 1.0?? FLT_MIN would be more appropriate
  // given meshes will vary in scale.

  // Note: If FLT_MIN is used ShapeWorks blows up.
  double buffer = 1.0;

  for (int i = 0; i < 3; i++) {
    m_mesh_lower_bound[i] = m_mesh_lower_bound[i] - buffer;
    m_mesh_upper_bound[i] = m_mesh_upper_bound[i] + buffer;
  }

  if (TriMesh::verbose) {
    std::cerr << "Mesh bounds: "
              << PrintValue<PointType>(m_mesh_lower_bound) << " -> "
              << PrintValue<PointType>(m_mesh_upper_bound) << "\n";
  }
}


//////////////////////////////////////////////////////////////////////
// Take the input point and walk along the mesh using the direction vector.
TriMeshWrapper::PointType
TriMeshWrapper::GeodesicWalk(const PointType pointType,
                             const vnl_vector_fixed<double, DIMENSION>& vector) const
{
  // First get the closest point on the mesh.
  PointType closestPointType = ClosestPointOnMesh(pointType);

  // Next get the closest point's face.
  int face =
    GetTriangleForPoint(convert<PointType, point>(closestPointType));

  // Vector conversion.
  Eigen::Vector3d vectorEigen =
    convert<const vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(vector);

  // Before starting any actual walk, project the direction vector to
  // be tangent to the face because the direction vector could be
  // normal to the face, (i.e. attempting to pull the point off the
  // surface) and the walk should be along the face not off the face.

  // Project the direction vector on to the face.
  Eigen::Vector3d projectedVector =
    this->ProjectVectorToFace(GetFaceNormal(face), vectorEigen);

  // Point conversion
  Eigen::Vector3d closestPoint =
    convert<PointType, Eigen::Vector3d>(closestPointType);

  // Take the point and face and walk along the projected direction
  // vector.
  Eigen::Vector3d newPoint =
    GeodesicWalkOnFace(closestPoint, face, projectedVector);

  return convert<Eigen::Vector3d, PointType>(newPoint);
}


//////////////////////////////////////////////////////////////////////
// Take the input point and conesponding face and walk along the
// face using the projected vector.

// Computes the targetpoint = pt + projectedVector to see if the
// update goes over any edges in the face
Eigen::Vector3d
TriMeshWrapper::GeodesicWalkOnFace(const Eigen::Vector3d current,
                                   const int face,
                                   const Eigen::Vector3d projectedVector) const
{
  int currentFace = face;
  Eigen::Vector3d currentPoint = current;
  Eigen::Vector3d remainingVector = projectedVector;

  // Oleks - Justification for these values as meshes will vary in size.
  // TODO Allen recommends using FLOAT_MIN
  double minimumUpdate = FLT_MIN; // 0.0000000001;

  std::vector<int> facesTraversed;

  while (remainingVector.norm() > minimumUpdate && currentFace != -1) {

    // Keep track of the faces traversed to prevent inifinite walking.
    facesTraversed.push_back(currentFace);

    // If the point has walked from one face to another, then back,
    // stop.
    if (facesTraversed.size() >= 3 &&
        facesTraversed[facesTraversed.size() - 1] ==
        facesTraversed[facesTraversed.size() - 3]) {

      // GIANT HACK for weird behavior

      // When at the intersection of two faces while also being at the
      // edge of the mesh, the edge walking will keep alternating
      // between the two faces without actually going anywhere since
      // it is at a corner in the mesh.

      // This prevents point from getting wrapped around on a "horn"
      // because it won't go on same face potentially

      // std::cerr << "Warning: face repetition" << std::endl;
      break;
    }

    if (facesTraversed.size() > 100) {
      // ANOTHER GIANT HACK

      // If 100 faces have been traversed, which is excessive amount,
      // kill the update and dump info.

      // TODO compute the limit based on the mesh and average triangle
      // size or similar.
      for (int i = 0; i < facesTraversed.size(); i++) {
        std::cerr << facesTraversed[i] << ": "
                  << PrintValue<TriMesh::Face>(m_mesh->faces[facesTraversed[i]]) << ", ";
      }

      Eigen::Vector3d targetPoint = currentPoint + remainingVector;

      vec3 currentBary = ComputeBarycentricCoordinates(
          point(currentPoint[0], currentPoint[1], currentPoint[2]), currentFace);

      vec3 targetBary = ComputeBarycentricCoordinates(
          point(targetPoint[0], targetPoint[1], targetPoint[2]), currentFace);

      std::cerr << "Current point: " << PrintValue<Eigen::Vector3d>(currentPoint) << "\n"
                << "remaining vector: " << PrintValue<Eigen::Vector3d>(remainingVector) << "\n"
                << "currentBary: " << PrintValue<vec3>(currentBary) << "\n"
                << "targetPoint: " << PrintValue<Eigen::Vector3d>(targetPoint) << "\n"
                << "targetBary: " << PrintValue<vec3>(targetBary) << "\n"
                << std::endl;
      break;
    }

    // Get the updated target point.
    Eigen::Vector3d targetPoint = currentPoint + remainingVector;

    // Walk towards the edge. If an edge is encountered it's index is
    // returned. The currentPoint is updated.
    int edge = GeodesicWalkTowardsEdge( currentPoint, targetPoint,
                                        currentFace );

    // Finished as the target point is inside the currect face.
    if( edge == -1 ) {
      currentFace  = -1;
      remainingVector = Eigen::Vector3d(0, 0, 0);

      break;
    }
    // Reached an edge.
    else
    {
      // Update the remaining vector.
      remainingVector = targetPoint - currentPoint;

      // Look for an adjacent face.
      int nextFace = m_mesh->across_edge[currentFace][edge];

      // No adjacent face so on the edge, walk along it.
      if (nextFace == -1) {

	// Walk along the edge towards the adjacent edge. If the
	// adjacent edge is encountered the index of adjacent face is
	// returned. The currentPoint is updated.
	
	// TODO consider whether to keep bending around corner(s) of
	// faces or if that will lead to weird results.
        nextFace = GeodesicWalkOnEdge(currentPoint, currentFace, edge,
                                      remainingVector);

        // Oleks original code will walk the point in potentially the
        // wrong direction because it uses the projected vector.

        // Even on an edge it, the walk should continue towards the
        // target point. Open question???
        remainingVector = targetPoint - currentPoint;
      }

      // Rotate the updated remaining direction vector from the
      // current face to the next face.
      if (nextFace != -1) {
        remainingVector = RotateVectorToFace(GetFaceNormal(currentFace),
                                             GetFaceNormal(nextFace),
                                             remainingVector);
      }

      currentFace = nextFace;
    }
  }

  return currentPoint;
}


//////////////////////////////////////////////////////////////////////
// Given a 3D Point inside a triangle and a target point that is
// potentially outside the triangle, return the point that walks
// towards the target without going outside the triangle.
int
TriMeshWrapper::GeodesicWalkTowardsEdge(      Eigen::Vector3d & current,
                                        const Eigen::Vector3d target,
                                        const int face) const
{
  point currentPt = convert<const Eigen::Vector3d, point>(current);
  point targetPt  = convert<const Eigen::Vector3d, point>(target);

  // Get the barycentric coordinates
  vec3 current_bary = ComputeBarycentricCoordinates( currentPt, face);
  vec3 target_bary  = ComputeBarycentricCoordinates( targetPt, face);

  // Initally assume the barycentric coordinates for the target point
  // are all between zero and one (iniside the triangle).
  current = target;
  int edge = -1;

  double closestDistance = DBL_MAX;

  // If any barycentric coordinate of the target point is negative get
  // the point intersecting the edge.  If two coordinates are negative
  // only one will return an intersection with all positive
  // barycentric coordinates.
  for( int i=0; i<3; ++i )
  {
    if ( target_bary[i] < 0.0f )
    {
      vec3 bary =
        GetBarycentricIntersection(current_bary, target_bary, face, i);

      // GetBarycentricIntersection can have rounding issues when very
      // near the edge so this check does not always work.
      // Open question as to whether a tolerance (FLT_MIN) should be
      // used.
      if( 0.0f <= bary[0] && bary[0] <= 1.0 &&
          0.0f <= bary[1] && bary[1] <= 1.0 &&
          0.0f <= bary[2] && bary[2] <= 1.0 )
      {
        // Get the resulting intersection point.
        point edgePt(0, 0, 0);
        for (int j = 0; j < 3; j++) {
          edgePt += m_mesh->vertices[m_mesh->faces[face][j]] * bary[j];
        }

        current = convert< point, Eigen::Vector3d > (edgePt);
        edge = i;

        break;
      }

      // Fail safe option for the roumding issue calculate the point
      // and check the distance.

      // Get the resulting intersection point.
      point edgePt(0, 0, 0);
      for (int j = 0; j < 3; j++) {
        edgePt += m_mesh->vertices[m_mesh->faces[face][j]] * bary[j];
      }

      // Get the distance from the current point to point on the edge.
      double distance = sqrt((edgePt - currentPt) DOT (edgePt - currentPt));

      // If the distance is minimal then keep that edge.
      if (closestDistance > distance) {
        closestDistance = distance;
        current = convert< point, Eigen::Vector3d > (edgePt);
        edge = i;
      }
    }
  }

  // Return the edge that the point walked toawrds.
  return edge;
}


//////////////////////////////////////////////////////////////////////
// Given a point and its corresponding face and edge walk along the
// edge using the projected vector.
int
TriMeshWrapper::GeodesicWalkOnEdge(Eigen::Vector3d& edgePt,
                                   const int face,
                                   const int edge,
                                   Eigen::Vector3d& remainingVector) const
{
  // Get the edge and vertex indices so to get the points that define
  // the edge.

  // Edges are represented using the index of the vertex opposite it.
  // So the indices of the vertices that define the edge are the other
  // two this is trimesh's paradigm for defining edges
  int edgeIndexA = (edge + 1) % 3;
  int edgeIndexB = (edge + 2) % 3;

  int vertexIndexA = m_mesh->faces[face][edgeIndexA];
  int vertexIndexB = m_mesh->faces[face][edgeIndexB];

  Eigen::Vector3d pointA =
    convert<trimesh::point, Eigen::Vector3d>(m_mesh->vertices[vertexIndexA]);
  Eigen::Vector3d pointB =
    convert<trimesh::point, Eigen::Vector3d>(m_mesh->vertices[vertexIndexB]);

  // Get the tangent for the edge.
  Eigen::Vector3d meshEdge = pointB - pointA;
  meshEdge.normalize();

  // Calculate the distance and direction to walk along the edge.
  double dotProd = meshEdge.dot(remainingVector);
  Eigen::Vector3d projectedVector = meshEdge.normalized() * dotProd;

  // Check which endpoint the point is walking towards.
  double newDot = projectedVector.dot(meshEdge);

  int towardsEdge;
  Eigen::Vector3d maxWalk;

  // Walking in the same direction as the mesh edge which is towards
  // point B.
  if (newDot >= 0) {
    maxWalk = pointB - edgePt;
    towardsEdge = edgeIndexA;
  }
  // Walking in the opposite direction as the mesh edge which is
  // towards point A.
  else if (newDot < 0) {
    maxWalk = pointA - edgePt;
    towardsEdge = edgeIndexB;
  }

  // If the projectected vector is less than the maximum walking
  // distance, add the projection as it finished.
  if (projectedVector.norm() < maxWalk.norm()) {
    edgePt += projectedVector;
    remainingVector = Eigen::Vector3d(0, 0, 0);

    return face;
  }
  // If the projectected vector is greater than the maximum walking
  // distance, add the maximum walking distance to the point, subtract
  // that from the projectected vector and return the face so to
  // continue to the next face.
  else {
    edgePt += maxWalk;


    // Oleks original code.
    // The remaining vector being returned is problematic because the
    // original remaining vector has been projected on to edge which
    // can have significantly different direction than the original
    // direction. As such, a point could walk into the interior and
    // have an incorrect finial location.

    remainingVector = projectedVector - maxWalk;

    // The trimesh across_edge structure gives the id of the
    // neighboring face that shares the edge. It is -1 if that face
    // doesnt exist.
    return m_mesh->across_edge[face][towardsEdge];
  }
}


//////////////////////////////////////////////////////////////////////
// Project a vector to the face (plane)
Eigen::Vector3d
TriMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d& normal,
                                    const Eigen::Vector3d& vector) const
{
  return vector - normal * normal.dot(vector);
}


//////////////////////////////////////////////////////////////////////
// Rotate a vector on to the next normal that is tagent to the
// previous normal
Eigen::Vector3d
TriMeshWrapper::RotateVectorToFace(const Eigen::Vector3d& prevNormal,
                                   const Eigen::Vector3d& nextNormal,
                                   const Eigen::Vector3d& vector) const
{
  double dotprod = prevNormal.normalized().dot(nextNormal.normalized());

  if (dotprod >= 1) {
    return vector;
  }

  double angle = acos(dotprod);

  Eigen::Vector3d rotationAxis =
    prevNormal.normalized().cross(nextNormal.normalized()).normalized();

  Eigen::AngleAxisd transform(angle, rotationAxis);
  Eigen::Vector3d rotated = transform * vector;

  return rotated;
}


//////////////////////////////////////////////////////////////////////
// Returns the mesh vertex that is closest to a point.
int
TriMeshWrapper::GetNearestVertex(const point pt) const
{
  const float* match = m_kd_tree->closest_to_pt(pt, DBL_MAX);

  int match_ind = (const point*) match - &(m_mesh->vertices[0]);

  return match_ind;
}


//////////////////////////////////////////////////////////////////////
// Returns N mesh vertices that are closest to a point.
std::vector<int>
TriMeshWrapper::GetNearestVertices(const point pt,
                                   const int nVerts) const
{
  std::vector<const float*> knn;
  m_kd_tree->find_k_closest_to_pt(knn, nVerts, pt, DBL_MAX);

  std::vector<int> vertices;
  for (int i = 0; i < knn.size(); i++) {
    int match_ind = (const point*) knn[i] - &(m_mesh->vertices[0]);
    vertices.push_back(match_ind);
  }

  return vertices;
}


//////////////////////////////////////////////////////////////////////
// Projects a 3D point on to the plane defined by a face - not used.
point
TriMeshWrapper::ProjectPointOnToFace(const point pt, const int face) const
{
  point v0 = m_mesh->vertices[m_mesh->faces[face][0]];
  point v1 = m_mesh->vertices[m_mesh->faces[face][1]];
  point v2 = m_mesh->vertices[m_mesh->faces[face][2]];

  // Get the normal for the plane.
  point n = (v1 - v0) CROSS (v2 - v0);
  normalize(n);

  // Project the point on to the plane.
  point v = (pt - v0);
  double dist = v DOT n;

  point pt0 = pt - dist * n;

  return pt0;
}


//////////////////////////////////////////////////////////////////////
// Calculates the Barycentric coordinates of a 3D point as projected
// onto the plane defined by the face.
vec3
TriMeshWrapper::ComputeBarycentricCoordinates(const point pt,
                                              const int face) const
{
  point v0 = m_mesh->vertices[m_mesh->faces[face][0]];
  point v1 = m_mesh->vertices[m_mesh->faces[face][1]];
  point v2 = m_mesh->vertices[m_mesh->faces[face][2]];

  // Compute the base vectors.
  point u = (v1 - v0);
  point v = (v2 - v0);
  point w = (pt - v0);

  // Get the normal
  point n = u CROSS v;

  // Barycentric coordinates of the projection P′of P onto T:
  point g = u CROSS w;
  double gamma = (g DOT n) / (n DOT n);

  point b = w CROSS v;
  double beta  = (b DOT n) / (n DOT n);

  double alpha = 1.0 - gamma - beta;

  // Return the barycentric coordinates.
  return vec3( alpha, beta, gamma );
}


//////////////////////////////////////////////////////////////////////
// StartBary and endBary are the barycentric coordinates of the
// starting and ending points. The edge is the index of the edge that
// the line segment defined by the start and end points crosses over.

// Edges are represented using the index of the vertex opposite it.
// So the indices of the vertices that define the edge are the other
// two this is trimesh's paradigm for defining edges

// The intersection of the edge and the line segment defined by the
// start and end points occurs when the barycentric coordinate of the
// vertex corrsponding to the edge is zero.
vec3
TriMeshWrapper::GetBarycentricIntersection(const vec3 startBary,
                                           const vec3 endBary,
                                           const int face,
                                           const int edge) const
{
  vec3 bary;

  // Get a barycentric coordinates vector for the line between the
  // start and end points.
  vec3 deltaBary = endBary - startBary;

  // If going parallel to the edge, it is allowed to go all the way to
  // the end.
  if (deltaBary[edge] == 0.0) {
    bary = endBary;
  }
  else {
    // The intersection occurs when the barycentric coordinate of the
    // vertex associated with the edge is zero. To obtain this get the
    // ratio of the start to the difference vector for the associated
    // vertext.
    double ratio = startBary[edge] / deltaBary[edge];

    // Scale the difference vector and subtract that from the start
    // which gives the intersection.
    bary = startBary - deltaBary * vec3(ratio, ratio, ratio);
  }

  // Rounding off - point is not always exactly on the edge!!!!
  // if( bary[edge] != 0.0f )
  // {
  //   std::cerr << "edge " << edge << "  "
  //          << "start bary " << startBary << "  "
  //          << "end bary " << endBary << "  "
  //          << "intersection bary " << bary << "  "
  //          << std::endl;
  // }

  return bary;
}


//////////////////////////////////////////////////////////////////////
// Checks all of the neighbor faces of the 10 nearest vertices to pt.
// Returns index of the face that is the closest to the point
int
TriMeshWrapper::GetTriangleForPoint(const point pt) const
{
  double closestDistance = DBL_MAX;
  int closestFace = -1;

  // For storing the faces previously checked.
  std::set<int> faceCandidatesSet;

  // Get the 10 nearest vertices to the point and check each
  // associated face.
  std::vector<int> vertices = GetNearestVertices(pt, 10);

  for (int j = 0; j < vertices.size(); j++) {
    int vert = vertices[j];

    // Check all of the faces associated with this vertex.
    for (int i = 0; i < m_mesh->adjacentfaces[vert].size(); i++) {

      // Check each face once.
      int face = m_mesh->adjacentfaces[vert][i];

      if (faceCandidatesSet.find(face) == faceCandidatesSet.end()) {
        faceCandidatesSet.insert(face);

        point closestPoint = ClosestPointOnFace( pt, face);

        // Get the distance from the original point to point on the triangle.
        double distance = sqrt((closestPoint - pt) DOT (closestPoint - pt));

        // If the distance is minimal then keep that triangle.
        if (closestDistance > distance) {
          closestDistance = distance;
          closestFace = face;
        }
      }
    }
  }

  // std::cerr << "closestFace " << closestFace << std::endl;

  return closestFace;
}


//////////////////////////////////////////////////////////////////////
// Given a 3D Point return the point that is the closest location on
// the face (which may be an edge or vertex).
point
TriMeshWrapper::ClosestPointOnFace(const point pt, const int face) const
{
  // Get the barycentric coordinates
  vec3 bary = ComputeBarycentricCoordinates(pt, face);

  // Get the vertices that form the face.
  point vert[3];
  vert[0] = m_mesh->vertices[m_mesh->faces[face][0]];
  vert[1] = m_mesh->vertices[m_mesh->faces[face][1]];
  vert[2] = m_mesh->vertices[m_mesh->faces[face][2]];

  // If any barycentric coordinate is negative get the point on the
  // line segment which will be the closeest point. Even if two
  // barycentric coordinates are negative the clamp forces the point
  // to be at the intersection of the two line segments.
  for( int i=0; i<3; ++i )
  {
    if ( bary[i] < 0.0f )
    {
      int j = (i+1) % 3;
      int k = (i+2) % 3;

      // Projection of the point on the line defined by points j and k.
      point u = vert[k] - vert[j];
      point v = pt      - vert[j];

      double t = (v DOT u) / (u DOT u);

      // Clamp so to get a point on the line segment.
      t = CLAMP( t );

      bary[i] = 0.0f;
      bary[j] = 1.0f - t;
      bary[k] = t;

      break;
    }
  }

  // Return the closest point as projected on to the triangle.
  return (bary[0] * vert[0] + bary[1] * vert[1] + bary[2] * vert[2]);
}

//////////////////////////////////////////////////////////////////////
// Return a the face normal
const
Eigen::Vector3d TriMeshWrapper::GetFaceNormal(const int face) const
{
  vec n = m_mesh->trinorm(face);
  Eigen::Vector3d normal = convert<vec, Eigen::Vector3d>(n);

  return normal.normalized();
}



}  // end namespace shapeworks
