#pragma once

#include "Project.h"
#include "ProjectReader.h"
#include "ProjectUtils.h"

namespace shapeworks {


//! JSON file format reader for Project
/*!
 * This class enables reader of Project objects from JSON file format
 */
class JsonProjectReader : public ProjectReader {
 public:
  using StringMap = project::types::StringMap;
  using StringMapList = project::types::StringMapList;

  JsonProjectReader(Project &project);
  ~JsonProjectReader();

  bool read_project(std::string filename);

  void read_subjects();

 private:
  class JsonContainer;
  // Pointer to the internal implementation
  std::unique_ptr<JsonContainer> container_;

  // ready a sheet into a vector of maps
  StringMapList object_to_map_list(std::string name);
};
}  // namespace shapeworks
