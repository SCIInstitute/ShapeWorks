#include <Data/StudioParticles.h>
#include <vtkTransform.h>

#include <cassert>

namespace shapeworks {

//---------------------------------------------------------------------------
StudioParticles::StudioParticles() {}

//---------------------------------------------------------------------------
void StudioParticles::set_local_particles(int domain, std::vector<itk::Point<double>> particles) {
  set_particles(domain, particles, true);
}

//---------------------------------------------------------------------------
void StudioParticles::set_world_particles(int domain, std::vector<itk::Point<double>> particles) {
  set_particles(domain, particles, false);
}

//---------------------------------------------------------------------------
void StudioParticles::set_particles(int domain, std::vector<itk::Point<double>> particles, bool local) {
  auto& points = local ? local_particles_ : global_particles_;
  if (domain >= points.size()) {
    points.resize(domain + 1);
  }

  Eigen::VectorXd vector(particles.size() * 3);
  int idx = 0;

  for (int i = 0; i < particles.size(); i++) {
    vector[idx++] = particles[i][0];
    vector[idx++] = particles[i][1];
    vector[idx++] = particles[i][2];
  }
  points[domain] = vector;

  transform_global_particles();
}

//---------------------------------------------------------------------------
std::vector<Eigen::VectorXd> StudioParticles::get_local_particles() { return local_particles_; }

//---------------------------------------------------------------------------
std::vector<Eigen::VectorXd> StudioParticles::get_world_particles() { return transformed_global_particles_; }

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> StudioParticles::get_local_points(int domain) {
  return eigen_to_point_vector(local_particles_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> StudioParticles::get_world_points(int domain) {
  return eigen_to_point_vector(transformed_global_particles_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> StudioParticles::eigen_to_point_vector(const Eigen::VectorXd& particles) {
  std::vector<itk::Point<double>> points;

  for (size_t i = 0; i < particles.size(); i += 3) {
    itk::Point<double> pt;
    pt[0] = particles[i];
    pt[1] = particles[i + 1];
    pt[2] = particles[i + 2];
    points.push_back(pt);
  }
  return points;
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_local_particles(int domain) {
  assert(domain < local_particles_.size());
  return local_particles_[domain];
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_world_particles(int domain) {
  assert(domain < global_particles_.size());
  return transformed_global_particles_[domain];
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_raw_world_particles(int domain) { return global_particles_[domain]; }

//---------------------------------------------------------------------------
void StudioParticles::set_local_particles(int domain, Eigen::VectorXd particles) {
  if (domain >= local_particles_.size()) {
    local_particles_.resize(domain + 1);
  }
  local_particles_[domain] = particles;
  transform_global_particles();
}

//---------------------------------------------------------------------------
void StudioParticles::set_world_particles(int domain, Eigen::VectorXd particles) {
  if (domain >= global_particles_.size()) {
    global_particles_.resize(domain + 1);
  }
  global_particles_[domain] = particles;
  transform_global_particles();
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_combined_local_particles() { return combine(local_particles_); }

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_combined_global_particles() const {
  return combine(transformed_global_particles_);
}

//---------------------------------------------------------------------------
void StudioParticles::set_combined_global_particles(const Eigen::VectorXd& particles) {
  int num_domains = global_particles_.size();
  if (num_domains == 0) {  // nothing set, assume one domain
    num_domains = 1;
    global_particles_ = {particles};
    return;
  }
  // split out into each domain
  int idx = 0;
  for (size_t i = 0; i < global_particles_.size(); i++) {  // one per domain
    for (size_t j = 0; j < global_particles_[i].size(); j++) {
      global_particles_[i][j] = particles[idx++];
    }
  }
  transformed_global_particles_ = global_particles_;
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::combine(const std::vector<Eigen::VectorXd>& particles) const {
  Eigen::VectorXd points;
  int size = 0;
  for (int i = 0; i < particles.size(); i++) {
    size += particles[i].size();
  }
  points.resize(size);
  int idx = 0;
  for (int i = 0; i < particles.size(); i++) {
    for (int j = 0; j < particles[i].size(); j++) {
      points[idx++] = particles[i][j];
    }
  }

  return points;
}

//---------------------------------------------------------------------------
int StudioParticles::get_domain_for_combined_id(int id) {
  int sum = 0;
  for (int i = 0; i < global_particles_.size(); i++) {
    sum += global_particles_[i].size();
    if (id < sum) {
      return i;
    }
  }
  return 0;
}

//---------------------------------------------------------------------------
void StudioParticles::set_transform(vtkSmartPointer<vtkTransform> transform) {
  transform_ = transform;
  transform_global_particles();
}

//---------------------------------------------------------------------------
void StudioParticles::set_procrustes_transforms(std::vector<vtkSmartPointer<vtkTransform>> transforms) {
  procrustes_transforms_ = transforms;
}

//---------------------------------------------------------------------------
Eigen::VectorXd StudioParticles::get_difference_vectors(const StudioParticles& other) {
  auto combined = get_combined_global_particles();
  auto other_combined = other.get_combined_global_particles();
  if (combined.size() != other_combined.size()) {
    return {};
  }
  return other_combined - combined;
}

//---------------------------------------------------------------------------
void StudioParticles::transform_global_particles() {
  transformed_global_particles_.clear();
  if (!transform_) {
    transformed_global_particles_ = global_particles_;
  } else {
    for (int d = 0; d < local_particles_.size(); d++) {
      Eigen::VectorXd eigen = local_particles_[d];

      for (size_t i = 0; i < eigen.size(); i += 3) {
        double pt[3];
        pt[0] = eigen[i];
        pt[1] = eigen[i + 1];
        pt[2] = eigen[i + 2];

        double* new_point = transform_->TransformPoint(pt);
        eigen[i] = new_point[0];
        eigen[i + 1] = new_point[1];
        eigen[i + 2] = new_point[2];

        if (d < procrustes_transforms_.size() && procrustes_transforms_[d]) {
          pt[0] = eigen[i];
          pt[1] = eigen[i + 1];
          pt[2] = eigen[i + 2];
          double* new_point = procrustes_transforms_[d]->TransformPoint(pt);
          eigen[i] = new_point[0];
          eigen[i + 1] = new_point[1];
          eigen[i + 2] = new_point[2];
        }
      }

      transformed_global_particles_.push_back(eigen);
    }
  }
}
}  // namespace shapeworks
