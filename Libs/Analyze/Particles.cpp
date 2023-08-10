#include <Analyze/Particles.h>
#include <Logging.h>
#include <vtkTransform.h>

#include <cassert>

namespace shapeworks {


//---------------------------------------------------------------------------
void Particles::set_local_particles(int domain, const std::vector<itk::Point<double>>& particles) {
  set_particles(domain, particles, true);
}

//---------------------------------------------------------------------------
void Particles::set_world_particles(int domain, const std::vector<itk::Point<double>>& particles) {
  set_particles(domain, particles, false);
}

//---------------------------------------------------------------------------
void Particles::set_particles(int domain, std::vector<itk::Point<double>> particles, bool local) {
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
std::vector<Eigen::VectorXd> Particles::get_local_particles() const { return local_particles_; }

//---------------------------------------------------------------------------
std::vector<Eigen::VectorXd> Particles::get_world_particles() const { return transformed_global_particles_; }

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> Particles::get_local_points(int domain) {
  return eigen_to_point_vector(local_particles_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> Particles::get_world_points(int domain) {
  return eigen_to_point_vector(transformed_global_particles_[domain]);
}

//---------------------------------------------------------------------------
std::vector<itk::Point<double>> Particles::eigen_to_point_vector(const Eigen::VectorXd& particles) const {
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
Eigen::VectorXd Particles::get_local_particles(int domain) {
  assert(domain < local_particles_.size());
  return local_particles_[domain];
}

//---------------------------------------------------------------------------
Eigen::VectorXd Particles::get_world_particles(int domain) {
  assert(domain < global_particles_.size());
  return transformed_global_particles_[domain];
}

//---------------------------------------------------------------------------
Eigen::VectorXd Particles::get_raw_world_particles(int domain) { return global_particles_[domain]; }

//---------------------------------------------------------------------------
void Particles::set_local_particles(int domain, Eigen::VectorXd particles) {
  if (domain >= local_particles_.size()) {
    local_particles_.resize(domain + 1);
  }
  local_particles_[domain] = particles;
  transform_global_particles();
}

//---------------------------------------------------------------------------
void Particles::set_world_particles(int domain, Eigen::VectorXd particles) {
  if (domain >= global_particles_.size()) {
    global_particles_.resize(domain + 1);
  }
  global_particles_[domain] = particles;
  transform_global_particles();
}

//---------------------------------------------------------------------------
Eigen::VectorXd Particles::get_combined_local_particles() const { return combine(local_particles_); }

//---------------------------------------------------------------------------
Eigen::VectorXd Particles::get_combined_global_particles() const { return combine(transformed_global_particles_); }

//---------------------------------------------------------------------------
void Particles::set_combined_global_particles(const Eigen::VectorXd& particles) {
  int num_domains = global_particles_.size();
  if (num_domains == 0) {  // nothing set, assume one domain
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
Eigen::VectorXd Particles::combine(const std::vector<Eigen::VectorXd>& particles) const {
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
int Particles::get_domain_for_combined_id(int id) {
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
void Particles::set_transform(vtkSmartPointer<vtkTransform> transform) {
  transform_ = transform;
  transform_global_particles();
}

//---------------------------------------------------------------------------
void Particles::set_procrustes_transforms(const std::vector<vtkSmartPointer<vtkTransform>>& transforms) {
  procrustes_transforms_ = transforms;
}

//---------------------------------------------------------------------------
Eigen::VectorXd Particles::get_difference_vectors(const Particles& other) const {
  auto combined = get_combined_global_particles();
  auto other_combined = other.get_combined_global_particles();
  if (combined.size() != other_combined.size()) {
    return {};
  }
  return other_combined - combined;
}

//---------------------------------------------------------------------------
void Particles::transform_global_particles() {
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

        const double* new_point = transform_->TransformPoint(pt);
        eigen[i] = new_point[0];
        eigen[i + 1] = new_point[1];
        eigen[i + 2] = new_point[2];
        if (alignment_type_ < 0) {
          // for local and global alignment, use per-domain value
          alignment_type_ = d;
        }

        if (d < procrustes_transforms_.size() && procrustes_transforms_[d]) {
          auto transform = procrustes_transforms_[d];
          // extract scale
          double scale[3];
          transform->GetScale(scale);

          // apply scale
          eigen[i] *= scale[0];
          eigen[i + 1] *= scale[1];
          eigen[i + 2] *= scale[2];
        }

        if (alignment_type_ > 0 && alignment_type_ < procrustes_transforms_.size() &&
            procrustes_transforms_[alignment_type_]) {
          pt[0] = eigen[i];
          pt[1] = eigen[i + 1];
          pt[2] = eigen[i + 2];
          auto transform = procrustes_transforms_[alignment_type_];

          // remove scale from transform, leaving rotation and translation
          double scale[3];
          transform->GetScale(scale);
          transform->Scale(1.0 / scale[0], 1.0 / scale[1], 1.0 / scale[2]);

          double* new_point2 = transform->TransformPoint(pt);
          eigen[i] = new_point2[0];
          eigen[i + 1] = new_point2[1];
          eigen[i + 2] = new_point2[2];
        }
      }

      transformed_global_particles_.push_back(eigen);
    }
  }
}

void Particles::save_particles_file(std::string filename, const Eigen::VectorXd& points) {
  std::ofstream out(filename);
  size_t newline = 1;
  for (int i = 0; i < points.size(); i++) {
    out << points[i] << (newline % 3 == 0 ? "\n" : "    ");
    newline++;
  }
  out.close();
}

void Particles::set_alignment_type(int alignment) { alignment_type_ = alignment; }

}  // namespace shapeworks
