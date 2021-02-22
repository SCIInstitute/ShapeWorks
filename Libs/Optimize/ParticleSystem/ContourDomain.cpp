#include "ContourDomain.h"
#include <iostream>
#include <fstream>

namespace itk{

void ContourDomain::LoadFromFile(const std::string& filepath) {
  std::ifstream in_file(filepath);

  this->m_FixedDomain = false;
  this->is_closed_ = true; // TODO handle this

  size_t n_points; in_file >> n_points;
  points.resize(n_points, 3);

  // todo figure out a better file format don't roll own
  for(int i=0; i<n_points; i++) {
    in_file >> points(i,0);
    in_file >> points(i,1);
    in_file >> points(i,2);
  }

  this->ComputeBounds();
}

vnl_vector_fixed<double, DIMENSION>
ContourDomain::ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, 3> &gradE,
                                             const PointType &pos, int idx) const {
  return gradE;
}

bool ContourDomain::ApplyConstraints(ContourDomain::PointType &p, int idx, bool dbg) const {
  Eigen::Vector3d closest_pt;
  Eigen::Vector3d pt_eigen(p[0], p[1], p[2]);
  double dist;
  const int closest_line = ClosestLine(pt_eigen, dist, closest_pt);

  p.GetVnlVector().set(closest_pt.data());
  return true;
}

ContourDomain::PointType ContourDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                               vnl_vector_fixed<double, 3> &update) const {
  PointType out(point);
  out.GetVnlVector() += update;
  ApplyConstraints(out, idx);
  return out;
}

double ContourDomain::Distance(const PointType &a, const PointType &b) const {
  return a.EuclideanDistanceTo(b);
}

void ContourDomain::ComputeBounds() {
  const Eigen::Vector3d lb = points.colwise().minCoeff();
  this->lower_bound_.GetVnlVector().set(lb.data());

  const Eigen::Vector3d ub = points.colwise().maxCoeff();
  this->upper_bound_.GetVnlVector().set(ub.data());
}

int ContourDomain::NumberOfLines() const {
  if(is_closed_) {
    return points.rows() - 1;
  }
  return points.rows();
}

double ContourDomain::DistanceToLine(const Eigen::Vector3d &pt, int line_idx, Eigen::Vector3d& closest_pt) const {
  // Reference: http://geomalgorithms.com/a02-_lines.html
  const Eigen::Vector3d p0 = points.row(line_idx);
  const Eigen::Vector3d p1 = points.row((line_idx+1) % points.rows());
  const Eigen::Vector3d v = p1 - p0;
  const Eigen::Vector3d w = pt - p0;

  const double c1 = w.dot(v);
  if(c1 <= 0.0) {
    closest_pt = p0;
    return (pt - p0).norm();
  }

  const double c2 = v.dot(v);
  if(c2 <= c1) {
    closest_pt = p1;
    return (pt - p1).norm();
  }

  const double b = c1 / c2;
  const Eigen::Vector3d pb = p0 + b * v;
  closest_pt = pb;
  return (pt - pb).norm();
}

int ContourDomain::ClosestLine(const Eigen::Vector3d &pt, double& closest_distance, Eigen::Vector3d& closest_pt) const {
  const auto n_lines = NumberOfLines();

  closest_distance = DistanceToLine(pt, 0, closest_pt);
  int closest_idx = 0;

  //todo faster search
  for(int i=1; i<n_lines; i++) {
    Eigen::Vector3d i_closest_pt;
    double dist = DistanceToLine(pt, i, i_closest_pt);
    if(dist < closest_distance) {
      closest_distance = dist;
      closest_pt = i_closest_pt;
      closest_idx = i;
    }
  }

  return closest_idx;
}

}
