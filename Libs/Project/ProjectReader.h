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
  using StringMapList = project::types::StringMapList;
  using StringMap = project::types::StringMap;
  using StringList = project::types::StringList;
  using StringMultiMap = project::types::StringMultiMap;

  //! Constructor
  ProjectReader(Project &project);

  //! Read a project from a file
  virtual bool read_project(std::string filename) = 0;

  virtual StringMap get_parameters(std::string name) = 0;

  virtual StringMultiMap get_multi_parameters(std::string name) = 0;

 protected:
  //! Load subjects from string map list
  void load_subjects(StringMapList list);

  void load_parameters();

  void load_parameter(std::string name, StringMap map);

  void load_landmark_definitions(StringMapList list);

  static StringList get_keys(StringMap map);

  bool contains(StringMap map, std::string key);

  Project &project_;
};
}  // namespace shapeworks
