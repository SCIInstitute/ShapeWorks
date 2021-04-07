#include <Data/MeshWarper.h>

#include <vtkCellLocator.h>
#include <vtkTriangle.h>

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

  Mesh output = MeshUtils::warpMesh(points.transpose(), this->warp_, this->faces_);
  return output.getVTKMesh();
}

//---------------------------------------------------------------------------
void MeshWarper::set_reference_mesh(vtkSmartPointer<vtkPolyData> reference_mesh,
                                    const vnl_vector<double>& reference_particles)
{
  if (this->reference_mesh_ == reference_mesh) {
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

  // mark that the warp needs to be generated
  this->needs_warp_ = true;
  this->reference_mesh_ = reference_mesh;
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

  this->points_(0,0) = this->points_(0,1);
  this->points_(1,0) = this->points_(1,1);
  this->points_(2,0) = this->points_(2,1);
  this->add_particle_vertices();

  this->vertices_ = MeshUtils::distilVertexInfo(this->reference_mesh_);
  this->faces_ = MeshUtils::distilFaceInfo(this->reference_mesh_);
  this->warp_ = MeshUtils::generateWarpMatrix(this->vertices_, this->faces_,
                                              this->points_.transpose());
  this->needs_warp_ = false;
}

//---------------------------------------------------------------------------
void MeshWarper::add_particle_vertices()
{

  for (int i = 0; i < this->points_.cols(); i++) {
    this->reference_mesh_->BuildLinks();
    this->reference_mesh_->BuildCells();

    auto locator = vtkSmartPointer<vtkCellLocator>::New();
    locator->SetCacheCellBounds(true);
    locator->SetDataSet(this->reference_mesh_);
    locator->BuildLocator();

    double pt[3]{this->points_(0, i), this->points_(1, i), this->points_(2, i)};
    double closest_point[3];//the coordinates of the closest point will be returned here
    double closest_point_dist2; //the squared distance to the closest point will be returned here
    vtkIdType cell_id; //the cell id of the cell containing the closest point will be returned here
    int sub_id; //this is rarely used (in triangle strips only, I believe)
    locator->FindClosestPoint(pt, closest_point, cell_id, sub_id, closest_point_dist2);

    int vertex_id = this->reference_mesh_->GetPoints()->InsertNextPoint(pt);

    vtkCell* cell = this->reference_mesh_->GetCell(cell_id);
    vtkSmartPointer<vtkIdList> list = vtkSmartPointer<vtkIdList>::New();

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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
