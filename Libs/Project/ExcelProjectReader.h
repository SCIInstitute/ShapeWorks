#pragma once

#include "ProjectReader.h"

namespace shapeworks {

//! Excel file format reader for Project
/*!
 * This class enables reader of Project objects from XLSX file format
 */
class ExcelProjectReader : public ProjectReader {
 public:
  ExcelProjectReader(Project &project);
  ~ExcelProjectReader();

  bool read_project(std::string filename) override;

  StringMap get_parameters(std::string name) override;

  StringMultiMap get_multi_parameters(std::string name) override;

 private:

  // pimpl container
  class Container;
  std::unique_ptr<Container> container_;
};
}  // namespace shapeworks
