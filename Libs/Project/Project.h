#pragma once

#include <string>
#include <vector>
#include <map>
#include <Subject.h>
#include <Settings.h>

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

  std::vector<std::shared_ptr<Subject>> & get_subjects();

  std::vector<std::string> get_string_column(std::string name);

  bool get_segmentations_present();
  bool get_groomed_present();
  bool get_particles_present();

  Settings get_settings(std::string name);
  void set_settings(std::string name, Settings);

  void store_subjects();

private:

  std::vector<std::string> get_list(std::vector<std::string> columns, int subject);
  void set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values);

  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* MESH_PREFIX = "mesh_";
  static constexpr const char* LOCAL_PARTICLES = "local_particles";
  static constexpr const char* WORLD_PARTICLES = "world_particles";

  std::vector<std::string> get_matching_columns(std::string prefix);

  std::string get_value(int column, int subject_id);
  void set_value(int column, int subject_id, std::string value);

  void set_value(std::string column_name, int subject_id, std::string value);
  std::string get_subject_value(int column, int subject_id);

  void load_subjects();

  int get_index_for_column(std::string name, bool create_if_not_found = false);

  void save_string_column(std::string name, std::vector<std::string> items);

  int num_domains_ = 0;

  std::unique_ptr<xlnt::workbook> wb_;

  std::vector<std::shared_ptr<Subject>> subjects_;

  bool loaded_{false};

  bool segmentations_present_{false};
  bool groomed_present_{false};
  bool particles_present_{false};
};
}
