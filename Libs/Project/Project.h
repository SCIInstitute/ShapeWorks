#pragma once

#include <string>
#include <vector>
#include <Subject.h>

// forward declaration
namespace xlnt
{
class workbook;
}

namespace shapeworks
{

//! Representation of a project.
/*!
 * The Project class encapsulates the spreadsheet based file format for storing project data.
 *
 */
class Project {

public:

  Project();
  ~Project();

  //! Load from XLSX file
  bool load(std::string filename);

  //! Save to XLSX file
  bool save(std::string filename);

  //! Return the headers of the subject sheet
  std::vector<std::string> get_headers();

  //! Return the number of subjects in the project
  int get_number_of_subjects();

  //! Return the number of domains
  int get_number_of_domains();

  //! Return the segmentation columns
  std::vector<std::string> get_segmentations();

  std::vector<std::string> get_original_files();
  void set_original_files(std::vector<std::string> files);

  std::vector<std::string> get_distance_transform_files();
  void set_distance_transform_files(std::vector<std::string> files);

  std::vector<std::string> get_local_point_files();
  void set_local_point_files(std::vector<std::string> files);

  std::vector<std::string> get_global_point_files();
  void set_global_point_files(std::vector<std::string> files);

  std::vector<Subject> get_subjects();

  std::vector<std::string> get_string_column(std::string name);

private:

  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";

  std::vector<std::string> get_matching_columns(std::string prefix);

  std::string get_value(int column, int subject_id);

  void load_subjects();

  int num_domains_ = 0;

  std::vector<std::string> original_files_;
  std::vector<std::string> distance_transform_files_;
  std::vector<std::string> local_point_files_;
  std::vector<std::string> global_point_files_;

  int get_index_for_column(std::string name, bool create_if_not_found = false);

  void save_string_column(std::string name, std::vector<std::string> items);

  std::unique_ptr<xlnt::workbook> wb_;

  std::vector<Subject> subjects_;

  bool loaded_ = false;
};
}
