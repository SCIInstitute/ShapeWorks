#pragma once

#include <Libs/Project/Project.h>
#include <ParticleShapeStatistics.h>
#include <Shape.h>

namespace shapeworks {

//! High level analyze API.
/*!
 * The Analyze class operates on a Project.
 *
 */

class Analyze {
 public:
  Analyze(ProjectHandle project);

  void run_offline_analysis(std::string outfile);

  ShapeList get_shapes();

  int get_num_modes();

 private:
  bool update_shapes();
  bool compute_stats();

  ProjectHandle project_;

  // collection of shapes
  ShapeList shapes_;

  ShapeList group1_list_;
  ShapeList group2_list_;


  std::shared_ptr<MeshManager> mesh_manager_;

  ParticleShapeStatistics stats_;
};
}  // namespace shapeworks
