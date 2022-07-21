#pragma once

#include "Project.h"

namespace shapeworks {

//! JSON file format writer for Project
/*!
 * This class enables writing of Project objects to JSON file format
 */
class JsonProjectWriter {
 public:
  static bool writeProject(ProjectHandle project, std::string filename);
};
}  // namespace shapeworks
