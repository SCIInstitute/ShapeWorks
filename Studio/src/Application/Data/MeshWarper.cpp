#include <Data/MeshWarper.h>

#include <Libs/Mesh/MeshUtils.h>

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

  Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>((double*) particles.data_block(),
                                                             particles.size());

  points.resize(3, particles.size() / 3);

  Mesh output = MeshUtils::warpMesh(points.transpose(), this->warp_, this->faces_);
  return output.get_poly_data();
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
  this->vertices_ = MeshUtils::distilVertexInfo(reference_mesh);
  this->faces_ = MeshUtils::distilFaceInfo(reference_mesh);
  this->points_ = Eigen::Map<const Eigen::VectorXd>((double*) reference_particles.data_block(),
                                                    reference_particles.size());
  this->points_.resize(3, reference_particles.size() / 3);
  this->warp_ = MeshUtils::generateWarpMatrix(this->vertices_, this->faces_,
                                              this->points_.transpose());

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
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
