#pragma once

#include "Project.h"
#include "ProjectUtils.h"

namespace shapeworks {

//! Base class for Project readers
/*!
 * This class serves as an abstract base class for project readers
 */
class ProjectReader {
 public:
  ProjectReader(Project &project);

 protected:
  void load_subjects(project::types::StringMapList list);

  static std::vector<std::string> get_keys(project::types::StringMap map);

  bool contains(project::types::StringMap map, std::string key);

  Project &project_;
};
}  // namespace shapeworks
