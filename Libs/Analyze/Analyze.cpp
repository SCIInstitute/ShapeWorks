#include "Analyze.h"

#include <Logging.h>

#include <iostream>

namespace shapeworks {

//---------------------------------------------------------------------------
Analyze::Analyze(ProjectHandle project) : project_(project), mesh_manager_(new MeshManager()) {}

//---------------------------------------------------------------------------
void Analyze::run_offline_analysis() {
  SW_LOG("ShapeWorks Offline Analysis");
  if (!project_->get_particles_present()) {
    throw std::runtime_error("Project has not been optimized, please run optimize first");
  }

  {
  int mode = 10;
  double eigen_value = 42.323423523532;
  SW_LOG("eigen value [{}]: {:.4}", mode, eigen_value);
  }

  SW_DEBUG("debug");

  // load data
  update_shapes();
  // compute stats
  compute_stats();

  // export json file index

  // export pca meshes

  double range = 2.0;
  double steps = 11;
  int half_steps = (steps / 2.0);
  double increment = range / half_steps;

  int num_modes = get_num_modes();
  SW_LOG("number of modes: {}", num_modes);
  for (int mode = num_modes; mode > 0; mode--) {
    double eigen_value = stats_.Eigenvalues()[mode];
    SW_LOG("eigen value [{}]: {}", mode, eigen_value);
  }
}

//---------------------------------------------------------------------------
ShapeList Analyze::get_shapes() { return shapes_; }

//---------------------------------------------------------------------------
int Analyze::get_num_modes() { return shapes_.size() - 1; }

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

  std::cerr << "shapes_.size() - 1 = " << shapes_.size() - 1 << "\n";
  SW_LOG("get_num_modes() = {}", get_num_modes());
  SW_LOG("Successfully loaded shapes");

  return true;
}

//---------------------------------------------------------------------------
bool Analyze::compute_stats() {
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

  SW_LOG("Computed stats successfully");
  return true;
}

}  // namespace shapeworks
