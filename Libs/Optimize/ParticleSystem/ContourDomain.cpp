#include "ContourDomain.h"
#include <iostream>
#include <fstream>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkDoubleArray.h>


namespace itk{

const double epsilon = 1e-6;

void ContourDomain::SetPolyLine(vtkSmartPointer<vtkPolyData> poly_data) {
  this->m_FixedDomain = false;

  // to avoid mutating the input mesh
  this->poly_data_ = vtkSmartPointer<vtkPolyData>::New();
  this->poly_data_->DeepCopy(poly_data);
  this->poly_data_->BuildCells();
  this->poly_data_->BuildLinks();

  auto cell = vtkSmartPointer<vtkGenericCell>::New();
  for(int i=0; i<this->poly_data_->GetNumberOfCells(); i++) {
    poly_data_->GetCell(i, cell);
    assert(cell->GetNumberOfPoints() == 2);

    auto line = vtkSmartPointer<vtkLine>::New();
    line->GetPointIds()->SetNumberOfIds(2); // is this necessary?
    line->GetPointIds()->SetId(0, cell->GetPointId(0));
    line->GetPointIds()->SetId(1, cell->GetPointId(1));
    line->GetPoints()->SetPoint(0, cell->GetPoints()->GetPoint(0));
    line->GetPoints()->SetPoint(1, cell->GetPoints()->GetPoint(1));

    this->lines_.push_back(line);
  }

  this->cell_locator_ = vtkSmartPointer<vtkCellLocator>::New();
  this->cell_locator_->SetCacheCellBounds(true);
  this->cell_locator_->SetDataSet(this->poly_data_);
  this->cell_locator_->BuildLocator();

  this->ComputeBounds();
  this->ComputeGeodesics(this->poly_data_);
}

vnl_vector_fixed<double, DIMENSION>
ContourDomain::ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, 3> &gradE,
                                             const PointType &p, int idx) const {
  PointType closest_pt;
  double dist;
  const int closest_line = GetLineForPoint(p.GetDataPointer(), idx, dist, closest_pt.GetDataPointer());
  const auto p0_idx = this->lines_[closest_line]->GetPointId(0);
  const auto p1_idx = this->lines_[closest_line]->GetPointId(1);

  const Eigen::Vector3d gradE_eigen{gradE[0], gradE[1], gradE[2]};
  const Eigen::Vector3d line_dir = (this->GetPoint(p1_idx) - this->GetPoint(p0_idx)).normalized();
  const double grad_dot_line_dir = gradE_eigen.dot(line_dir);
  const double grad_mag = gradE_eigen.norm();

  // Particle wants to move perpendicular to the line, this is disallowed.
  if(grad_dot_line_dir == 0.0) {
    return { 0.0, 0.0, 0.0 };
  }

  // Since the particle is on a line, there are only two meaningful directions to take. We choose the one which
  // has a closer direction to the gradient. The result is scaled by the gradient magnitude.
  const double dir = grad_dot_line_dir > 0.0 ? 1.0 : -1.0;

  return {
          line_dir[0]*grad_mag*dir,
          line_dir[1]*grad_mag*dir,
          line_dir[2]*grad_mag*dir
  };
}

bool ContourDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  PointType closest_pt;
  double dist;
  GetLineForPoint(p.GetDataPointer(), idx, dist, closest_pt.GetDataPointer());
  p = closest_pt;
  return true;
}

ContourDomain::PointType ContourDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                               vnl_vector_fixed<double, 3> &update) const {
  const Eigen::Vector3d update_vec { -update[0], -update[1], -update[2] };
  return GeodesicWalk(point, idx, update_vec);
}

ContourDomain::PointType ContourDomain::GeodesicWalk(const PointType& start_pt, int idx, const Eigen::Vector3d& update_vec) const {
  double dist;
  Eigen::Vector3d current_pt;
  int current_line = GetLineForPoint(start_pt.GetDataPointer(), idx, dist, current_pt.data());
  double remaining_update_mag = update_vec.norm();

  const auto p0_idx = this->lines_[current_line]->GetPointId(0);
  const auto p1_idx = this->lines_[current_line]->GetPointId(1);
  const auto p0 = this->GetPoint(p0_idx);
  const auto p1 = this->GetPoint(p1_idx);
  const Eigen::Vector3d line_dir = (p1 - p0).normalized();
  const double update_dot_line_dir = update_vec.dot(line_dir);

  // if the particle wants to travel perpendicular to the line, disallow it.
  if(update_dot_line_dir == 0.0) {
    return start_pt;
  }
  int current_target_idx = update_dot_line_dir > 0.0 ? p1_idx : p0_idx;
  auto neighbors = vtkSmartPointer<vtkIdList>::New();

  while(remaining_update_mag >= 0.0) {

    const auto target_pt = this->GetPoint(current_target_idx);
    const double dist_to_target = (target_pt - current_pt).norm();

    if(dist_to_target > remaining_update_mag) {
      current_pt += (target_pt - current_pt) * remaining_update_mag / dist_to_target;
      break;
    } else {
      current_pt = target_pt;
      remaining_update_mag -= dist_to_target;
    }

    this->poly_data_->GetPointCells(current_target_idx, neighbors);

    assert(neighbors->GetNumberOfIds() <= 2); // todo remove this when ready to handle branches

    // hit a dead end. Stop
    if(neighbors->GetNumberOfIds() == 1) {
      break;
    }

    if(neighbors->GetId(0) == current_line) {
      current_line = neighbors->GetId(1);
    } else {
      current_line = neighbors->GetId(0);
    }

    if(current_target_idx == this->lines_[current_line]->GetPointId(0)) {
      current_target_idx = this->lines_[current_line]->GetPointId(1);
    } else {
      current_target_idx = this->lines_[current_line]->GetPointId(0);
    }
  }

  if(idx >= 0) {
    this->particle_lines_[idx] = current_line;
  }

  PointType res(current_pt.data());
  return res;
}

double ContourDomain::Distance(const PointType &a, int idx_a,
                               const PointType &b, int idx_b,
                               vnl_vector_fixed<double, 3>* out_grad) const {
  Eigen::Vector3d pt_a, pt_b;
  double dist_a, dist_b;
  int line_a, line_b;

  if(idx_a == geo_lq_idx_) {
    pt_a = {a[0], a[1], a[2]};
    line_a = geo_lq_line_;
    dist_a = geo_lq_dist_;
  } else {
    line_a = this->GetLineForPoint(a.GetDataPointer(), idx_a, dist_a, pt_a.data());
    geo_lq_idx_ = idx_a;
    geo_lq_line_ = line_a;
    geo_lq_dist_ = dist_a;
  }

  line_b = this->GetLineForPoint(b.GetDataPointer(), idx_b, dist_b, pt_b.data());

  if(line_a == line_b) {
    if(out_grad != nullptr) {
      for(int i=0; i<DIMENSION; i++) {
        (*out_grad)[i] = a[i] - b[i];
      }
    }
    return a.EuclideanDistanceTo(b);
  }

  // consider 4 paths and choose the shortest
  double shortest_dist = std::numeric_limits<double>::infinity();
  int chosen_dir = 0;

  for(int i=0; i<2; i++) {
    const auto ai_idx = this->lines_[line_a]->GetPointId(i);
    const auto ai = this->GetPoint(ai_idx);
    const double dist_to_ai = (pt_a - ai).norm();

    for(int j=0; j<2; j++) {
      const auto bi_idx = this->lines_[line_b]->GetPointId(j);
      const auto bj = this->GetPoint(bi_idx);
      const double dist_to_bj = (pt_b - bj).norm();

      const double dist = dist_to_ai + geodesics_(ai_idx, bi_idx) + dist_to_bj;
      if(dist < shortest_dist) {
        shortest_dist = dist;
        chosen_dir = i;
      }
    }
  }

  if(out_grad != nullptr) {
    Eigen::Map<Eigen::Vector3d> out_grad_eigen(out_grad->data_block());
    const auto ai_idx = this->lines_[line_a]->GetPointId(chosen_dir);
    // todo is this math correct
    out_grad_eigen = (pt_a - this->GetPoint(ai_idx)).normalized();
    out_grad_eigen *= shortest_dist;
  }
  return shortest_dist;
}

double ContourDomain::SquaredDistance(const PointType &a, int idx_a,
                                      const PointType &b, int idx_b) const {
  const double dist = this->Distance(a, idx_a, b, idx_b);
  return dist*dist;
}

void ContourDomain::ComputeBounds() {
  double buffer = 5.0; // copied from MeshDomain
  double bounds[6];
  this->poly_data_->GetBounds(bounds);
  this->lower_bound_[0] = bounds[0] - buffer;
  this->lower_bound_[1] = bounds[2] - buffer;
  this->lower_bound_[2] = bounds[4] - buffer;
  this->upper_bound_[0] = bounds[1] + buffer;
  this->upper_bound_[1] = bounds[3] + buffer;
  this->upper_bound_[2] = bounds[5] + buffer;
}

void ContourDomain::ComputeGeodesics(vtkSmartPointer<vtkPolyData> poly_data) {
  const auto N = this->NumberOfPoints();

  geodesics_.resize(N, N);
  geodesics_.fill(std::numeric_limits<double>::infinity());

  for(int i=0; i<N; i++) {
    auto dijkstra = vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New();
    dijkstra->SetInputData(poly_data);
    dijkstra->SetStartVertex(i);
    dijkstra->Update();

    auto out = vtkSmartPointer<vtkDoubleArray>::New();
    dijkstra->GetCumulativeWeights(out);

    for(int j=0; j<N; j++) {
      geodesics_(i, j) = out->GetValue(j);
    }
  }
}

int ContourDomain::NumberOfLines() const {
  return poly_data_->GetNumberOfCells();
}

int ContourDomain::NumberOfPoints() const {
  return poly_data_->GetNumberOfPoints();
}

int ContourDomain::GetLineForPoint(const double pt[3], int idx, double& closest_distance, double closest_pt[3]) const {
  if(idx >= 0) {
    if(particle_lines_.size() <= idx) {
      particle_lines_.resize(idx + 1, -1);
    }
    const auto guess = particle_lines_[idx];
    if(guess != -1) {
      const auto weight = ComputeLineCoordinate(pt, guess);
      if(weight >= -epsilon && weight <= epsilon) {
        closest_pt[0] = pt[0];
        closest_pt[1] = pt[1];
        closest_pt[2] = pt[2];
        return guess;
      }
    }
  }

  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id;
  this->cell_locator_->FindClosestPoint(pt, closest_pt, cell_id, sub_id, closest_distance);
  if(idx >= 0) {
    particle_lines_[idx] = cell_id;
  }
  return cell_id;
}

double ContourDomain::ComputeLineCoordinate(const double pt[3], int line) const {
  //todo GetLineForPoint is computing this almost always. just return it there

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[2];
  if(this->lines_[line]->EvaluatePosition(pt, closest, sub_id, pcoords, dist2, weights) == -1) {
    // todo vtk says this is a rare occasion where a numerical error has occurred. return _something_? or crash?
    // see https://vtk.org/doc/nightly/html/classvtkLine.html#a42025caa9c76a44ef740ae1ac3bf6b95
    throw std::runtime_error("numerical error in ContourDomain::ComputeLineCoordinate");
  }
  assert(std::abs(weights[0] + weights[1] - 1.0) < 1e-8);
  return weights[1];
}

Eigen::Vector3d ContourDomain::GetPoint(int id) const {
  const auto pt = this->poly_data_->GetPoint(id);
  return {pt[0], pt[1], pt[2]};
}

void ContourDomain::InvalidateParticlePosition(int idx) const {
  assert(idx >= 0); // should always be passed a valid particle
  if (idx >= particle_lines_.size()) {
    particle_lines_.resize(idx + 1, -1);
  }
  this->particle_lines_[idx] = -1;
  this->geo_lq_idx_ = -1;
}

ContourDomain::PointType ContourDomain::GetPositionAfterSplit(const PointType& pt,
                                                              const vnl_vector_fixed<double, 3>& random,
                                                              double epsilon) const {
  // This relies on all the lines being in a consistent order(either clockwise or anticlockwise). With that, it ensures
  // that given a seed(`random`) all the domains end up returning a consistent split direction
  // The algorithm just looks at the sign of random[0] to decide whether to go clockwise or anticlockwise

  PointType closest_pt;
  double dist;
  const int closest_line = GetLineForPoint(pt.GetDataPointer(), -1, dist, closest_pt.GetDataPointer());
  const auto p0_idx = this->lines_[closest_line]->GetPointId(0);
  const auto p1_idx = this->lines_[closest_line]->GetPointId(1);
  const auto p0 = this->GetPoint(p0_idx);
  const auto p1 = this->GetPoint(p1_idx);

  // pick one of two directions depending on the `random` passed in
  Eigen::Vector3d split_dir = (p1 - p0).normalized();
  if(random[0] < 0.0) {
    split_dir *= -1;
  }

  // divide by 5.0 copied from existing splitting code. unsure of rationale
  split_dir *= epsilon / 5.0;
  vnl_vector_fixed<double, 3> vnl_split_dir(split_dir.data());
  return UpdateParticlePosition(pt, -1, vnl_split_dir); // pass -1 because this really corresponds to an unborn particle
}

int ContourDomain::NumberOfLinesIncidentOnPoint(int i) const {
  auto neighbors = vtkSmartPointer<vtkIdList>::New();
  this->poly_data_->GetPointCells(i, neighbors);
  return neighbors->GetNumberOfIds();
}

}
