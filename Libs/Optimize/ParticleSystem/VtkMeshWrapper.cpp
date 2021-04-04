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
#include <vtkPLYWriter.h>

#include <igl/grad.h>
#include <igl/per_vertex_normals.h>
#include <geometrycentral/surface/surface_mesh_factories.h>

namespace shapeworks {

namespace {
static float epsilon = 1e-6;
}

namespace {
template<class FROM, class TO>
inline TO convert(FROM &value) {
  TO converted;
  converted[0] = value[0];
  converted[1] = value[1];
  converted[2] = value[2];
  return converted;
}
}

template<class T>
inline std::string PrintValue(T value) {
  return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " +
         std::to_string(value[2]) + ")";
}

using vec3 = Eigen::Vector3d;
using NormalType = VtkMeshWrapper::NormalType;
using VectorType = VtkMeshWrapper::VectorType;
using PointType = VtkMeshWrapper::PointType;
using GradNType = VtkMeshWrapper::GradNType;

//---------------------------------------------------------------------------
VtkMeshWrapper::VtkMeshWrapper(vtkSmartPointer<vtkPolyData> poly_data,
                               bool is_geodesics_enabled, size_t geodesics_cache_size) {
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

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  this->GetIGLMesh(V, F);
  this->ComputeGradN(V, F);

  this->is_geodesics_enabled_ = is_geodesics_enabled;
  if (is_geodesics_enabled_) {
    if(geodesics_cache_size == 0) {
      // heuristic for cache size based on some experiments
      geodesics_cache_size = 3 * 115 * this->triangles_.size();
    }
    this->geo_max_cache_entries_ = geodesics_cache_size;
    this->PrecomputeGeodesics(V, F);
  }
}

//---------------------------------------------------------------------------
double VtkMeshWrapper::ComputeDistance(const PointType &pt_a, int idx_a,
                                       const PointType &pt_b, int idx_b, VectorType *out_grad) const {
  // return euclidean if geodesics are disabled
  if (!is_geodesics_enabled_) {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = pt_a[i] - pt_b[i];
      }
    }
    return pt_a.EuclideanDistanceTo(pt_b);
  }

  // Find the triangle for the point a. If this was the same as the previous query, just used that cached value
  int face_a, face_b;
  vec3 bary_a, bary_b;
  if (idx_a >=0 && geo_lq_pidx_ == idx_a) {
    face_a = geo_lq_face_;
    bary_a = geo_lq_bary_;
  } else {
    face_a = ComputeFaceAndWeights(pt_a, idx_a, bary_a);

    geo_lq_pidx_ = idx_a;
    geo_lq_face_ = face_a;
    geo_lq_bary_ = bary_a;
  }

  // Find the triangle for the point b
  // (some initial experiments at caching these like we do for point_a proved to be unfruitful. no significant perf gain)
  face_b = ComputeFaceAndWeights(pt_b, idx_b, bary_b);

  // The geodesics(and more importantly, its gradient) are very inaccurate if both the points are on the
  // same face, or share an edge. In this case, just return the euclidean distance
  if (face_a == face_b || AreFacesInKRing(face_a, face_b)) {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = pt_a[i] - pt_b[i];
      }
    }
    return pt_a.EuclideanDistanceTo(pt_b);
  }

  // Define for convenience
  const auto& faces = this->triangles_;
  const auto vb0 = faces[face_b]->GetPointId(0);
  const auto vb1 = faces[face_b]->GetPointId(1);
  const auto vb2 = faces[face_b]->GetPointId(2);

  // Compute geodesic distance via barycentric approximation
  // Geometric Correspondence for Ensembles of Nonregular Shapes, Datar et al
  // https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3346950/
  const Eigen::Matrix3d geo_from_a = GeodesicsFromTriangleToTriangle(face_a, face_b);
  const Eigen::Vector3d geo_to_b = geo_from_a * bary_b;
  const double geo_dist = bary_a.dot(geo_to_b);

  // Check if gradient is needed
  if (out_grad == nullptr) {
    return geo_dist;
  }

  // Compute gradient of geodesics
  const auto& G = face_grad_[face_a];
  Eigen::Vector3d out_grad_eigen = (G*geo_to_b).rowwise().sum();
  out_grad_eigen *= geo_dist / out_grad_eigen.norm();

  // Copy to VNL data structure
  (*out_grad)[0] = out_grad_eigen[0];
  (*out_grad)[1] = out_grad_eigen[1];
  (*out_grad)[2] = out_grad_eigen[2];

  return geo_dist;
}

//---------------------------------------------------------------------------
bool VtkMeshWrapper::IsWithinDistance(const PointType &pt_a, int idx_a,
                                      const PointType &pt_b, int idx_b,
                                      double test_dist, double& dist) const
{
  if(!is_geodesics_enabled_) {
    dist = pt_a.EuclideanDistanceTo(pt_b);
    return dist < test_dist;
  }

  if(idx_a != -1) {
    if(particle_neighboorhood_.size() <= idx_a) {
      particle_neighboorhood_.resize(idx_a+1);
    }
    particle_neighboorhood_[idx_a] = test_dist;
  }

  // Find the triangle for the point a. If this was the same as the previous query, just used that cached value
  int face_a, face_b;
  vec3 bary_a, bary_b;
  if (geo_lq_pidx_ == idx_a) {
    face_a = geo_lq_face_;
    bary_a = geo_lq_bary_;
  } else {
    face_a = ComputeFaceAndWeights(pt_a, idx_a, bary_a);

    geo_lq_pidx_ = idx_a;
    geo_lq_face_ = face_a;
    geo_lq_bary_ = bary_a;
  }

  // Find the triangle for the point b
  face_b = ComputeFaceAndWeights(pt_b, idx_b, bary_b);
  if (face_a == face_b || AreFacesInKRing(face_a, face_b)) {
    dist = pt_a.EuclideanDistanceTo(pt_b);
    return dist < test_dist;
  }

  const auto vb0 = this->triangles_[face_b]->GetPointId(0);
  const auto vb1 = this->triangles_[face_b]->GetPointId(1);
  const auto vb2 = this->triangles_[face_b]->GetPointId(2);

  bool should_check = false;
  for(int i=0; i<3; i++) {
    const auto vai = this->triangles_[face_a]->GetPointId(i);

    // 1.5 is an heuristic to pull in a little more than we need
    const auto& geo_entry = GeodesicsFromVertex(vai, test_dist*1.5);
    if(geo_entry.is_full_mode()) {
      should_check = true;
    } else {
      const auto& data = geo_entry.data_partial;
      const auto find_v0 = data.find(vb0);
      const auto find_v1 = data.find(vb1);
      const auto find_v2 = data.find(vb2);
      if(find_v0 != data.end() || find_v1 != data.end() || find_v2 != data.end()) {
        should_check = true;
      }
    }
  }

  if(!should_check) {
    return false;
  }

  const Eigen::Matrix3d geo_from_a = GeodesicsFromTriangleToTriangle(face_a, face_b);
  const Eigen::Vector3d geo_to_b = geo_from_a * bary_b;
  dist = bary_a.dot(geo_to_b);
  return dist < test_dist;
}

//---------------------------------------------------------------------------
PointType VtkMeshWrapper::GeodesicWalk(PointType p, int idx,
                                       VectorType vector) const
{
  vec3 point(p[0], p[1], p[2]);
  double closest_point[3];
  int face_index = GetTriangleForPoint(point.data(), idx, closest_point);
  point = convert<double[3], vec3>(closest_point);

  Eigen::Vector3d vector_eigen =
    convert<VectorType, Eigen::Vector3d>(vector);
  Eigen::Vector3d projected_vector =
    this->ProjectVectorToFace(GetFaceNormal(face_index), vector_eigen);

  int ending_face = -1;

  Eigen::Vector3d new_point = GeodesicWalkOnFace(point, projected_vector, face_index,
                                                 ending_face);

  PointType new_point_pt = convert<Eigen::Vector3d, PointType>(new_point);

  // update cache
  if (idx >= 0 && ending_face >= 0) {
    if (idx >= particle_triangles_.size()) {
      particle_triangles_.resize(idx + 1, -1);
    }

    particle_triangles_[idx] = ending_face;
    geo_lq_pidx_ = -1;

    this->CalculateNormalAtPoint(new_point_pt, idx);
  }

  return new_point_pt;

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
VectorType
VtkMeshWrapper::ProjectVectorToSurfaceTangent(const PointType &pointa, int idx,
                                              VectorType &vector) const
{
  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];
  double closest_point[3];

  int faceIndex = this->GetTriangleForPoint(point, idx, closest_point);

  double* normal = this->poly_data_->GetCellData()->GetNormals()->GetTuple(faceIndex);

  Eigen::Vector3d vec_normal = convert<double*, vec3>(normal);
  Eigen::Vector3d vec_vector = convert<VectorType &, vec3>(vector);

  Eigen::Vector3d result = this->ProjectVectorToFace(vec_normal, vec_vector);
  VectorType resultvnl(result[0], result[1], result[2]);
  return resultvnl;
}

//---------------------------------------------------------------------------
NormalType VtkMeshWrapper::SampleNormalAtPoint(PointType p, int idx) const
{
  // if the particle is not in the cache or it has changed position, we must recompute
  if (idx < 0 || idx >= this->particle_normals_.size() || p != this->particle_positions_[idx]) {
    return this->CalculateNormalAtPoint(p, idx);
  }
  return this->particle_normals_[idx];
}

//---------------------------------------------------------------------------
GradNType VtkMeshWrapper::SampleGradNAtPoint(PointType p, int idx) const
{
  vec3 weights;
  int face_index = this->ComputeFaceAndWeights(p, idx, weights);

  GradNType weighted_grad_normal = GradNType(0.0);

  for (int i = 0; i < 3; i++) {
    auto id = this->triangles_[face_index]->GetPointId(i);
    GradNType grad_normal = grad_normals_[id];
    grad_normal *= weights[i];
    weighted_grad_normal += grad_normal;
  }
  return weighted_grad_normal;
}

//---------------------------------------------------------------------------
PointType VtkMeshWrapper::SnapToMesh(PointType pointa, int idx) const
{
  VtkMeshWrapper::PointType out;
  this->GetTriangleForPoint(pointa.GetDataPointer(), idx, out.GetDataPointer());
  return out;
}

//---------------------------------------------------------------------------
PointType VtkMeshWrapper::GetPointOnMesh() const
{
  PointType p = GetMeshUpperBound();
  p = SnapToMesh(p, -1);
  return p;
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::GetTriangleForPoint(const double pt[3], int idx, double closest_point[3]) const
{
  // given a guess, just check whether it is still valid.
  if (idx >= 0) {
    // ensure that the cache has enough elements. this will never be resized to more than the number of particles,
    if (idx >= particle_triangles_.size()) {
      particle_triangles_.resize(idx + 1, -1);
    }

    const int guess = particle_triangles_[idx];

    if (guess != -1 && this->IsInTriangle(pt, guess)) {
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
    // update cache, no need to check size as it was already checked above
    particle_triangles_[idx] = cell_id;
  }

  assert(cell_id >= 0);
  return cell_id;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
VtkMeshWrapper::ProjectVectorToFace(const Eigen::Vector3d &normal,
                                    const Eigen::Vector3d &vector) const
{
  auto old_mag = vector.norm();

  Eigen::Vector3d new_vector = vector - normal * normal.dot(vector);

  /// Old behavior is to not rescale it, uncomment for the old behavior
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
void VtkMeshWrapper::ComputeGradN(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F)
{
  const int n_verts = V.rows();
  const int n_faces = F.rows();

  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);

  // Compute normals
  Eigen::MatrixXd N;
  igl::per_vertex_normals(V, F, N);

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
Eigen::Vector3d VtkMeshWrapper::ComputeBarycentricCoordinates(const Eigen::Vector3d& pt, int face) const
{
  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  Eigen::Vector3d bary;
  this->triangles_[face]->EvaluatePosition(pt.data(), closest, sub_id, pcoords, dist2, bary.data());
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
                                   int face_index, int &ending_face) const
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

      double sqrLength0 = (intersect - currentPoint).squaredNorm();
      double sqrLength1 = (intersect1 - currentPoint).squaredNorm();

      if (sqrLength0 < sqrLength1) {
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
int VtkMeshWrapper::SlideAlongEdge(Eigen::Vector3d &point, Eigen::Vector3d &remaining_vector,
                                   int face, int edge) const
{

  int indexa = (edge + 1) % 3;
  int indexb = (edge + 2) % 3;
  int vertexindexa = this->GetFacePointID(face, indexa);
  int vertexindexb = this->GetFacePointID(face, indexb);
  Eigen::Vector3d pointa = this->GetVertexCoords(vertexindexa);
  Eigen::Vector3d pointb = this->GetVertexCoords(vertexindexb);
  Eigen::Vector3d mesh_edge(pointb[0] - pointa[0], pointb[1] - pointa[1], pointb[2] - pointa[2]);
  mesh_edge.normalize();
  double dotprod = mesh_edge.dot(remaining_vector);
  Eigen::Vector3d projected_vector = mesh_edge.normalized() * dotprod;

  Eigen::Vector3d max_slide = pointb - point;
  double new_dot = projected_vector.dot(mesh_edge);
  int towards_edge = indexa;
  if (new_dot < 0) {
    // going in opposite direction as mesh edge
    max_slide = pointa - point;
    towards_edge = indexb;
  }

  if (projected_vector.norm() > max_slide.norm()) {
    point += max_slide;
    remaining_vector = projected_vector - max_slide;
    return this->GetAcrossEdge(face, towards_edge);
  }
  else {
    point += projected_vector;
    remaining_vector = Eigen::Vector3d(0, 0, 0);
    return face;
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
Eigen::Vector3d VtkMeshWrapper::RotateVectorToFace(const Eigen::Vector3d &prev_normal,
                                                   const Eigen::Vector3d &next_normal,
                                                   const Eigen::Vector3d &vector) const
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
NormalType VtkMeshWrapper::CalculateNormalAtPoint(PointType p, int idx) const
{
  vec3 weights;
  int face_index = this->ComputeFaceAndWeights(p, idx, weights);

  NormalType weighted_normal(0, 0, 0);

  for (int i = 0; i < 3; i++) {
    auto id = this->triangles_[face_index]->GetPointId(i);
    double* normal = this->poly_data_->GetPointData()->GetNormals()->GetTuple(id);
    weighted_normal[0] = weighted_normal[0] + normal[0] * weights[i];
    weighted_normal[1] = weighted_normal[1] + normal[1] * weights[i];
    weighted_normal[2] = weighted_normal[2] + normal[2] * weights[i];
  }

  if (idx >= 0) { // cache
    if (idx >= this->particle_normals_.size()) {
      this->particle_normals_.resize(idx + 1);
      this->particle_positions_.resize(idx + 1);
    }
    this->particle_positions_[idx] = p;
    this->particle_normals_[idx] = weighted_normal;
  }

  return weighted_normal;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::InvalidateParticle(int idx)
{
  assert(idx >= 0); // should always be passed a valid particle
  if (idx >= particle_triangles_.size()) {
    particle_triangles_.resize(idx + 1, -1);
  }
  this->particle_triangles_[idx] = -1;
  this->geo_lq_pidx_ = -1;
}

//---------------------------------------------------------------------------
int VtkMeshWrapper::ComputeFaceAndWeights(const PointType& p, int idx, Eigen::Vector3d &weights) const
{
  Eigen::Vector3d closest_point;
  int face_index = this->GetTriangleForPoint(p.GetDataPointer(), idx, closest_point.data());
  // TODO: GetTriangleForPoint is usually already computing the weights
  weights = this->ComputeBarycentricCoordinates(closest_point, face_index);
  return face_index;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::GetIGLMesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const
{
  const int n_verts = this->poly_data_->GetNumberOfPoints();
  const int n_faces = this->poly_data_->GetNumberOfCells();

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
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::PrecomputeGeodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F)
{
  // Resize cache to correct size
  geo_dist_cache_.resize(V.rows());

  // Compute gradient operator
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);

  // Flatten the gradient operator so we can quickly compute the gradient at a given point
  face_grad_.resize(F.rows());
  size_t n_insertions = 0;
  for(int k=0; k<G.outerSize(); k++) {
    for(Eigen::SparseMatrix<double>::InnerIterator it(G, k); it; ++it) {
      const double val = it.value();
      const auto r = it.row();
      const auto c = it.col();

      const auto f = r % F.rows();
      const auto axis = r / F.rows();
      for(int i=0; i<3; i++) {
        if(F(f, i) == c) {
          face_grad_[f](axis, i) = val;
          n_insertions++;
          break;
        }
      }
    }
  }
  assert(n_insertions == 9*F.rows());


  // geometry central stuff
  {
    using namespace geometrycentral::surface;
    std::tie(gc_mesh_, gc_geometry_) = makeSurfaceMeshAndGeometry(V, F);
    gc_heatsolver_ = std::make_unique<HeatMethodDistanceSolver>(*gc_geometry_);
  }

  // compute k-ring
  const int k = 1;
  face_kring_.resize(this->triangles_.size());
  for(int f=0; f<this->triangles_.size(); f++) {
    ComputeKRing(f, k, face_kring_[f]);
  }
}

//---------------------------------------------------------------------------
bool VtkMeshWrapper::AreFacesInKRing(int f_a, int f_b) const
{
  return face_kring_[f_a].find(f_b) != face_kring_[f_a].end();
}

//---------------------------------------------------------------------------
const MeshGeoEntry& VtkMeshWrapper::GeodesicsFromVertex(int v, double max_dist, int req_target_f) const
{
  if(geo_cache_size_ >= geo_max_cache_entries_) {
    this->ClearGeodesicCache();
  }

  auto& entry = geo_dist_cache_[v];
  if(entry.is_full_mode()) {
    return entry;
  }

  if(entry.max_dist >= max_dist) {
    return entry;
  }

  geo_cache_size_ -= entry.data_partial.size();
  entry.data_partial.clear();

  const auto n_verts = this->poly_data_->GetNumberOfPoints();

  // Compute geodesics using heat method
  // todo switch to zero-copy API when that is available: https://github.com/nmwsharp/geometry-central/issues/77
  const auto gc_v = gc_mesh_->vertex(v);
  const auto geodesics = gc_heatsolver_->computeDistance(gc_v);

  if(max_dist == std::numeric_limits<double>::infinity()) {
    entry.mode = MeshGeoEntry::Full;
    entry.data_full = std::move(geodesics.raw());
    entry.update_max_dist();
    geo_cache_size_ += n_verts;
    return entry;
  }

  // if the caller requires the geodesics to every vertex of a certain triangle
  if(req_target_f >= 0) {
    for(int i=0; i<3; i++) {
      const int req_v = this->triangles_[req_target_f]->GetPointId(i);
      max_dist = std::max(max_dist, geodesics[req_v]);
    }
  }

  // this is a heuristic chosen: at a certain size, its faster to switch to full mode
  const auto switch_to_full_at = n_verts / 4;

  robin_hood::unordered_set<int> needed_points;
  auto incident_cells = vtkSmartPointer<vtkIdList>::New();
  double new_max_dist = max_dist;

  // Figure out which of these we actually need. If a vertex is included, we also pull in all 1-ring neighbors of
  // that vertex since they may be needed to compute geodesics on triangles incident on that vertex
  for(int i=0; i<n_verts; i++) {
    const auto d = geodesics[i];
    if(d <= max_dist) {
      this->poly_data_->GetPointCells(i, incident_cells);
      for(int j=0; j<incident_cells->GetNumberOfIds(); j++) {
        const auto& tri = this->triangles_[incident_cells->GetId(j)];
        needed_points.insert(tri->GetPointId(0));
        needed_points.insert(tri->GetPointId(1));
        needed_points.insert(tri->GetPointId(2));

        if(needed_points.size() >= switch_to_full_at) {
          break;
        }
      }
    }
    if(needed_points.size() >= switch_to_full_at) {
      break;
    }
  }

  if(needed_points.size() >= switch_to_full_at) {
    entry.mode = MeshGeoEntry::Full;
    entry.data_full = std::move(geodesics.raw());
    entry.update_max_dist();
    geo_cache_size_ += n_verts;
    return entry;
  }

  for(auto v : needed_points) {
    const auto d = geodesics[v];
    new_max_dist = std::max(new_max_dist, d);
    entry.data_partial[v] = d;
  }

  geo_cache_size_ += entry.data_partial.size();
  entry.max_dist = new_max_dist;
  return entry;
}

//---------------------------------------------------------------------------
const Eigen::Matrix3d VtkMeshWrapper::GeodesicsFromTriangleToTriangle(int f_a, int f_b) const
{
  const int vb0 = this->triangles_[f_b]->GetPointId(0);
  const int vb1 = this->triangles_[f_b]->GetPointId(1);
  const int vb2 = this->triangles_[f_b]->GetPointId(2);

  Eigen::Matrix3d result;
  for(int i=0; i<3; i++) {
    const int vai = this->triangles_[f_a]->GetPointId(i);
    const auto& entry = geo_dist_cache_[vai];

    // if full mode, then we have all geodesics available from this vertex
    if(entry.is_full_mode()) {
      const auto& data = entry.data_full;
      result(i, 0) = data[vb0];
      result(i, 1) = data[vb1];
      result(i, 2) = data[vb2];
    } else {
      // Partial mode: we may have to pull in the geodesics if we don't have all of them
      const auto& data = entry.data_partial;
      auto find_v0 = data.find(vb0);
      auto find_v1 = data.find(vb1);
      auto find_v2 = data.find(vb2);

      // geodesics to atleast one vertex is missing. pull it in
      if(find_v0 == data.end() || find_v1 == data.end() || find_v2 == data.end()) {
        // call GeodesicsFromVertex to ensure that we have the target face in cache
        const auto& new_entry = GeodesicsFromVertex(vai, 0.0, f_b);

        // if we ended up caching geodesics to all vertices, we fill in the result
        // and continue the loop to the next vertex of a
        if(new_entry.is_full_mode()) {
          result(i, 0) = new_entry.data_full[vb0];
          result(i, 1) = new_entry.data_full[vb1];
          result(i, 2) = new_entry.data_full[vb2];
          continue;
        }

        // otherwise, get it from the cache
        find_v0 = new_entry.data_partial.find(vb0);
        find_v1 = new_entry.data_partial.find(vb1);
        find_v2 = new_entry.data_partial.find(vb2);
      }

      result(i, 0) = find_v0->second;
      result(i, 1) = find_v1->second;
      result(i, 2) = find_v2->second;
    }
  }

  return result;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ClearGeodesicCache() const {
  const auto n_verts = this->poly_data_->GetNumberOfPoints();

  robin_hood::unordered_set<int> active_verts;
  for(auto f : particle_triangles_) {
    if(f < 0) {
      continue;
    }

    for(int i=0; i<3; i++) {
      const int v = this->triangles_[f]->GetPointId(i);
      active_verts.insert(v);
    }
  }

  size_t new_cache_size = 0;

  // figure out which triangles each particle is on, and clear only the other entries
  for(int i=0; i<geo_dist_cache_.size(); i++) {
    auto& entry = geo_dist_cache_[i];
    if(active_verts.find(i) == active_verts.end()) {
      entry.clear();
    } else {
      new_cache_size += entry.is_full_mode() ? n_verts : entry.data_partial.size();
    }
  }

  // the cache is not large enough to store the subset that we know is required. we are forced to clear everything
  if(new_cache_size > geo_max_cache_entries_) {
    std::cerr << "Warning: Cache entries too small, clearing everything. Consider increasing cache size if this happens repeatedly\n";
    for(auto i : active_verts) {
      if(i >= 0) {
        geo_dist_cache_[i].clear();
      }
    }
    new_cache_size = 0;
  }

  geo_cache_size_ = new_cache_size;
}

//---------------------------------------------------------------------------
void VtkMeshWrapper::ComputeKRing(int f, int k, std::unordered_set<int>& ring) const {
  if(k == 0) {
    return;
  }

  auto neighbors = vtkSmartPointer<vtkIdList>::New();
  for(int i=0; i<3; i++) {
    const int v = this->triangles_[f]->GetPointId(i);
    this->poly_data_->GetPointCells(v, neighbors);
    for(int j=0; j<neighbors->GetNumberOfIds(); j++) {
      const int f_j = neighbors->GetId(j);
      if(ring.find(f_j) != ring.end()) {
        ring.insert(f_j);
        ComputeKRing(f_j, k-1, ring);
      }
    }
  }
}

}