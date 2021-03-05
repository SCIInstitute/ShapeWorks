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

double ContourDomain::Distance(const PointType &a, const PointType &b) const {
  return a.EuclideanDistanceTo(b);
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
      std::cout << geodesics_(i, j) << "\n";
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

}
