#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>

#include "Subject.h"
#include "Parameters.h"

// forward declaration
namespace xlnt {
class workbook;
}

namespace shapeworks {

class Project;
using ProjectHandle = std::shared_ptr<Project>;

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
  bool load(const std::string& filename);

  //! Save to XLSX file
  bool save(const std::string& filename);

  //! Return the headers of the subject sheet
  std::vector<std::string> get_headers();

  //! Return a column by name
  std::vector<std::string> get_string_column(const std::string& name) const;

  //! Return the number of subjects in the project
  int get_number_of_subjects();

  //! Return the number of domains
  int get_number_of_domains_per_subject();

  //! Return the list of Subjects
  std::vector<std::shared_ptr<Subject>>& get_subjects();

  //! Return if segmentations are present
  bool get_segmentations_present() const;

  //! Return if groomed files are present
  bool get_groomed_present() const;

  //! Return if particle files are present
  bool get_particles_present() const;

  //! Get feature names
  std::vector<std::string> get_feature_names();

  //! Get group names
  std::vector<std::string> get_group_names();

  //! Get possible group values
  std::vector<std::string> get_group_values(const std::string& group_name) const;

  //! Retrieve parameters based on key
  Parameters get_parameters(const std::string& name);

  //! Store parameters base on key
  void set_parameters(const std::string& name, Parameters params);

  //! Store from subject list to spreadsheet
  void store_subjects();

  //! Get the supported version (this version of the code)
  int get_supported_version() const;

  //! Get the version of the currently loaded project
  int get_version() const;

private:

  // known prefixes
  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "transform_";
  static constexpr const char* FEATURE_PREFIX = "feature_";
  static constexpr const char* LOCAL_PARTICLES = "local_particles";
  static constexpr const char* WORLD_PARTICLES = "world_particles";
  static constexpr const char* GROUP_PREFIX = "group_";

  std::vector<std::string> get_list(std::vector<std::string> columns, int subject);
  void set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values);

  void set_map(int subject, const std::string& prefix, const std::map<std::string, std::string>& map);

  std::vector<std::vector<double>>
  get_transform_list(std::vector<std::string> columns, int subject);

  void set_transform_list(const std::vector<std::string>& columns, int subject,
                          std::vector<std::vector<double>> transforms);

  std::vector<std::string> get_matching_columns(const std::string& prefix);

  std::vector<std::string> get_extra_columns() const;

  std::string get_value(int column, int subject_id);
  void set_value(int column, int subject_id, const std::string& value);

  void set_value(const std::string& column_name, int subject_id, const std::string& value);
  std::string get_subject_value(int column, int subject_id);

  void load_subjects();

  int get_index_for_column(const std::string& name, bool create_if_not_found = false) const;

  void save_string_column(const std::string& name, std::vector<std::string> items);

  int num_domains_per_subject_ = 0;

  std::unique_ptr<xlnt::workbook> wb_;

  std::vector<std::shared_ptr<Subject>> subjects_;

  bool loaded_{false};

  bool segmentations_present_{false};
  bool groomed_present_{false};
  bool particles_present_{false};

  std::set<std::string> matching_columns_;


  const int supported_version_{1};
  int version_{1};
};
}
