#include "VtkMeshWrapper.h"

#include <vtkCellLocator.h>
#include <vtkPolyDataNormals.h>
#include <vtkCellData.h>
#include <vtkCell.h>
#include <vtkPointData.h>
#include <vtkTriangleFilter.h>
#include <vtkGenericCell.h>
#include <vtkCleanPolyData.h>
#include <vtkTriangle.h>

#include <igl/grad.h>
#include <igl/per_vertex_normals.h>

namespace shapeworks {

namespace {
static float epsilon = 1e-6;
}

namespace {
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

template<class T>
inline std::string PrintValue(T value)
{
  return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " +
         std::to_string(value[2]) + ")";
}

using vec3 = Eigen::Vector3d;

//---------------------------------------------------------------------------
VtkMeshWrapper::VtkMeshWrapper(vtkSmartPointer<vtkPolyData> poly_data)
{
  vtkSmartPointer<vtkTriangleFilter> triangle_filter =
    vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInputData(poly_data);
  triangle_filter->Update();

  vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->ConvertPolysToLinesOff();
  clean->ConvertLinesToPointsOff();
  clean->ConvertStripsToPolysOff();
  clean->PointMergingOn();
  clean->SetInputConnection(triangle_filter->GetOutputPort());

  vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputConnection(clean->GetOutputPort());
  normals->SplittingOff(); // very important or connectivity is lost
  normals->ComputeCellNormalsOn();
  normals->ComputePointNormalsOn();
  normals->Update();

  this->poly_data_ = normals->GetOutput();
  this->poly_data_->BuildCells();
  this->poly_data_->BuildLinks();

  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();
  for (int i = 0; i < this->poly_data_->GetNumberOfCells(); i++) {
    this->poly_data_->GetCell(i, cell);

    vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
    triangle->GetPointIds()->SetNumberOfIds(3);
    triangle->GetPointIds()->SetId(0, cell->GetPointId(0));
    triangle->GetPointIds()->SetId(1, cell->GetPointId(1));
    triangle->GetPointIds()->SetId(2, cell->GetPointId(2));
    triangle->GetPoints()->SetPoint(0, cell->GetPoints()->GetPoint(0));
    triangle->GetPoints()->SetPoint(1, cell->GetPoints()->GetPoint(1));
    triangle->GetPoints()->SetPoint(2, cell->GetPoints()->GetPoint(2));

    this->triangles_.push_back(triangle);
  }

  this->cell_locator_ = vtkSmartPointer<vtkCellLocator>::New();
  this->cell_locator_->SetCacheCellBounds(true);
  this->cell_locator_->SetDataSet(this->poly_data_);
  this->cell_locator_->BuildLocator();

  this->ComputeMeshBounds();
  this->ComputeGradN();
}

//---------------------------------------------------------------------------
double VtkMeshWrapper::ComputeDistance(VtkMeshWrapper::PointType pointa,
                                       VtkMeshWrapper::PointType pointb) const
{
  double x = pointa[0] - pointb[0];
  double y = pointa[1] - pointb[1];
  double z = pointa[2] - pointb[2];
  double distance = std::sqrt((x * x) + (y * y) + (z * z));
  return distance;
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType VtkMeshWrapper::GeodesicWalk(VtkMeshWrapper::PointType pointa, int idx,
                                                       vnl_vector_fixed<double, 3> vector) const
{
  //std::cerr << "------------------------------------------\n";
  //std::cerr << "GeodesicWalk\n";

  PointType snapped = this->SnapToMesh(pointa, idx);
  vec3 point(snapped[0], snapped[1], snapped[2]);

  double closest_point[3];

  int faceIndex = GetTriangleForPoint(point.data(), idx, closest_point);

  vec3 currentBary = this->ComputeBarycentricCoordinates(point, faceIndex);
  //std::cerr << "Starting Bary: " << PrintValue<Eigen::Vector3d>(currentBary) << "\n";

  Eigen::Vector3d vectorEigen = convert<vnl_vector_fixed<double, DIMENSION>, Eigen::Vector3d>(
    vector);
  Eigen::Vector3d projectedVector = this->ProjectVectorToFace(GetFaceNormal(faceIndex),
                                                              vectorEigen);

  Eigen::Vector3d snappedPoint = convert<PointType, Eigen::Vector3d>(snapped);
  int ending_face = -1;

  Eigen::Vector3d newPoint = GeodesicWalkOnFace(snappedPoint, projectedVector, faceIndex,
                                                ending_face);

  PointType newPointpt;
  newPointpt[0] = newPoint[0];
  newPointpt[1] = newPoint[1];
  newPointpt[2] = newPoint[2];

  // update cache
  if (idx >= 0 && ending_face >= 0) {
    if (idx >= particle2tri_.size()) {
      particle2tri_.resize(idx + 1, 0);
    }

    particle2tri_[idx] = ending_face;

    if (idx >= this->particle_normals_.size()) {
      this->particle_normals_.resize(idx + 1);
    }
    this->particle_normals_[idx] = this->CalculateNormalAtPoint(newPointpt, idx);
  }

  return newPointpt;

  /*
   * Alternate, just snap to mesh
   *
  pointa[0] += vector[0];
  pointa[1] += vector[1];
  pointa[2] += vector[2];

  return this->SnapToMesh(pointa, idx);
   */
}

//---------------------------------------------------------------------------
vnl_vector_fixed<double, DIMENSION>
VtkMeshWrapper::ProjectVectorToSurfaceTangent(const VtkMeshWrapper::PointType& pointa, int idx,
                                              vnl_vector_fixed<double, 3>& vector) const
{

  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];

  double closest_point[3];

  int faceIndex = this->GetTriangleForPoint(point, idx, closest_point);

  double* normal = this->poly_data_->GetCellData()->GetNormals()->GetTuple(faceIndex);

  Eigen::Vector3d vec_normal;
  vec_normal[0] = normal[0];
  vec_normal[1] = normal[1];
  vec_normal[2] = normal[2];

  Eigen::Vector3d vec_vector;
  vec_vector[0] = vector[0];
  vec_vector[1] = vector[1];
  vec_vector[2] = vector[2];

  Eigen::Vector3d result = this->ProjectVectorToFace(vec_normal, vec_vector);
  vnl_vector_fixed<double, DIMENSION> resultvnl(result[0], result[1], result[2]);
  return resultvnl;

}

//---------------------------------------------------------------------------
vnl_vector_fixed<float, DIMENSION>
VtkMeshWrapper::SampleNormalAtPoint(VtkMeshWrapper::PointType p, int idx) const
{

  if (idx < 0 || idx >= this->particle_normals_.size()) {
    return this->CalculateNormalAtPoint(p, idx);
  }

  return this->particle_normals_[idx];
}

//---------------------------------------------------------------------------
VtkMeshWrapper::GradNType
VtkMeshWrapper::SampleGradNAtPoint(VtkMeshWrapper::PointType p, int idx) const
{
  double point[3];
  point[0] = p[0];
  point[1] = p[1];
  point[2] = p[2];

  double closest_point[3];
  int face_index = this->GetTriangleForPoint(point, idx, closest_point);

  auto cell = this->poly_data_->GetCell(face_index);

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[3];
  cell->EvaluatePosition(point, closest, sub_id, pcoords, dist2, weights);

  GradNType weighted_grad_normal = GradNType(0.0);

  for (int i = 0; i < cell->GetNumberOfPoints(); i++) {
    auto id = cell->GetPointId(i);
    GradNType grad_normal = grad_normals_[id];
    grad_normal *= weights[i];
    weighted_grad_normal += grad_normal;
  }
  return weighted_grad_normal;
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType
VtkMeshWrapper::SnapToMesh(VtkMeshWrapper::PointType pointa, int idx) const
{
  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];
  double closest_point[3];
  this->GetTriangleForPoint(point, idx, closest_point);

  VtkMeshWrapper::PointType out;
  out[0] = closest_point[0];
  out[1] = closest_point[1];
  out[2] = closest_point[2];
  return out;

/*

  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];

  double closest_point[3]; //the coordinates of the closest point will be returned here
  double closest_point_dist2; //the squared distance to the closest point will be returned here
  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id; //this is rarely used (in triangle strips only, I believe)

  this->cell_locator_->FindClosestPoint(point, closest_point, cell_id, sub_id, closest_point_dist2);

  VtkMeshWrapper::PointType out;
  out[0] = closest_point[0];
  out[1] = closest_point[1];
  out[2] = closest_point[2];

  // update cache
  if (idx > 0) {
    if (idx >= particle2tri_.size()) {
      particle2tri_.resize(idx + 1, 0);
    }

    particle2tri_[idx] = cell_id;
  }

  return out;
  */
}

//---------------------------------------------------------------------------
VtkMeshWrapper::PointType VtkMeshWrapper::GetPointOnMesh() const
{
  int face_index = 0;

  double* point = this->poly_data_->GetPoint(face_index);

  PointType p;
  p[0] = point[0];
  p[1] = point[1];
  p[2] = point[2];

  return p;
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::GetTriangleForPoint(const double pt[3], int idx, double closest_point[3]) const
{
  // given a guess, just check whether it is still valid.
  if (idx >= 0) {
    // ensure that the cache has enough elements. this will never be resized to more than the number of particles,
    if (idx >= particle2tri_.size()) {
      particle2tri_.resize(idx + 1, -1);
    }

    const int guess = particle2tri_[idx];

    /*
    if (guess != -1 && this->IsInTriangle(pt, guess)) {
      closest_point[0] = pt[0];
      closest_point[1] = pt[1];
      closest_point[2] = pt[2];
      return guess;
    }
    */


    if (guess > 0) {
      closest_point[0] = pt[0];
      closest_point[1] = pt[1];
      closest_point[2] = pt[2];
      return guess;
    }

  }

  //double closest_point[3];//the coordinates of the closest point will be returned here
  double closest_point_dist2; //the squared distance to the closest point will be returned here
  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id; //this is rarely used (in triangle strips only, I believe)

  this->cell_locator_->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

  if (idx >= 0) {
    particle2tri_[idx] = cell_id;
  }

  assert(cell_id >= 0);
  return cell_id;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
VtkMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d& normal,
                                    const Eigen::Vector3d& vector) const
{
  auto old_mag = vector.norm();

  Eigen::Vector3d new_vector = vector - normal * normal.dot(vector);

  //return new_vector;

  auto new_mag = new_vector.norm();
  double ratio = old_mag / new_mag;

  new_vector *= ratio;

  return new_vector;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ComputeMeshBounds()
{
  double buffer = 5.0;
  double bounds[6];
  this->poly_data_->GetBounds(bounds);
  this->mesh_lower_bound_[0] = bounds[0] - buffer;
  this->mesh_lower_bound_[1] = bounds[2] - buffer;
  this->mesh_lower_bound_[2] = bounds[4] - buffer;
  this->mesh_upper_bound_[0] = bounds[1] + buffer;
  this->mesh_upper_bound_[1] = bounds[3] + buffer;
  this->mesh_upper_bound_[2] = bounds[5] + buffer;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ComputeGradN()
{

  const int n_verts = this->poly_data_->GetNumberOfPoints();
  const int n_faces = this->poly_data_->GetNumberOfCells();

  // Copy from vtk to libigl data structures
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  V.resize(n_verts, 3);
  F.resize(n_faces, 3);

  auto points = this->poly_data_->GetPoints();
  for (int i = 0; i < n_verts; i++) {
    double p[3];
    points->GetPoint(i, p);
    V(i, 0) = p[0];
    V(i, 1) = p[1];
    V(i, 2) = p[2];
  }
  for (int i = 0; i < n_faces; i++) {
    auto cell = this->poly_data_->GetCell(i);
    assert (cell->GetNumberOfPoints() == 3);
    F(i, 0) = cell->GetPointId(0);
    F(i, 1) = cell->GetPointId(1);
    F(i, 2) = cell->GetPointId(2);
  }

  // Compute normals
  Eigen::MatrixXd N;
  igl::per_vertex_normals(V, F, N);

  // Compute gradient operator
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);

  // Compute gradient of normals per face
  const Eigen::MatrixXd GN = Eigen::Map<const Eigen::MatrixXd>((G * N).eval().data(), n_faces, 9);

  // Convert per-face values to per-vertex using face area as weight
  Eigen::MatrixXd GN_pervertex(n_verts, 9);
  GN_pervertex.setZero();
  Eigen::MatrixXd A_perface;
  igl::doublearea(V, F, A_perface);
  Eigen::VectorXd A_pervertex(n_verts);
  A_pervertex.setZero();
  // scatter the per-face values
  for (int i = 0; i < n_faces; i++) {
    for (int j = 0; j < 3; j++) {
      GN_pervertex.row(F(i, j)) += A_perface(i, 0) * GN.row(i);
      A_pervertex(F(i, j)) += A_perface(i, 0);
    }
  }
  for (int i = 0; i < n_verts; i++) {
    if (A_pervertex(i) != 0.0) {
      GN_pervertex.row(i) /= A_pervertex(i);
    }
  }

  // Copy back to VNL data structure
  grad_normals_.resize(n_verts);
  for (int j = 0; j < n_verts; j++) {
    for (int i = 0; i < 3; i++) {
      grad_normals_[j].set(i, 0, GN_pervertex(j, i * 3 + 0));
      grad_normals_[j].set(i, 1, GN_pervertex(j, i * 3 + 1));
      grad_normals_[j].set(i, 2, GN_pervertex(j, i * 3 + 2));
    }
  }

}

//---------------------------------------------------------------------------
bool VtkMeshWrapper::IsInTriangle(const double* pt, int face_index) const
{
  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double bary[3];

  int ret = this->triangles_[face_index]->EvaluatePosition(pt, closest, sub_id, pcoords, dist2,
                                                           bary);
  if (ret && dist2 < epsilon) {
    bool bary_check = ((bary[0] >= -epsilon) && (bary[0] <= 1 + epsilon)) &&
                      ((bary[1] >= -epsilon) && (bary[1] <= 1 + epsilon)) &&
                      ((bary[2] >= -epsilon) && (bary[2] <= 1 + epsilon));
    return bary_check;

  }
  return false;
}

//---------------------------------------------------------------------------
Eigen::Vector3d VtkMeshWrapper::ComputeBarycentricCoordinates(Eigen::Vector3d pt, int face) const
{

  double point[3];
  point[0] = pt[0];
  point[1] = pt[1];
  point[2] = pt[2];

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[3];
  this->triangles_[face]->EvaluatePosition(point, closest, sub_id, pcoords, dist2,
                                           weights);

  Eigen::Vector3d bary(weights[0], weights[1], weights[2]);
  return bary;
}

//---------------------------------------------------------------------------
const Eigen::Vector3d VtkMeshWrapper::GetFaceNormal(int face_index) const
{
  auto normals = this->poly_data_->GetCellData()->GetNormals();
  double* normal = normals->GetTuple(face_index);
  Eigen::Vector3d n(normal[0], normal[1], normal[2]);
  return n;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
VtkMeshWrapper::GeodesicWalkOnFace(Eigen::Vector3d point_a, Eigen::Vector3d projected_vector,
                                   int face_index, int& ending_face) const
{
  int currentFace = face_index;
  Eigen::Vector3d currentPoint = point_a;
  Eigen::Vector3d remainingVector = projected_vector;
  double minimumUpdate = 0.0000000001;
  double barycentricEpsilon = 0.0001;
  std::vector<int> facesTraversed;
  //std::vector<vec3> positions;
  int prevFace = currentFace;
  while (remainingVector.norm() > minimumUpdate && currentFace != -1) {
    facesTraversed.push_back(currentFace);
    vec3 currentBary = ComputeBarycentricCoordinates(
      vec3(currentPoint[0], currentPoint[1], currentPoint[2]), currentFace);
    //std::cerr << "Current Bary: " << PrintValue<Eigen::Vector3d>(currentBary) << "\n";
    assert(abs(currentBary[0]) < 1.01);
    assert(abs(currentBary[1]) < 1.01);
    assert(abs(currentBary[2]) < 1.01);

    Eigen::Vector3d targetPoint = currentPoint + remainingVector;
    //positions.push_back(currentPoint);
    vec3 targetBary = ComputeBarycentricCoordinates(
      vec3(targetPoint[0], targetPoint[1], targetPoint[2]), currentFace);
    //std::cerr << "Target Bary: " << PrintValue<Eigen::Vector3d>(targetBary) << "\n";

    if (facesTraversed.size() >= 3 &&
        facesTraversed[facesTraversed.size() - 1] == facesTraversed[facesTraversed.size() - 3]) {
      // When at the intersection of two faces while also being at the edge of the mesh, the edge-sliding will keep alternating
      // between the two faces without actually going anywhere since it is at a corner in the mesh.
      //std::cerr << "exiting due to face repetition\n";
      break;
    }

    if (facesTraversed.size() > 1000) {
      std::cerr << "Warning, more than 1000 faces traversed\n";
      for (int i = 0; i < facesTraversed.size(); i++) {
        std::cerr << facesTraversed[i] << ", ";
      }
      //std::cerr << "\nPositions: ";
      //for (int i = 0; i < positions.size(); i++) {
      //    std::cerr << "(" << positions[i][0] << "," << positions[i][1] << "," << positions[i][2]
      //                << ") ";
      //  }
      /*
        std::cerr << "\n\n";
        std::cerr << "ID: " << idx << "\n";
        std::cerr << "Original point: " << PrintValue<Eigen::Vector3d>(point_a) << "\n";
        std::cerr << "Original vector: " << PrintValue<Eigen::Vector3d>(projected_vector) << "\n";
        std::cerr << "Current point: " << PrintValue<Eigen::Vector3d>(currentPoint) << "\n";
        std::cerr << "remaining vector: " << PrintValue<Eigen::Vector3d>(remainingVector) << "\n";
        std::cerr << "currentBary: " << PrintValue<vec3>(currentBary) << "\n";
        std::cerr << "targetPoint: " << PrintValue<Eigen::Vector3d>(targetPoint) << "\n";
        std::cerr << "targetBary: " << PrintValue<vec3>(targetBary) << "\n";
        std::cerr << std::endl;
        */
      break;

    }

    if (targetBary[0] + barycentricEpsilon >= 0 && targetBary[1] + barycentricEpsilon >= 0 &&
        targetBary[2] + barycentricEpsilon >= 0 && targetBary[0] - barycentricEpsilon <= 1 &&
        targetBary[1] - barycentricEpsilon <= 1 && targetBary[2] - barycentricEpsilon <= 1) {
      currentPoint = targetPoint;
      break;
    }

    std::vector<int> negativeVertices;
    for (int i = 0; i < 3; i++) {
      if (targetBary[i] < 0) {
        negativeVertices.push_back(i);
      }
    }

    if (negativeVertices.size() == 0 || negativeVertices.size() > 2) {
      std::cerr << "ERROR: invalid number of negative vertices. Point is not on surface.\n";
      break;
    }
    int negativeEdge = negativeVertices[0];
    Eigen::Vector3d intersect = GetBarycentricIntersection(currentBary, targetBary, currentFace,
                                                           negativeEdge);

    // When more than 1 negative barycentric coordinate, compute both intersections and take the closest one.
    if (negativeVertices.size() == 2) {
      int negativeEdge1 = negativeVertices[1];
      Eigen::Vector3d intersect1 = GetBarycentricIntersection(currentBary, targetBary, currentFace,
                                                              negativeEdge1);

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
    int nextFace = this->GetAcrossEdge(currentFace, negativeEdge);
    if (nextFace == -1) {
      nextFace = this->SlideAlongEdge(intersect, remaining, currentFace, negativeEdge);
    }
    remainingVector = remaining;
    if (nextFace != -1) {
      remainingVector = RotateVectorToFace(GetFaceNormal(currentFace),
                                           GetFaceNormal(nextFace), remainingVector);
    }
    currentPoint = intersect;
    currentFace = nextFace;
    if (currentFace != -1) {
      prevFace = currentFace;
    }
  }

  if (currentFace != -1) {
    prevFace = currentFace;
  }

  vec3 bary = ComputeBarycentricCoordinates(
    vec3(currentPoint[0], currentPoint[1], currentPoint[2]), prevFace);

  assert(abs(bary[0]) < 1.01);
  assert(abs(bary[1]) < 1.01);
  assert(abs(bary[2]) < 1.01);




  //ending_face = currentFace;
  ending_face = prevFace;
  assert(ending_face != -1);
  return currentPoint;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
VtkMeshWrapper::GetBarycentricIntersection(Eigen::Vector3d start, Eigen::Vector3d end,
                                           int currentFace, int edge) const
{
  vec3 delta = end - start;
  if (delta[edge] == 0) {
    // If going parallel to the edge, it is allowed to go all the way to the end where it wants to go
    return end;
  }
  double ratio = -start[edge] / delta[edge];
  vec3 intersect = start + delta * ratio;

  auto cell = this->poly_data_->GetCell(currentFace);

  vec3 inter(0, 0, 0);
  for (int q = 0; q < 3; q++) {
    auto point = cell->GetPoints()->GetPoint(q);
    vec3 p(point[0], point[1], point[2]);
    inter += p * intersect[q];
  }
  return inter;

}

//---------------------------------------------------------------------------
int VtkMeshWrapper::GetAcrossEdge(int face_id, int edge_id) const
{
  // get the neighbors of the cell
  auto neighbors = vtkSmartPointer<vtkIdList>::New();

  int edge_p1 = this->triangles_[face_id]->GetPointId(1);
  int edge_p2 = this->triangles_[face_id]->GetPointId(2);
  if (edge_id == 1) {
    edge_p1 = this->triangles_[face_id]->GetPointId(2);
    edge_p2 = this->triangles_[face_id]->GetPointId(0);
  }
  else if (edge_id == 2) {
    edge_p1 = this->triangles_[face_id]->GetPointId(0);
    edge_p2 = this->triangles_[face_id]->GetPointId(1);
  }

  this->poly_data_->GetCellEdgeNeighbors(face_id, edge_p1, edge_p2, neighbors);

  if (neighbors->GetNumberOfIds() == 0) {
    // This is the boundary edge of an open mesh
    return -1;
  }

  assert(neighbors->GetNumberOfIds() == 1);
  return neighbors->GetId(0);
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::SlideAlongEdge(Eigen::Vector3d& point_, Eigen::Vector3d& remainingVector_,
                                   int face_, int edge_) const
{

  int indexa = (edge_ + 1) % 3;
  int indexb = (edge_ + 2) % 3;
  int vertexindexa = this->GetFacePointID(face_, indexa);
  int vertexindexb = this->GetFacePointID(face_, indexb);
  Eigen::Vector3d pointa = this->GetVertexCoords(vertexindexa);
  Eigen::Vector3d pointb = this->GetVertexCoords(vertexindexb);
  Eigen::Vector3d meshEdge(pointb[0] - pointa[0], pointb[1] - pointa[1], pointb[2] - pointa[2]);
  meshEdge.normalize();
  double dotprod = meshEdge.dot(remainingVector_);
  Eigen::Vector3d projectedVector = meshEdge.normalized() * dotprod;

  Eigen::Vector3d maxSlide = pointb - point_;
  double newDot = projectedVector.dot(meshEdge);
  int towardsEdge = indexa;
  if (newDot < 0) {
    // going in opposite direction as mesh edge
    maxSlide = pointa - point_;
    towardsEdge = indexb;
  }

  if (projectedVector.norm() > maxSlide.norm()) {
    point_ += maxSlide;
    remainingVector_ = projectedVector - maxSlide;
    return this->GetAcrossEdge(face_, towardsEdge);
  }
  else {
    point_ += projectedVector;
    remainingVector_ = Eigen::Vector3d(0, 0, 0);
    return face_;
  }
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::GetFacePointID(int face, int point_id) const
{
  return this->poly_data_->GetCell(face)->GetPointId(point_id);
}

//---------------------------------------------------------------------------
Eigen::Vector3d VtkMeshWrapper::GetVertexCoords(int vertex_id) const
{
  double* p = this->poly_data_->GetPoint(vertex_id);
  return Eigen::Vector3d(p[0], p[1], p[2]);
}

//---------------------------------------------------------------------------
Eigen::Vector3d VtkMeshWrapper::RotateVectorToFace(const Eigen::Vector3d& prev_normal,
                                                   const Eigen::Vector3d& next_normal,
                                                   const Eigen::Vector3d& vector) const
{
  float dotprod = prev_normal.normalized().dot(next_normal.normalized());
  if (dotprod >= 1) {
    return vector;
  }
  float angle = acos(dotprod);
  Eigen::Vector3d rotationAxis = prev_normal.normalized().cross(
    next_normal.normalized()).normalized();
  Eigen::AngleAxisd transform(angle, rotationAxis);
  Eigen::Vector3d rotated = transform * vector;
  return rotated;
}

//---------------------------------------------------------------------------
vnl_vector_fixed<float, DIMENSION>
VtkMeshWrapper::CalculateNormalAtPoint(VtkMeshWrapper::PointType p, int idx) const
{

  double point[3];
  point[0] = p[0];
  point[1] = p[1];
  point[2] = p[2];

  double closest_point[3];

  /*
  int guess = -1;
  if (idx >= 0 && idx < particle2tri_.size()) {
    guess = particle2tri_[idx];
  }
   */
  int face_index = this->GetTriangleForPoint(point, idx, closest_point);

  /*
  if (idx >= 0 && idx < particle2tri_.size()) {
    if (guess != face_index) {
      std::cerr << "how can they be different? " << guess << " vs " << face_index << "\n";
    }
  }*/

  //auto cell = this->poly_data_->GetCell(face_index);
  //auto cell = this->triangles_[face_index];

  vnl_vector_fixed<float, DIMENSION> weightedNormal(0, 0, 0);

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[3];
  this->triangles_[face_index]->EvaluatePosition(point, closest, sub_id, pcoords, dist2, weights);

  //std::cerr << ret << ", weights = " << weights[0] << ", " << weights[1] << ", " << weights[2] << "\n";

  for (int i = 0; i < 3; i++) {

    auto id = this->triangles_[face_index]->GetPointId(i);

    double* normal = this->poly_data_->GetPointData()->GetNormals()->GetTuple(id);

    weightedNormal[0] = weightedNormal[0] + normal[0] * weights[i];
    weightedNormal[1] = weightedNormal[1] + normal[1] * weights[i];
    weightedNormal[2] = weightedNormal[2] + normal[2] * weights[i];
  }
  return weightedNormal;

}

void VtkMeshWrapper::InvalidateParticle(int idx)
{
  if (idx >= particle2tri_.size()) {
    particle2tri_.resize(idx + 1, -1);
  }
  this->particle2tri_[idx] = -1;
}

}