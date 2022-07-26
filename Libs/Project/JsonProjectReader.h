#pragma once

#include "Project.h"

namespace shapeworks {

//! JSON file format reader for Project
/*!
 * This class enables reader of Project objects to JSON file format
 */
class JsonProjectReader {
 public:
  static bool read_project(ProjectHandle project, std::string filename);

private:

};
}  // namespace shapeworks
