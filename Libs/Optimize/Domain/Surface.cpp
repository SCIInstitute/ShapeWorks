#include "Surface.h"

#include <geometrycentral/surface/surface_mesh_factories.h>
#include <igl/grad.h>
#include <igl/per_vertex_normals.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellLocator.h>
#include <vtkCleanPolyData.h>
#include <vtkGenericCell.h>
#include <vtkPLYWriter.h>
#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>
#include <vtkTriangle.h>
#include <vtkTriangleFilter.h>
#include <vtkLine.h>

#include "ExternalLibs/robin_hood/robin_hood.h"

#include <igl/barycentric_coordinates.h>

namespace shapeworks {
namespace {
const static float epsilon = 1e-6;
}

namespace {
template<class FROM, class TO>
inline TO convert(FROM& value) {
  TO converted;
  converted[0] = value[0];
  converted[1] = value[1];
  converted[2] = value[2];
  return converted;
}
} // namespace

template<class T>
inline std::string PrintValue(T value) {
  return "(" + std::to_string(value[0]) + ", " + std::to_string(value[1]) + ", " + std::to_string(value[2]) + ")";
}

using vec3 = Eigen::Vector3d;
using NormalType = Surface::NormalType;
using VectorType = Surface::VectorType;
using PointType = Surface::PointType;
using GradNType = Surface::GradNType;

//------------------------------------------------------------------------------
//! From vtkTriangle::EvaluatePosition
int evaluate_position(const double x[3],
                      double closestPoint[3],
                      int& subId,
                      double pcoords[3],
                      double& dist2,
                      double weights[],
                      double pt3[3],
                      double pt1[3],
                      double pt2[3]) {
  int i, j;
  double n[3], fabsn;
  double rhs[2], c1[2], c2[2];
  double det;
  double maxComponent;
  int idx = 0, indices[2];
  double dist2Point, dist2Line1, dist2Line2;
  double* closest, closestPoint1[3], closestPoint2[3], cp[3];

  subId = 0;
  pcoords[2] = 0.0;

  // Get normal for triangle, only the normal direction is needed, i.e. the
  // normal need not be normalized (unit length)
  vtkTriangle::ComputeNormalDirection(pt1, pt2, pt3, n);

  // Project point to plane
  vtkPlane::GeneralizedProjectPoint(x, pt1, n, cp);

  // Construct matrices.  Since we have over determined system, need to find
  // which 2 out of 3 equations to use to develop equations. (Any 2 should
  // work since we've projected point to plane.)
  //
  for (maxComponent = 0.0, i = 0; i < 3; i++) {
    // trying to avoid an expensive call to fabs()
    if (n[i] < 0) {
      fabsn = -n[i];
    } else {
      fabsn = n[i];
    }
    if (fabsn > maxComponent) {
      maxComponent = fabsn;
      idx = i;
    }
  }
  for (j = 0, i = 0; i < 3; i++) {
    if (i != idx) {
      indices[j++] = i;
    }
  }

  for (i = 0; i < 2; i++) {
    rhs[i] = cp[indices[i]] - pt3[indices[i]];
    c1[i] = pt1[indices[i]] - pt3[indices[i]];
    c2[i] = pt2[indices[i]] - pt3[indices[i]];
  }

  if ((det = vtkMath::Determinant2x2(c1, c2)) == 0.0) {
    pcoords[0] = pcoords[1] = 0.0;
    return -1;
  }

  pcoords[0] = vtkMath::Determinant2x2(rhs, c2) / det;
  pcoords[1] = vtkMath::Determinant2x2(c1, rhs) / det;

  // Okay, now find closest point to element
  //
  weights[0] = 1 - (pcoords[0] + pcoords[1]);
  weights[1] = pcoords[0];
  weights[2] = pcoords[1];

  if (weights[0] >= 0.0 && weights[0] <= 1.0 && weights[1] >= 0.0 && weights[1] <= 1.0 &&
    weights[2] >= 0.0 && weights[2] <= 1.0) {
    // projection distance
    if (closestPoint) {
      dist2 = vtkMath::Distance2BetweenPoints(cp, x);
      closestPoint[0] = cp[0];
      closestPoint[1] = cp[1];
      closestPoint[2] = cp[2];
    }
    return 1;
  } else {
    double t;
    if (closestPoint) {
      if (weights[1] < 0.0 && weights[2] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt3);
        dist2Line1 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt3, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt3;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[2] < 0.0 && weights[0] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt1);
        dist2Line1 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt1;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[1] < 0.0 && weights[0] < 0.0) {
        dist2Point = vtkMath::Distance2BetweenPoints(x, pt2);
        dist2Line1 = vtkLine::DistanceToLine(x, pt2, pt3, t, closestPoint1);
        dist2Line2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint2);
        if (dist2Point < dist2Line1) {
          dist2 = dist2Point;
          closest = pt2;
        } else {
          dist2 = dist2Line1;
          closest = closestPoint1;
        }
        if (dist2Line2 < dist2) {
          dist2 = dist2Line2;
          closest = closestPoint2;
        }
        for (i = 0; i < 3; i++) {
          closestPoint[i] = closest[i];
        }
      } else if (weights[0] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt1, pt2, t, closestPoint);
      } else if (weights[1] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt2, pt3, t, closestPoint);
      } else if (weights[2] < 0.0) {
        dist2 = vtkLine::DistanceToLine(x, pt1, pt3, t, closestPoint);
      }
    }
    return 0;
  }
}

//---------------------------------------------------------------------------
Surface::Surface(vtkSmartPointer<vtkPolyData> poly_data,
                 bool is_geodesics_enabled,
                 size_t geodesics_cache_size_multiplier) {
  original_mesh_ = poly_data;
  vtkSmartPointer<vtkTriangleFilter> triangle_filter = vtkSmartPointer<vtkTriangleFilter>::New();
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

  poly_data_ = normals->GetOutput();
  poly_data_->BuildCells();
  poly_data_->BuildLinks();

  vtkSmartPointer<vtkGenericCell> cell = vtkSmartPointer<vtkGenericCell>::New();

  triangles_.reserve(poly_data_->GetNumberOfCells());
  for (int i = 0; i < poly_data_->GetNumberOfCells(); i++) {
    poly_data_->GetCell(i, cell);
    if (cell->GetNumberOfPoints() != 3) {
      throw std::runtime_error("Mesh input was not triangular");
    }

    vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
    triangle->GetPointIds()->SetNumberOfIds(3);
    triangle->GetPointIds()->SetId(0, cell->GetPointId(0));
    triangle->GetPointIds()->SetId(1, cell->GetPointId(1));
    triangle->GetPointIds()->SetId(2, cell->GetPointId(2));
    triangle->GetPoints()->SetPoint(0, cell->GetPoints()->GetPoint(0));
    triangle->GetPoints()->SetPoint(1, cell->GetPoints()->GetPoint(1));
    triangle->GetPoints()->SetPoint(2, cell->GetPoints()->GetPoint(2));

    triangles_.push_back(Triangle(cell->GetPointId(0), cell->GetPointId(1), cell->GetPointId(2)));
  }

  cell_locator_ = vtkSmartPointer<vtkCellLocator>::New();
  cell_locator_->SetCacheCellBounds(true);
  cell_locator_->SetDataSet(poly_data_);
  cell_locator_->BuildLocator();

  compute_mesh_bounds();

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  get_igl_mesh(V, F);

  get_igl_mesh(vertices_, faces_);
  compute_grad_normals(V, F);

  is_geodesics_enabled_ = is_geodesics_enabled;
  if (is_geodesics_enabled_) {
    if (geodesics_cache_size_multiplier == 0) {
      // this is heuristic that gives a good trade off between memory usage and performance
      geodesics_cache_size_multiplier = 120;
    }

    // the caller provides how many times the number of triangles entries should be stored in cache
    geo_max_cache_entries_ = geodesics_cache_size_multiplier * triangles_.size();
    precompute_geodesics(V, F);
  }
}

//---------------------------------------------------------------------------
double Surface::compute_distance(const PointType& pt_a,
                                 int idx_a,
                                 const PointType& pt_b,
                                 int idx_b,
                                 VectorType* out_grad) const {
  // return euclidean if geodesics are disabled
  if (!is_geodesics_enabled_) {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = pt_a[i] - pt_b[i];
      }
    }
    return pt_a.EuclideanDistanceTo(pt_b);
  }

  int face_a, face_b;
  vec3 bary_a, bary_b;
  fetch_and_cache_first_point(pt_a, idx_a, face_a, bary_a);

  // Find the triangle for the point b
  // (some initial experiments at caching these like we do for point_a proved to be unfruitful. no significant perf
  // gain)
  face_b = compute_face_and_weights(pt_b, idx_b, bary_b);

  // The geodesics(and more importantly, its gradient) are very inaccurate if both the points are on the
  // same face, or share an edge. In this case, just return the euclidean distance
  if (face_a == face_b || are_faces_in_k_ring(face_a, face_b)) {
    if (out_grad != nullptr) {
      for (int i = 0; i < DIMENSION; i++) {
        (*out_grad)[i] = pt_a[i] - pt_b[i];
      }
    }
    return pt_a.EuclideanDistanceTo(pt_b);
  }

  // Ensure we have geodesics available in the cache. This will resize the cache to fit face_b or max_dist, whichever
  // is greater. We do this pre-emptively since GeodesicsFromTriangleToTriangle would pull geodesics to every point
  // into the cache if face_b is not found. 1.5 is an heuristic to pull in a little more than we need
  if (!geo_dist_cache_[face_a].has_entry(face_b)) {
    const double max_dist =
        idx_a >= 0 ? particle_neighboorhood_[idx_a] * 1.5 : std::numeric_limits<double>::infinity();
    geodesics_from_triangle(face_a, max_dist, face_b);
  }

  // Compute geodesic distance via barycentric approximation
  // Geometric Correspondence for Ensembles of Nonregular Shapes, Datar et al
  // https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3346950/
  const Eigen::Matrix3d geo_from_a = geodesics_from_triangle_to_triangle(face_a, face_b);
  const Eigen::Vector3d geo_to_b = geo_from_a * bary_b;
  const double geo_dist = bary_a.dot(geo_to_b);

  // Check if gradient is needed
  if (out_grad == nullptr) {
    return geo_dist;
  }

  // Compute gradient of geodesics
  const auto& G = face_grad_[face_a];
  Eigen::Vector3d out_grad_eigen = (G * geo_to_b).rowwise().sum();
  out_grad_eigen *= geo_dist / out_grad_eigen.norm();

  // Copy to VNL data structure
  (*out_grad)[0] = out_grad_eigen[0];
  (*out_grad)[1] = out_grad_eigen[1];
  (*out_grad)[2] = out_grad_eigen[2];

  return geo_dist;
}

//---------------------------------------------------------------------------
// Fetches face/triangle index and barycentric coordinates of point in face,
// caching or retrieving results from cache if already cached.
void Surface::fetch_and_cache_first_point(const PointType pt_a, int idx_a, int& face_a, vec3& bary_a) const {
  if (geo_lq_cached_ && pt_a == geo_lq_pt_a_) {
    face_a = geo_lq_face_;
    bary_a = geo_lq_bary_;
  } else {
    face_a = compute_face_and_weights(pt_a, idx_a, bary_a);

    geo_lq_cached_ = true;
    geo_lq_face_ = face_a;
    geo_lq_bary_ = bary_a;
    geo_lq_pt_a_ = pt_a;
  }
}

//---------------------------------------------------------------------------
bool Surface::is_within_distance(const PointType& pt_a,
                                 int idx_a,
                                 const PointType& pt_b,
                                 int idx_b,
                                 double test_dist,
                                 double& dist) const {
  if (!is_geodesics_enabled_) {
    dist = pt_a.EuclideanDistanceTo(pt_b);
    return dist < test_dist;
  }

  if (idx_a != -1) {
    if (particle_neighboorhood_.size() <= idx_a) {
      particle_neighboorhood_.resize(idx_a + 1);
    }
    particle_neighboorhood_[idx_a] = test_dist;
  }

  int face_a, face_b;
  vec3 bary_a, bary_b;
  fetch_and_cache_first_point(pt_a, idx_a, face_a, bary_a);

  // Find the triangle for the point b
  face_b = compute_face_and_weights(pt_b, idx_b, bary_b);
  if (face_a == face_b || are_faces_in_k_ring(face_a, face_b)) {
    dist = pt_a.EuclideanDistanceTo(pt_b);
    return dist < test_dist;
  }

  const auto vb0 = triangles_[face_b].a_;
  const auto vb1 = triangles_[face_b].b_;
  const auto vb2 = triangles_[face_b].c_;

  // 1.5 is an heuristic to pull in a little more than we need
  const auto& geo_entry = geodesics_from_triangle(face_a, test_dist * 1.5);

  if (!geo_entry.is_full_mode()) {
    const auto& data = geo_entry.data_partial;
    const auto find_v0 = data.find(vb0);
    const auto find_v1 = data.find(vb1);
    const auto find_v2 = data.find(vb2);
    if (find_v0 == data.end() && find_v1 == data.end() && find_v2 == data.end()) {
      return false;
    }
    // quick test to see if we _really_ need the geodesic.
    double min_found = std::numeric_limits<double>::infinity();
    if (find_v0 != data.end()) {
      min_found = std::min(min_found, find_v0->second.minCoeff());
    }
    if (find_v1 != data.end()) {
      min_found = std::min(min_found, find_v1->second.minCoeff());
    }
    if (find_v2 != data.end()) {
      min_found = std::min(min_found, find_v2->second.minCoeff());
    }
    if (min_found > test_dist) {
      return false;
    }
  }

  const Eigen::Matrix3d geo_from_a = geodesics_from_triangle_to_triangle(face_a, face_b);
  const Eigen::Vector3d geo_to_b = geo_from_a * bary_b;
  dist = bary_a.dot(geo_to_b);
  return dist < test_dist;
}

//---------------------------------------------------------------------------
PointType Surface::geodesic_walk(PointType p, int idx, VectorType vector) const {
  vec3 point(p[0], p[1], p[2]);
  double closest_point[3];
  int face_index = get_triangle_for_point(point.data(), idx, closest_point);
  point = convert<double[3], vec3>(closest_point);

  Eigen::Vector3d vector_eigen = convert<VectorType, Eigen::Vector3d>(vector);
  Eigen::Vector3d projected_vector = project_vector_to_face(get_face_normal(face_index), vector_eigen);

  int ending_face = -1;

  Eigen::Vector3d new_point = geodesic_walk_on_face(point, projected_vector, face_index, ending_face);

  PointType new_point_pt = convert<Eigen::Vector3d, PointType>(new_point);

  // update cache
  if (idx >= 0 && ending_face >= 0) {
    if (idx >= particle_triangles_.size()) {
      particle_triangles_.resize(idx + 1, -1);
    }

    particle_triangles_[idx] = ending_face;
    geo_lq_cached_ = false;

    calculate_normal_at_point(new_point_pt, idx);
  }

  return new_point_pt;

  /*
   * Alternate, just snap to mesh
   *
  pointa[0] += vector[0];
  pointa[1] += vector[1];
  pointa[2] += vector[2];

  return SnapToMesh(pointa, idx);
   */
}

//---------------------------------------------------------------------------
VectorType Surface::project_vector_to_surface_tangent(const PointType& pointa, int idx, VectorType& vector) const {
  double point[3];
  point[0] = pointa[0];
  point[1] = pointa[1];
  point[2] = pointa[2];
  double closest_point[3];

  int faceIndex = get_triangle_for_point(point, idx, closest_point);

  double* normal = poly_data_->GetCellData()->GetNormals()->GetTuple(faceIndex);

  Eigen::Vector3d vec_normal = convert<double*, vec3>(normal);
  Eigen::Vector3d vec_vector = convert<VectorType&, vec3>(vector);

  Eigen::Vector3d result = project_vector_to_face(vec_normal, vec_vector);
  VectorType resultvnl(result[0], result[1], result[2]);
  return resultvnl;
}

//---------------------------------------------------------------------------
NormalType Surface::sample_normal_at_point(PointType p, int idx) const {
  // if the particle is not in the cache or it has changed position, we must recompute
  if (idx < 0 || idx >= particle_normals_.size() || p != particle_positions_[idx]) {
    return calculate_normal_at_point(p, idx);
  }
  return particle_normals_[idx];
}

//---------------------------------------------------------------------------
GradNType Surface::sample_gradient_normal_at_point(PointType p, int idx) const {
  vec3 weights;
  int face_index = compute_face_and_weights(p, idx, weights);

  GradNType weighted_grad_normal = GradNType(0.0);

  for (int i = 0; i < 3; i++) {
    auto id = triangles_[face_index].get_point(i);
    GradNType grad_normal = grad_normals_[id];
    grad_normal *= weights[i];
    weighted_grad_normal += grad_normal;
  }
  return weighted_grad_normal;
}

//---------------------------------------------------------------------------
PointType Surface::snap_to_mesh(PointType pointa, int idx) const {
  Surface::PointType out;
  get_triangle_for_point(pointa.GetDataPointer(), idx, out.GetDataPointer());
  return out;
}

//---------------------------------------------------------------------------
PointType Surface::get_point_on_mesh() const {
  PointType p = get_mesh_upper_bound();
  p = snap_to_mesh(p, -1);
  return p;
}

//---------------------------------------------------------------------------
int Surface::get_triangle_for_point(const double pt[3], int idx, double closest_point[3]) const {
  // given a guess, just check whether it is still valid.
  if (idx >= 0) {
    // ensure that the cache has enough elements. this will never be resized to more than the number of particles,
    if (idx >= particle_triangles_.size()) {
      particle_triangles_.resize(idx + 1, -1);
    }

    const int guess = particle_triangles_[idx];

    if (guess != -1 && is_in_triangle(pt, guess)) {
      closest_point[0] = pt[0];
      closest_point[1] = pt[1];
      closest_point[2] = pt[2];
      return guess;
    }
  }

  // double closest_point[3];//the coordinates of the closest point will be returned here
  double closest_point_dist2; // the squared distance to the closest point will be returned here
  vtkIdType cell_id; // the cell id of the cell containing the closest point will be returned here
  int sub_id; // this is rarely used (in triangle strips only, I believe)

  cell_locator_->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

  if (idx >= 0) {
    // update cache, no need to check size as it was already checked above
    particle_triangles_[idx] = cell_id;
  }

  assert(cell_id >= 0);
  return cell_id;
}

//---------------------------------------------------------------------------
Eigen::Vector3d
Surface::project_vector_to_face(const Eigen::Vector3d& normal, const Eigen::Vector3d& vector) const {
  auto old_mag = vector.norm();

  Eigen::Vector3d new_vector = vector - normal * normal.dot(vector);

  /// Old behavior is to not rescale it, uncomment for the old behavior
  // return new_vector;

  auto new_mag = new_vector.norm();
  double ratio = old_mag / new_mag;

  new_vector *= ratio;

  return new_vector;
}

//---------------------------------------------------------------------------
void Surface::compute_mesh_bounds() {
  double buffer = 5.0;
  double bounds[6];
  poly_data_->GetBounds(bounds);
  mesh_lower_bound_[0] = bounds[0] - buffer;
  mesh_lower_bound_[1] = bounds[2] - buffer;
  mesh_lower_bound_[2] = bounds[4] - buffer;
  mesh_upper_bound_[0] = bounds[1] + buffer;
  mesh_upper_bound_[1] = bounds[3] + buffer;
  mesh_upper_bound_[2] = bounds[5] + buffer;
}

//---------------------------------------------------------------------------
void Surface::compute_grad_normals(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {
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
  Eigen::MatrixXd gn_per_vertex(n_verts, 9);
  gn_per_vertex.setZero();
  Eigen::MatrixXd a_per_face;
  igl::doublearea(V, F, a_per_face);
  Eigen::VectorXd a_per_vertex(n_verts);
  a_per_vertex.setZero();
  // scatter the per-face values
  for (int i = 0; i < n_faces; i++) {
    for (int j = 0; j < 3; j++) {
      gn_per_vertex.row(F(i, j)) += a_per_face(i, 0) * GN.row(i);
      a_per_vertex(F(i, j)) += a_per_face(i, 0);
    }
  }
  for (int i = 0; i < n_verts; i++) {
    if (a_per_vertex(i) != 0.0) {
      gn_per_vertex.row(i) /= a_per_vertex(i);
    }
  }

  // Copy back to VNL data structure
  grad_normals_.resize(n_verts);
  for (int j = 0; j < n_verts; j++) {
    for (int i = 0; i < 3; i++) {
      grad_normals_[j].set(i, 0, gn_per_vertex(j, i * 3 + 0));
      grad_normals_[j].set(i, 1, gn_per_vertex(j, i * 3 + 1));
      grad_normals_[j].set(i, 2, gn_per_vertex(j, i * 3 + 2));
    }
  }
}

//---------------------------------------------------------------------------
bool Surface::is_in_triangle(const double* pt, int face_index) const {
  double dist2;
  Eigen::Vector3d bary;

  Eigen::Vector3d pt_eigen(pt[0], pt[1], pt[2]);
  int ret = compute_barycentric_coordinates(pt_eigen, face_index, dist2, bary);

  if (ret && dist2 < epsilon) {
    bool bary_check = ((bary[0] >= -epsilon) && (bary[0] <= 1 + epsilon)) &&
        ((bary[1] >= -epsilon) && (bary[1] <= 1 + epsilon)) &&
        ((bary[2] >= -epsilon) && (bary[2] <= 1 + epsilon));
    return bary_check;
  }
  return false;
}

//---------------------------------------------------------------------------
Eigen::Vector3d Surface::compute_barycentric_coordinates(const Eigen::Vector3d& pt, int face) const {
  Eigen::Vector3d bary;
  double dist2;
  compute_barycentric_coordinates(pt, face, dist2, bary);
  return bary;
}

//---------------------------------------------------------------------------
int Surface::compute_barycentric_coordinates(const Eigen::Vector3d& pt,
                                             int face,
                                             double& dist2,
                                             Eigen::Vector3d& bary) const {
  double pt1[3], pt2[3], pt3[3];
  double closest[3];
  int sub_id;
  double pcoords[3];

  poly_data_->GetPoint(triangles_[face].a_, pt1);
  poly_data_->GetPoint(triangles_[face].b_, pt2);
  poly_data_->GetPoint(triangles_[face].c_, pt3);

  int rc = evaluate_position(pt.data(), closest, sub_id, pcoords, dist2, bary.data(), pt1, pt2, pt3);
  return rc;
}

//---------------------------------------------------------------------------
const Eigen::Vector3d Surface::get_face_normal(int face_index) const {
  auto normals = poly_data_->GetCellData()->GetNormals();
  double* normal = normals->GetTuple(face_index);
  Eigen::Vector3d n(normal[0], normal[1], normal[2]);
  return n;
}

//---------------------------------------------------------------------------
Eigen::Vector3d Surface::geodesic_walk_on_face(Eigen::Vector3d point_a,
                                               Eigen::Vector3d projected_vector,
                                               int face_index,
                                               int& ending_face) const {
  int current_face = face_index;
  Eigen::Vector3d current_point = point_a;
  Eigen::Vector3d remaining_vector = projected_vector;
  double minimum_update = 0.0000000001;
  double barycentric_epsilon = 0.0001;
  std::vector<int> faces_traversed;
  // std::vector<vec3> positions;
  int prev_face = current_face;
  while (remaining_vector.norm() > minimum_update && current_face != -1) {
    faces_traversed.push_back(current_face);
    vec3 current_bary =
        compute_barycentric_coordinates(vec3(current_point[0], current_point[1], current_point[2]), current_face);
    // std::cerr << "Current Bary: " << PrintValue<Eigen::Vector3d>(currentBary) << "\n";

    Eigen::Vector3d target_point = current_point + remaining_vector;
    // positions.push_back(currentPoint);
    vec3 target_bary = compute_barycentric_coordinates(vec3(target_point[0], target_point[1], target_point[2]),
                                                       current_face);
    // std::cerr << "Target Bary: " << PrintValue<Eigen::Vector3d>(targetBary) << "\n";

    if (faces_traversed.size() >= 3 &&
      faces_traversed[faces_traversed.size() - 1] == faces_traversed[faces_traversed.size() - 3]) {
      // When at the intersection of two faces while also being at the edge of the mesh, the edge-sliding will keep
      // alternating between the two faces without actually going anywhere since it is at a corner in the mesh.
      // std::cerr << "exiting due to face repetition\n";
      break;
    }

    if (faces_traversed.size() > 1000) {
      std::cerr << "Warning, more than 1000 faces traversed\n";
      for (int i = 0; i < faces_traversed.size(); i++) {
        std::cerr << faces_traversed[i] << ", ";
      }
      // std::cerr << "\nPositions: ";
      // for (int i = 0; i < positions.size(); i++) {
      //     std::cerr << "(" << positions[i][0] << "," << positions[i][1] << "," << positions[i][2]
      //                 << ") ";
      //   }
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

    if
    (target_bary[0]
      +
      barycentric_epsilon
      >=
      0
      &&
      target_bary[1]
      +
      barycentric_epsilon
      >=
      0
      &&
      target_bary[2]
      +
      barycentric_epsilon
      >=
      0
      &&
      target_bary[0]
      -
      barycentric_epsilon
      <=
      1
      &&
      target_bary[1]
      -
      barycentric_epsilon
      <=
      1
      &&
      target_bary[2]
      -
      barycentric_epsilon
      <=
      1
    ) {
      current_point = target_point;
      break;
    }

    std::vector<int> negative_vertices;
    for
    (
      int i = 0;
      i < 3;
      i
      ++
    ) {
      if (target_bary[i] < 0) {
        negative_vertices.push_back(i);
      }
    }

    if
    (negative_vertices
      .
      size()
      ==
      0
      ||
      negative_vertices
      .
      size()
      >
      2
    ) {
      std::cerr << "ERROR: invalid number of negative vertices. Point is not on surface.\n";
      break;
    }
    int negativeEdge = negative_vertices[0];
    Eigen::Vector3d intersect = get_barycentric_intersection(current_bary, target_bary, current_face, negativeEdge);

    // When more than 1 negative barycentric coordinate, compute both intersections and take the closest one.
    if
    (negative_vertices
      .
      size()
      ==
      2
    ) {
      int negativeEdge1 = negative_vertices[1];
      Eigen::Vector3d intersect1 = get_barycentric_intersection(current_bary, target_bary, current_face, negativeEdge1);

      double sqr_length0 = (intersect - current_point).squaredNorm();
      double sqr_length1 = (intersect1 - current_point).squaredNorm();

      if (sqr_length0 < sqr_length1) {
        intersect = intersect;
        negativeEdge = negativeEdge;
      } else {
        intersect = intersect1;
        negativeEdge = negativeEdge1;
      }
    }

    Eigen::Vector3d remaining = target_point - intersect;
    int next_face = get_across_edge(current_face, negativeEdge);
    if
    (next_face
      ==
      -
      1
    ) {
      next_face = slide_along_edge(intersect, remaining, current_face, negativeEdge);
    }
    remaining_vector = remaining;
    if
    (next_face
      !=
      -
      1
    ) {
      remaining_vector = rotate_vector_to_face(get_face_normal(current_face),
                                               get_face_normal(next_face),
                                               remaining_vector);
    }
    current_point = intersect;
    current_face = next_face;
    if
    (current_face
      !=
      -
      1
    ) {
      prev_face = current_face;
    }
  }

  if
  (current_face
    !=
    -
    1
  ) {
    prev_face = current_face;
  }

  ending_face = prev_face;
  assert(ending_face != -1);
  return
      current_point;
}

//---------------------------------------------------------------------------
Eigen::Vector3d Surface::get_barycentric_intersection(Eigen::Vector3d start,
                                                      Eigen::Vector3d end,
                                                      int currentFace,
                                                      int edge) const {
  vec3 delta = end - start;
  if (delta[edge] == 0) {
    // If going parallel to the edge, it is allowed to go all the way to the end where it wants to go
    return end;
  }
  double ratio = -start[edge] / delta[edge];
  vec3 intersect = start + delta * ratio;

  auto cell = poly_data_->GetCell(currentFace);

  vec3 inter(0, 0, 0);
  for (int q = 0; q < 3; q++) {
    auto point = cell->GetPoints()->GetPoint(q);
    vec3 p(point[0], point[1], point[2]);
    inter += p * intersect[q];
  }
  return inter;
}

//---------------------------------------------------------------------------
int Surface::get_across_edge(int face_id, int edge_id) const {
  // get the neighbors of the cell
  auto neighbors = vtkSmartPointer<vtkIdList>::New();

  int edge_p1 = triangles_[face_id].get_point(1);
  int edge_p2 = triangles_[face_id].get_point(2);
  if (edge_id == 1) {
    edge_p1 = triangles_[face_id].get_point(2);
    edge_p2 = triangles_[face_id].get_point(0);
  } else if (edge_id == 2) {
    edge_p1 = triangles_[face_id].get_point(0);
    edge_p2 = triangles_[face_id].get_point(1);
  }

  poly_data_->GetCellEdgeNeighbors(face_id, edge_p1, edge_p2, neighbors);

  if (neighbors->GetNumberOfIds() == 0) {
    // This is the boundary edge of an open mesh
    return -1;
  }

  assert(neighbors->GetNumberOfIds() == 1);
  return neighbors->GetId(0);
}

//---------------------------------------------------------------------------
int Surface::slide_along_edge(Eigen::Vector3d& point, Eigen::Vector3d& remaining_vector, int face, int edge) const {
  int indexa = (edge + 1) % 3;
  int indexb = (edge + 2) % 3;
  int vertexindexa = get_face_point_id(face, indexa);
  int vertexindexb = get_face_point_id(face, indexb);
  Eigen::Vector3d pointa = get_vertex_coords(vertexindexa);
  Eigen::Vector3d pointb = get_vertex_coords(vertexindexb);
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
    return get_across_edge(face, towards_edge);
  } else {
    point += projected_vector;
    remaining_vector = Eigen::Vector3d(0, 0, 0);
    return face;
  }
}

//---------------------------------------------------------------------------
int Surface::get_face_point_id(int face, int point_id) const {
  return poly_data_->GetCell(face)->GetPointId(point_id);
}

//---------------------------------------------------------------------------
Eigen::Vector3d Surface::get_vertex_coords(int vertex_id) const {
  double* p = poly_data_->GetPoint(vertex_id);
  return Eigen::Vector3d(p[0], p[1], p[2]);
}

//---------------------------------------------------------------------------
Eigen::Vector3d Surface::rotate_vector_to_face(const Eigen::Vector3d& prev_normal,
                                               const Eigen::Vector3d& next_normal,
                                               const Eigen::Vector3d& vector) const {
  float dotprod = prev_normal.normalized().dot(next_normal.normalized());
  if (dotprod >= 1) {
    return vector;
  }
  float angle = acos(dotprod);
  Eigen::Vector3d rotationAxis = prev_normal.normalized().cross(next_normal.normalized()).normalized();
  Eigen::AngleAxisd transform(angle, rotationAxis);
  Eigen::Vector3d rotated = transform * vector;
  return rotated;
}

//---------------------------------------------------------------------------
NormalType Surface::calculate_normal_at_point(PointType p, int idx) const {
  vec3 weights;
  int face_index = compute_face_and_weights(p, idx, weights);

  NormalType weighted_normal(0, 0, 0);

  for (int i = 0; i < 3; i++) {
    auto id = triangles_[face_index].get_point(i);
    double* normal = poly_data_->GetPointData()->GetNormals()->GetTuple(id);
    weighted_normal[0] = weighted_normal[0] + normal[0] * weights[i];
    weighted_normal[1] = weighted_normal[1] + normal[1] * weights[i];
    weighted_normal[2] = weighted_normal[2] + normal[2] * weights[i];
  }

  if (idx >= 0) {
    // cache
    if (idx >= particle_normals_.size()) {
      particle_normals_.resize(idx + 1);
      particle_positions_.resize(idx + 1);
    }
    particle_positions_[idx] = p;
    particle_normals_[idx] = weighted_normal;
  }

  return weighted_normal;
}

//---------------------------------------------------------------------------
void Surface::invalidate_particle(int idx) {
  assert(idx >= 0); // should always be passed a valid particle
  if (idx >= particle_triangles_.size()) {
    particle_triangles_.resize(idx + 1, -1);
  }
  particle_triangles_[idx] = -1;
  geo_lq_cached_ = false;
}

//---------------------------------------------------------------------------
int Surface::compute_face_and_weights(const PointType& p, int idx, Eigen::Vector3d& weights) const {
  Eigen::Vector3d closest_point;
  int face_index = get_triangle_for_point(p.GetDataPointer(), idx, closest_point.data());
  // TODO: GetTriangleForPoint is usually already computing the weights
  weights = compute_barycentric_coordinates(closest_point, face_index);
  return face_index;
}

//---------------------------------------------------------------------------
void Surface::get_igl_mesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const {
  const int n_verts = poly_data_->GetNumberOfPoints();
  const int n_faces = poly_data_->GetNumberOfCells();

  V.resize(n_verts, 3);
  F.resize(n_faces, 3);

  auto points = poly_data_->GetPoints();
  for (int i = 0; i < n_verts; i++) {
    double p[3];
    points->GetPoint(i, p);
    V(i, 0) = p[0];
    V(i, 1) = p[1];
    V(i, 2) = p[2];
  }
  for (int i = 0; i < n_faces; i++) {
    auto cell = poly_data_->GetCell(i);
    assert(cell->GetNumberOfPoints() == 3);
    F(i, 0) = cell->GetPointId(0);
    F(i, 1) = cell->GetPointId(1);
    F(i, 2) = cell->GetPointId(2);
  }
}

//---------------------------------------------------------------------------
void Surface::precompute_geodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) {
  // Resize cache to correct size
  geo_dist_cache_.resize(F.rows());

  // Compute gradient operator
  Eigen::SparseMatrix<double> G;
  igl::grad(V, F, G);

  // Flatten the gradient operator so we can quickly compute the gradient at a given point
  face_grad_.resize(F.rows());
  size_t n_insertions = 0;
  for (int k = 0; k < G.outerSize(); k++) {
    for (Eigen::SparseMatrix<double>::InnerIterator it(G, k); it; ++it) {
      const double val = it.value();
      const auto r = it.row();
      const auto c = it.col();

      const auto f = r % F.rows();
      const auto axis = r / F.rows();
      for (int i = 0; i < 3; i++) {
        if (F(f, i) == c) {
          face_grad_[f](axis, i) = val;
          n_insertions++;
          break;
        }
      }
    }
  }
  assert(n_insertions == 9 * F.rows());

  // geometry central stuff
  {
    using namespace geometrycentral::surface;
    std::tie(gc_mesh_, gc_geometry_) = makeSurfaceMeshAndGeometry(V, F);
    gc_heatsolver_ = std::make_unique<HeatMethodDistanceSolver>(*gc_geometry_, 1.0, true);
  }

  // compute k-ring
  face_kring_.resize(triangles_.size());
  for (int f = 0; f < triangles_.size(); f++) {
    compute_k_ring(f, kring_, face_kring_[f]);
  }
}

//---------------------------------------------------------------------------
bool Surface::are_faces_in_k_ring(int f_a, int f_b) const {
  return face_kring_[f_a].find(f_b) != face_kring_[f_a].end();
}

//---------------------------------------------------------------------------
const MeshGeoEntry& Surface::geodesics_from_triangle(int f, double max_dist, int req_target_f) const {
  if (geo_cache_size_ >= geo_max_cache_entries_) {
    clear_geodesic_cache();
  }

  auto& entry = geo_dist_cache_[f];
  if (entry.is_full_mode()) {
    return entry;
  }

  if (entry.max_dist >= max_dist) {
    return entry;
  }

  geo_cache_size_ -= entry.data_partial.size();
  entry.data_partial.clear();

  const auto n_verts = poly_data_->GetNumberOfPoints();

  const auto which_vert_of_tri = [&](int tri, int v) {
    if (triangles_[tri].get_point(0) == v) {
      return 0;
    }
    if (triangles_[tri].get_point(1) == v) {
      return 1;
    }
    return 2;
  };

  double new_max_dist = max_dist;
  Eigen::VectorXd dists[3];

  // if any of this triangle's 1-ring is cached in full mode, we can copy that. This allows us to skip
  // some geodesic computations. its a lot trickier to handle all the edge cases to reuse a neighbors's
  // partial mode values, so we don't do that.
  auto incident_cells = vtkSmartPointer<vtkIdList>::New();
  for (int i = 0; i < 3; i++) {
    const int v = triangles_[f].get_point(i);
    poly_data_->GetPointCells(v, incident_cells);
    for (int j = 0; j < incident_cells->GetNumberOfIds(); j++) {
      const int f_j = incident_cells->GetId(j);
      if (f_j == f) {
        continue;
      }
      if (geo_dist_cache_[f_j].is_full_mode()) {
        const auto which = which_vert_of_tri(f_j, v);
        dists[i] = geo_dist_cache_[f_j].data_full[which];
        break;
      }
    }
  }

  // Compute geodesics using heat method
  for (int i = 0; i < 3; i++) {
    if (dists[i].size() != 0) {
      // we have already figured out these geodesics using a neighbor's
      continue;
    }
    // todo switch to zero-copy API when that is available: https://github.com/nmwsharp/geometry-central/issues/77
    const auto v = gc_mesh_->vertex(triangles_[f].get_point(i));
    const auto gc_dists = gc_heatsolver_->computeDistance(v);
    dists[i] = std::move(gc_dists.raw());
  }

  if (max_dist == std::numeric_limits<double>::infinity()) {
    entry.mode = MeshGeoEntry::Full;
    entry.data_full[0] = std::move(dists[0]);
    entry.data_full[1] = std::move(dists[1]);
    entry.data_full[2] = std::move(dists[2]);
    entry.update_max_dist();
    geo_cache_size_ += n_verts;
    return entry;
  }

  if (req_target_f >= 0) {
    for (int i = 0; i < 3; i++) {
      const int req_v = triangles_[req_target_f].get_point(i);
      max_dist = std::max({max_dist, dists[0][req_v], dists[1][req_v], dists[2][req_v]});
    }
  }

  // this is a heuristic chosen: at a certain size, its simpler to just switch to full mode
  const auto switch_to_full_at = n_verts / 4;

  robin_hood::unordered_set<int> needed_points;

  // Figure out which of these we actually need. If a vertex is included, we also pull in all 1-ring incident_cells of
  // that vertex since they may be needed to compute geodesics on triangles incident on that vertex
  for (int i = 0; i < n_verts; i++) {
    const auto& d0 = dists[0][i];
    const auto& d1 = dists[1][i];
    const auto& d2 = dists[2][i];
    if (d0 <= max_dist || d1 <= max_dist || d2 <= max_dist) {
      poly_data_->GetPointCells(i, incident_cells);
      for (int j = 0; j < incident_cells->GetNumberOfIds(); j++) {
        const auto& tri = triangles_[incident_cells->GetId(j)];
        needed_points.insert(tri.get_point(0));
        needed_points.insert(tri.get_point(1));
        needed_points.insert(tri.get_point(2));

        if (needed_points.size() >= switch_to_full_at) {
          break;
        }
      }
    }
    if (needed_points.size() >= switch_to_full_at) {
      break;
    }
  }

  if (needed_points.size() >= switch_to_full_at) {
    entry.mode = MeshGeoEntry::Full;
    entry.data_full[0] = std::move(dists[0]);
    entry.data_full[1] = std::move(dists[1]);
    entry.data_full[2] = std::move(dists[2]);
    entry.update_max_dist();
    geo_cache_size_ += n_verts;
    return entry;
  }

  for (auto v: needed_points) {
    const auto& d0 = dists[0][v];
    const auto& d1 = dists[1][v];
    const auto& d2 = dists[2][v];
    new_max_dist = std::max({new_max_dist, d0, d1, d2});
    entry.data_partial[v] = {d0, d1, d2};
  }

  geo_cache_size_ += entry.data_partial.size();
  entry.max_dist = new_max_dist;
  return entry;
}

//---------------------------------------------------------------------------
const Eigen::Matrix3d Surface::geodesics_from_triangle_to_triangle(int f_a, int f_b) const {
  auto& entry = geo_dist_cache_[f_a];
  const int v0 = triangles_[f_b].get_point(0);
  const int v1 = triangles_[f_b].get_point(1);
  const int v2 = triangles_[f_b].get_point(2);

  if (entry.is_full_mode()) {
    Eigen::Matrix3d result;
    const auto& data = entry.data_full;
    for (int i = 0; i < 3; i++) {
      result(i, 0) = data[i][v0];
      result(i, 1) = data[i][v1];
      result(i, 2) = data[i][v2];
    }
    return result;
  }

  // Partial mode
  const auto& data = entry.data_partial;
  auto find_v0 = data.find(v0);
  auto find_v1 = data.find(v1);
  auto find_v2 = data.find(v2);
  if (find_v0 == data.end() || find_v1 == data.end() || find_v2 == data.end()) {
    // couldn't find geodesics. force the cache to add this item and rerun
    // *1.0001 is a hacky way to ensure that we don't end up infinitely recursing...
    geodesics_from_triangle(f_a, entry.max_dist * 1.0001, f_b);
    return geodesics_from_triangle_to_triangle(f_a, f_b);
  }
  Eigen::Matrix3d result;
  result.col(0) = find_v0->second;
  result.col(1) = find_v1->second;
  result.col(2) = find_v2->second;
  return result;
}

//---------------------------------------------------------------------------
void Surface::clear_geodesic_cache() const {
  const auto n_verts = poly_data_->GetNumberOfPoints();

  // figure out which triangle each particle is on
  robin_hood::unordered_set<int> active_triangles(particle_triangles_.begin(), particle_triangles_.end());
  size_t new_cache_size = 0;

  // clear entries only if no particle is on that triangle. (this is significantly more performant than clearing
  // everything)
  for (int i = 0; i < geo_dist_cache_.size(); i++) {
    auto& entry = geo_dist_cache_[i];
    if (active_triangles.find(i) == active_triangles.end()) {
      entry.clear();
    } else {
      new_cache_size += entry.is_full_mode() ? n_verts : entry.data_partial.size();
    }
  }

  // the cache is not large enough to store the subset that we know is required. we are forced to clear everything
  if (new_cache_size > geo_max_cache_entries_) {
    std::cerr << "Warning: Cache entries too small, clearing everything. Consider increasing cache size if this "
        "happens repeatedly\n";
    for (auto i: active_triangles) {
      if (i >= 0) {
        geo_dist_cache_[i].clear();
      }
    }
    new_cache_size = 0;
  }

  geo_cache_size_ = new_cache_size;
}

//---------------------------------------------------------------------------
void Surface::compute_k_ring(int f, int k, std::unordered_set<int>& ring) const {
  if (k == 0) {
    return;
  }

  auto neighbors = vtkSmartPointer<vtkIdList>::New();
  for (int i = 0; i < 3; i++) {
    const int v = triangles_[f].get_point(i);
    poly_data_->GetPointCells(v, neighbors);
    for (int j = 0; j < neighbors->GetNumberOfIds(); j++) {
      const int f_j = neighbors->GetId(j);
      if (ring.find(f_j) == ring.end()) {
        ring.insert(f_j);
        compute_k_ring(f_j, k - 1, ring);
      }
    }
  }
}
} // namespace shapeworks
