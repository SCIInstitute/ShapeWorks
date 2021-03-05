#include "ContourDomain.h"
#include <iostream>
#include <fstream>
#include <vtkDijkstraGraphGeodesicPath.h>
#include <vtkDoubleArray.h>

namespace itk{

void ContourDomain::SetPolyLine(vtkSmartPointer<vtkPolyData> poly_data) {
  this->m_FixedDomain = false;
  this->poly_data_ = poly_data;

  auto cell = vtkSmartPointer<vtkGenericCell>::New();
  for(int i=0; i<poly_data->GetNumberOfCells(); i++) {
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
  this->ComputeGeodesics(poly_data);
}

vnl_vector_fixed<double, DIMENSION>
ContourDomain::ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, 3> &gradE,
                                             const PointType &pos, int idx) const {
  return gradE;
}

bool ContourDomain::ApplyConstraints(PointType &p, int idx, bool dbg) const {
  PointType closest_pt;
  double dist;
  const int closest_line = GetLineForPoint(p.GetDataPointer(), dist, closest_pt.GetDataPointer());
  p = closest_pt;
  return true;
}

ContourDomain::PointType ContourDomain::UpdateParticlePosition(const PointType &point, int idx,
                                                               vnl_vector_fixed<double, 3> &update) const {
  PointType out(point);
  out.GetVnlVector() += update;
  ApplyConstraints(out, idx);
  return out;
}

// todo change APIs for point index, gradients
double ContourDomain::Distance(const PointType &a, const PointType &b) const {
  Eigen::Vector3d pt_a, pt_b;
  double dist_a, dist_b;
  const int line_a = this->GetLineForPoint(a.GetDataPointer(), dist_a, pt_a.data());
  const int line_b = this->GetLineForPoint(b.GetDataPointer(), dist_b, pt_b.data());

  // consider 4(+1) paths and choose the shortest
  double shortest_dist = std::numeric_limits<double>::infinity();

  if(line_a == line_b) {
    // we could probably just return at this point. The rest of the function handles the unlikely situation
    // that taking a round trip around the contour is shorter than walking along this line.
    shortest_dist = (pt_a - pt_b).norm();
  }

  for(int i=0; i<2; i++) {
    const auto ai_idx = this->lines_[line_a]->GetPointId(i);
    const auto ai = this->GetPoint(ai_idx);
    const double dist_to_ai = (pt_a - ai).norm();

    for(int j=0; j<2; j++) {
      const auto bi_idx = this->lines_[line_b]->GetPointId(j);
      const auto bj = this->GetPoint(bi_idx);
      const double dist_to_bj = (pt_b - bj).norm();

      const double dist = dist_to_ai + geodesics_(ai_idx, bi_idx) + dist_to_bj;
      shortest_dist = std::min(shortest_dist, dist);
    }
  }

  return shortest_dist;
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

int ContourDomain::GetLineForPoint(const double pt[3], double& closest_distance, double closest_pt[3]) const {
  //todo particle_line_ cache

  vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
  int sub_id;

  this->cell_locator_->FindClosestPoint(pt, closest_pt, cell_id, sub_id, closest_distance);
  return cell_id;
}

double ContourDomain::ComputeLineCoordinate(const Eigen::Vector3d& pt, int line) const {
  //todo can the vtkCellLocator compute this for us?

  double closest[3];
  int sub_id;
  double pcoords[3];
  double dist2;
  double weights[2];
  if(this->lines_[line]->EvaluatePosition(pt.data(), closest, sub_id, pcoords, dist2, weights) == -1) {
    // todo vtk says this is a rare occasion where a numerical error has occurred. return 0? or crash?
    // see https://vtk.org/doc/nightly/html/classvtkLine.html#a42025caa9c76a44ef740ae1ac3bf6b95
    return 0.0;
  }
  return weights[1];
}

Eigen::Vector3d ContourDomain::GetPoint(int id) const {
  const auto pt = this->poly_data_->GetPoint(id);
  return {pt[0], pt[1], pt[2]};
}

}
