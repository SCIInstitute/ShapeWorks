#pragma once

#include <Project/Project.h>

namespace shapeworks {

class MonaiParameters {
  enum class MonaiModelOption { segmentation, deepgrow_2d,  deepgrow_3d};

 public:
  explicit MonaiParameters(ProjectHandle project);
  void save_to_project();
  void restore_defaults();

 private:
  Parameters params_;
  ProjectHandle project_;
};

}  // namespace shapeworks
