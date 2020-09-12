#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Subject.h"
#include "Parameters.h"

// forward declaration
namespace xlnt {
class workbook;
}

namespace shapeworks {

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

  //! Return a column by name
  std::vector<std::string> get_string_column(std::string name);

  //! Return the number of subjects in the project
  int get_number_of_subjects();

  //! Return the number of domains
  int get_number_of_domains();

  //! Return the list of Subjects
  std::vector<std::shared_ptr<Subject>>& get_subjects();

  //! Return if segmentations are present
  bool get_segmentations_present();

  //! Return if groomed files are present
  bool get_groomed_present();

  //! Return if particle files are present
  bool get_particles_present();

  //! Get feature names
  std::vector<std::string> get_feature_names() const;

  //! Get group names
  std::vector<std::string> get_group_names() const;

  //! Retrieve parameters based on key
  Parameters get_parameters(std::string name);

  //! Store parameters base on key
  void set_parameters(std::string name, Parameters params);

  //! Store from subject list to spreadsheet
  void store_subjects();

  //! Get the supported version (this version of the code)
  int get_supported_version();

  //! Get the version of the currently loaded project
  int get_version();

private:

  // known prefixes
  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "transform_";
  static constexpr const char* MESH_PREFIX = "mesh_";
  static constexpr const char* FEATURE_PREFIX = "feature_";
  static constexpr const char* LOCAL_PARTICLES = "local_particles";
  static constexpr const char* WORLD_PARTICLES = "world_particles";
  static constexpr const char* GROUP_PREFIX = "group_";

  std::vector<std::string> get_list(std::vector<std::string> columns, int subject);
  void set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values);

  std::vector<std::vector<double>>
  get_transform_list(std::vector<std::string> columns, int subject);

  void set_transform_list(std::vector<std::string> columns, int subject,
                          std::vector<std::vector<double>> transforms);

  std::vector<std::string> get_matching_columns(std::string prefix) const;

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

  const int supported_version_{1};
  int version_{1};
};
}
