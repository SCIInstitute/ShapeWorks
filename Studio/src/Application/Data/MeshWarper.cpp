#include <Data/MeshWarper.h>

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

  vtkSmartPointer<vtkPolyData> poly_data = output.getVTKMesh();

  double bounds[6];
  poly_data->GetBounds(bounds);

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
  this->vertices_ = MeshUtils::distilVertexInfo(this->reference_mesh_);
  this->faces_ = MeshUtils::distilFaceInfo(this->reference_mesh_);
  this->points_ = Eigen::Map<const Eigen::VectorXd>(
    (double*) this->reference_particles_.data_block(),
    this->reference_particles_.size());
  this->points_.resize(3, this->reference_particles_.size() / 3);
  this->warp_ = MeshUtils::generateWarpMatrix(this->vertices_, this->faces_,
                                              this->points_.transpose());
  this->needs_warp_ = false;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
