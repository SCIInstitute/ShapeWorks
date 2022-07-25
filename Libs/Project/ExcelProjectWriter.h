#pragma once

#include "Project.h"

namespace shapeworks {

//! Excel file format writer for Project
/*!
 * This class enables writing of Project objects to Excel file format
 */
class ExcelProjectWriter {
 public:
  static bool write_project(ProjectHandle project, std::string filename);

private:

};
}  // namespace shapeworks
