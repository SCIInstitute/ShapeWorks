#pragma once

#include <ParticleShapeStatistics.h>
#include <Project/Project.h>

#include "Shape.h"

namespace shapeworks {

//! High level analyze API.
/*!
 * The Analyze class operates on a Project.
 *
 */

class Analyze {
 public:
  Analyze(ProjectHandle project);

  /// Run offline analysis, saving results to outfile
  void run_offline_analysis(std::string outfile, float range, float steps);

  /// Return the list of shapes
  ShapeList get_shapes();

  /// Return the number of subjects
  int get_num_subjects();

  /// Return the number of modes
  int get_num_modes();

  /// Return the number of particles
  int get_num_particles();

  /// Return the mean shape
  Particles get_mean_shape_points();

  /// Return the mean shape
  ShapeHandle get_mean_shape();

  Particles get_group_shape_particles(double ratio);
  ShapeHandle get_group_shape(double ratio);

  /// Return the particles for a given mode and value
  Particles get_shape_points(int mode, double value);
  /// Return the shape for a given mode and value
  ShapeHandle get_mode_shape(int mode, double value);

  bool groups_active() { return false; }

  ShapeHandle create_shape_from_points(Particles points);

  Eigen::VectorXf get_subject_features(int subject, std::string feature_name);

  void set_group_selection(std::string group_name, std::string group1, std::string group2);

 private:
  bool update_shapes();
  bool compute_stats();

  //! To be merged with AnalysisTool's version
  void initialize_mesh_warper();

  //! Break apart combined points into per-domain
  Particles convert_from_combined(const Eigen::VectorXd& points);

  ProjectHandle project_;

  // collection of shapes
  ShapeList shapes_;

  ShapeList group1_list_;
  ShapeList group2_list_;

  std::shared_ptr<MeshManager> mesh_manager_;

  ParticleShapeStatistics stats_;
  bool stats_ready_ = false;

  std::string selected_group_;
  std::string group1_;
  std::string group2_;
};
}  // namespace shapeworks
