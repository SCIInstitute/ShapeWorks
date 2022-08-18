#include "Analyze.h"

#include <Logging.h>

#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

namespace shapeworks {

//---------------------------------------------------------------------------
Analyze::Analyze(ProjectHandle project) : project_(project), mesh_manager_(new MeshManager()) {}

//---------------------------------------------------------------------------
void Analyze::run_offline_analysis(std::string outfile) {
  SW_LOG("ShapeWorks Offline Analysis");
  if (!project_->get_particles_present()) {
    throw std::runtime_error("Project has not been optimized, please run optimize first");
  }

  // load data from the project
  update_shapes();

  // compute stats
  compute_stats();

  json j;

  double range = 2.0;
  double steps = 11;
  int half_steps = (steps / 2.0);
  double increment = range / half_steps;

  int num_domains = project_->get_domain_names().size();
  int num_modes = get_num_modes();
  j["number_of_modes"] = num_modes;
  SW_LOG("number of modes: {}", num_modes);

  // output mean shape
  auto mean_shape = get_mean_shape();
  auto meshes = mean_shape->get_reconstructed_meshes(true);
  json jmean;
  for (int d = 0; d < num_domains; d++) {
    std::string domain_id = std::to_string(d);
    auto mesh = meshes.meshes()[d];
    json item;
    std::string filename = "mean_shape_" + domain_id + ".vtk";
    item["mesh"] = filename;
    Mesh(mesh->get_poly_data()).write(filename);
    jmean[domain_id] = item;
  }
  j["mean"] = jmean;

  // export modes
  for (int mode = num_modes; mode > 0; mode--) {
    unsigned int m = stats_.Eigenvectors().cols() - (mode + 1);
    json jmode;
    double eigen_value = stats_.Eigenvalues()[mode];
    jmode["eigen_value"] = eigen_value;
    SW_LOG("eigen value [{}]: {}", mode, eigen_value);

    j[std::to_string(m)] = jmode;
  }

  std::ofstream file(outfile);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + outfile + " for writing");
  }
  file << j.dump(4);
}

//---------------------------------------------------------------------------
ShapeList Analyze::get_shapes() { return shapes_; }

//---------------------------------------------------------------------------
int Analyze::get_num_modes() { return shapes_.size() - 1; }

//---------------------------------------------------------------------------
Particles Analyze::get_mean_shape_points() {
  if (!compute_stats()) {
    return Particles();
  }

  return convert_from_combined(stats_.Mean());
}

//---------------------------------------------------------------------------
ShapeHandle Analyze::get_mean_shape() {
  auto shape_points = get_mean_shape_points();
  ShapeHandle shape = create_shape_from_points(shape_points);

  return shape;
}

//---------------------------------------------------------------------------
Particles Analyze::get_shape_points(int mode, double value) {}

//---------------------------------------------------------------------------
ShapeHandle Analyze::get_mode_shape(int mode, double value) {}

//---------------------------------------------------------------------------
ShapeHandle Analyze::create_shape_from_points(Particles points) {
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(mesh_manager_);
  shape->set_particles(points);
  shape->get_reconstructed_meshes();
  /// shape->set_reconstruction_transforms(reconstruction_transforms_);
  return shape;
}

//---------------------------------------------------------------------------
bool Analyze::update_shapes() {
  int num_subjects = project_->get_number_of_subjects();

  auto subjects = project_->get_subjects();

  SW_LOG("number of subjects: {}", num_subjects);

  for (int i = 0; i < num_subjects; i++) {
    auto shape = std::make_shared<Shape>();
    shape->set_mesh_manager(mesh_manager_);
    shape->set_subject(subjects[i]);

    auto locals = subjects[i]->get_local_particle_filenames();
    auto worlds = subjects[i]->get_world_particle_filenames();
    auto landmark_files = subjects[i]->get_landmarks_filenames();
    auto constraints_files = subjects[i]->get_constraints_filenames();

    if (!shape->import_local_point_files(locals)) {
      return false;
    }
    if (!shape->import_global_point_files(worlds)) {
      return false;
    }
    if (!shape->import_landmarks_files(landmark_files)) {
      return false;
    }
    if (!shape->import_constraints(constraints_files)) {
      return false;
    }

    // add to landmark definitions if landmark files contain more landmarks than the project definitions
    auto landmarks = shape->landmarks();
    for (int row = 0; row < landmarks.rows(); row++) {
      int domain_id = landmarks(row, 0);
      int point_id = landmarks(row, 1);
      auto landmark_definitions = project_->get_landmarks(domain_id);
      while (point_id >= landmark_definitions.size()) {
        project_->new_landmark(domain_id);
        landmark_definitions = project_->get_landmarks(domain_id);
      }
    }
    shapes_.push_back(shape);
  }

  SW_DEBUG("Successfully loaded shapes");

  return true;
}

//---------------------------------------------------------------------------
bool Analyze::compute_stats() {
  if (stats_ready_) {
    return true;
  }

  SW_LOG("Computing stats...");

  std::vector<Eigen::VectorXd> points;
  std::vector<int> group_ids;

  std::string group_set = "need to add groups1";
  std::string left_group = "need to add groups2";
  std::string right_group = "need to add groups3";

  /// TODO: implement groups
  /// bool groups_enabled = groups_active();
  bool groups_enabled = false;

  group1_list_.clear();
  group2_list_.clear();

  for (auto& shape : shapes_) {
    if (groups_enabled) {
      auto value = shape->get_subject()->get_group_value(group_set);
      if (value == left_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(1);
        group1_list_.push_back(shape);
      } else if (value == right_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(2);
        group2_list_.push_back(shape);
      } else {
        // we don't include it
      }
    } else {
      points.push_back(shape->get_global_correspondence_points());
      group_ids.push_back(1);
    }
  }

  if (points.empty()) {
    return false;
  }

  // consistency check
  size_t point_size = points[0].size();
  for (auto&& p : points) {
    if (p.size() != point_size) {
      SW_ERROR("Inconsistency in data, particle files must contain the same number of points");
      return false;
    }
  }

  stats_.ImportPoints(points, group_ids);
  stats_.ComputeModes();

  stats_ready_ = true;
  SW_LOG("Computed stats successfully");
  return true;
}

//---------------------------------------------------------------------------
Particles Analyze::convert_from_combined(const Eigen::VectorXd& points) {
  Particles particles;
  if (shapes_.empty()) {
    return particles;
  }
  auto base = shapes_[0]->get_particles();

  auto worlds = base.get_world_particles();
  int idx = 0;
  for (int d = 0; d < worlds.size(); d++) {
    Eigen::VectorXd new_world(worlds[d].size());
    for (int i = 0; i < worlds[d].size(); i++) {
      new_world[i] = points[idx++];
    }
    particles.set_world_particles(d, new_world);
    particles.set_local_particles(d, new_world);
  }

  return particles;
}

}  // namespace shapeworks
