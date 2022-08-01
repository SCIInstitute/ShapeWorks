#pragma once

#include "Project.h"
#include "ProjectReader.h"
#include "ProjectUtils.h"

namespace shapeworks {

//! Excel file format reader for Project
/*!
 * This class enables reader of Project objects from XLSX file format
 */
class ExcelProjectReader : public ProjectReader {
 public:
  using StringMapList = project::types::StringMapList;

  ExcelProjectReader(Project &project);

  bool read_project(std::string filename);

 private:
  void load_subjects();

  // ready a sheet into a vector of maps
  StringMapList sheet_to_map(std::string name);

  std::unique_ptr<xlnt::workbook> wb_;
};
}  // namespace shapeworks
