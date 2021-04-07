#include <Data/MeshWarper.h>

#include <vtkCellLocator.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>

#include <Libs/Mesh/MeshUtils.h>
#include <Data/StudioLog.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
MeshWarper::MeshWarper()
{
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshWarper::build_mesh(const vnl_vector<double>& particles)
{
  if (!this->warp_available_) {
    return nullptr;
  }

  this->check_warp_ready();

  Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>((double*) particles.data_block(),
                                                             particles.size());

  points.resize(3, particles.size() / 3);
  points.transposeInPlace();
  points = this->remove_bad_particles(points);

  Mesh output = MeshUtils::warpMesh(points, this->warp_, this->faces_);

  vtkSmartPointer<vtkPolyData> poly_data = output.getVTKMesh();

  for (int i = 0; i < poly_data->GetNumberOfPoints(); i++) {
    double* p = poly_data->GetPoint(i);
    if (std::isnan(p[0]) || std::isnan(p[1]) || std::isnan(p[2])) {
      this->warp_available_ = false; // failed
      std::cerr << "Reconstruction Failed\n";
      return nullptr;
    }
  }
  return poly_data;
}

//---------------------------------------------------------------------------
void MeshWarper::set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                                    const vnl_vector<double>& reference_particles)
{
  if (this->incoming_reference_mesh_ == reference_mesh) {
    if (this->reference_particles_.size() == reference_particles.size()) {
      bool same = true;
      for (int i = 0; i < reference_particles.size(); i++) {
        if (this->reference_particles_[i] != reference_particles[i]) {
          same = false;
        }
      }
      if (same) {
        // we can skip the processing below, nothing has changed
        return;
      }
    }
  }

  this->incoming_reference_mesh_ = reference_mesh;

  vtkSmartPointer<vtkTriangleFilter> triangle_filter =
    vtkSmartPointer<vtkTriangleFilter>::New();
  triangle_filter->SetInputData(reference_mesh);
  triangle_filter->Update();

  vtkSmartPointer<vtkCleanPolyData> clean = vtkSmartPointer<vtkCleanPolyData>::New();
  clean->ConvertPolysToLinesOff();
  clean->ConvertLinesToPointsOff();
  clean->ConvertStripsToPolysOff();
  clean->PointMergingOn();
  clean->SetInputConnection(triangle_filter->GetOutputPort());
  clean->Update();

  // mark that the warp needs to be generated
  this->needs_warp_ = true;
  this->reference_mesh_ = clean->GetOutput();
  this->reference_particles_ = reference_particles;

  this->warp_available_ = true;
}

//---------------------------------------------------------------------------
bool MeshWarper::get_warp_available()
{
  return this->warp_available_;
}

//---------------------------------------------------------------------------
void MeshWarper::check_warp_ready()
{
  QMutexLocker locker(&this->mutex_);
  if (!this->needs_warp_) {
    // warp already done
    return;
  }

  // perform warp
  this->points_ = Eigen::Map<const Eigen::VectorXd>(
    (double*) this->reference_particles_.data_block(),
    this->reference_particles_.size());
  this->points_.resize(3, this->reference_particles_.size() / 3);
  this->points_.transposeInPlace();

  this->find_good_particles();
  this->points_ = this->remove_bad_particles(this->points_);

  this->add_particle_vertices();

  this->vertices_ = MeshUtils::distilVertexInfo(this->reference_mesh_);
  this->faces_ = MeshUtils::distilFaceInfo(this->reference_mesh_);
  this->warp_ = MeshUtils::generateWarpMatrix(this->vertices_, this->faces_,
                                              this->points_);
  this->needs_warp_ = false;
}

//---------------------------------------------------------------------------
void MeshWarper::add_particle_vertices()
{

  for (int i = 0; i < this->points_.rows(); i++) {
    this->reference_mesh_->BuildLinks();
    this->reference_mesh_->BuildCells();

    auto locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetCacheCellBounds(true);
    locator->SetDataSet(this->reference_mesh_);
    locator->BuildLocator();

    double pt[3]{this->points_(i, 0), this->points_(i, 1), this->points_(i, 2)};
    double closest_point[3];//the coordinates of the closest point will be returned here
    double closest_point_dist2; //the squared distance to the closest point will be returned here
    vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
    int sub_id; //this is rarely used (in triangle strips only, I believe)
    locator->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

    // grab the closest cell
    vtkCell* cell = this->reference_mesh_->GetCell(cell_id);
    vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();

    bool same = false;
    // check that the point is not already on an existing vertex
    for (int i = 0; i < 3; i++) {
      double* p = cell->GetPoints()->GetPoint(i);
      if (p[0] == pt[0] && p[1] == pt[1] && p[2] == pt[2]) {
        same = true;
      }
    }

    if (!same) {
      int vertex_id = this->reference_mesh_->GetPoints()->InsertNextPoint(pt);

      list->SetNumberOfIds(3);
      list->SetId(0, cell->GetPointId(0));
      list->SetId(1, vertex_id);
      list->SetId(2, cell->GetPointId(1));
      this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

      list->SetId(0, cell->GetPointId(1));
      list->SetId(1, vertex_id);
      list->SetId(2, cell->GetPointId(2));
      this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

      list->SetId(0, cell->GetPointId(0));
      list->SetId(1, vertex_id);
      list->SetId(2, cell->GetPointId(2));
      this->reference_mesh_->InsertNextCell(VTK_TRIANGLE, list);

      this->reference_mesh_->DeleteCell(cell_id);
      this->reference_mesh_->RemoveDeletedCells();
    }
  }

}

//---------------------------------------------------------------------------
void MeshWarper::find_good_particles()
{
  std::set<int> set;  // initially store in set to avoid duplicates
  for (int i = 0; i < this->points_.rows(); i++) {
    for (int j = i + 1; j < this->points_.rows(); j++) {
      double p1[3]{this->points_(i, 0), this->points_(i, 1), this->points_(i, 2)};
      double p2[3]{this->points_(j, 0), this->points_(j, 1), this->points_(j, 2)};
      if (p1[0] == p2[0] && p1[1] == p2[1] && p1[2] == p2[2]) {
        set.insert(i);
        set.insert(j);
      }
    }
  }

  this->good_particles_.clear();
  for (int i = 0; i < this->points_.rows(); i++) {
    if (set.find(i) == set.end()) {
      this->good_particles_.push_back(i);
    }
  }
}

//---------------------------------------------------------------------------
Eigen::MatrixXd MeshWarper::remove_bad_particles(const Eigen::MatrixXd& particles)
{
  Eigen::MatrixXd new_particles(this->good_particles_.size(), 3);
  for (int i = 0; i < this->good_particles_.size(); i++) {
    int id = this->good_particles_[i];
    new_particles(i, 0) = particles(id, 0);
    new_particles(i, 1) = particles(id, 1);
    new_particles(i, 2) = particles(id, 2);
  }

  return new_particles;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
