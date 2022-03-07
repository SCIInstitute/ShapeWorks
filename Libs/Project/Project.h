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

//! Landmark class containing properties of each landmark
class LandmarkDefinition {
public:
  std::string domain_;
  int domain_id_ = -1;
  int landmark_id = -1;
  std::string name_;
  bool visible_ = true;
  std::string color_;
  std::string comment_;
};


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

  //! Return the filename
  std::string get_filename();

  //! Set project filename
  void set_filename(std::string filename);

  //! Return the headers of the subject sheet
  std::vector<std::string> get_headers();

  //! Return a column by name
  std::vector<std::string> get_string_column(const std::string& name) const;

  //! Return the number of subjects in the project
  int get_number_of_subjects();

  //! Return the number of domains
  int get_number_of_domains_per_subject();

  //! Return the domain names (e.g. femur, pelvis, etc)
  std::vector<std::string> get_domain_names();

  //! Return the list of Subjects
  std::vector<std::shared_ptr<Subject>>& get_subjects();

  //! Return if segmentations are present
  bool get_segmentations_present() const;

  //! Return if groomed files are present
  bool get_groomed_present() const;

  //! Return if particle files are present
  bool get_particles_present() const;

  //! Return if images are present (e.g. CT/MRI)
  bool get_images_present();

  //! Get feature names
  std::vector<std::string> get_feature_names();

  //! Get image names
  std::vector<std::string> get_image_names();

  //! Get group names
  std::vector<std::string> get_group_names();

  //! Get possible group values
  std::vector<std::string> get_group_values(const std::string& group_name) const;

  //! Retrieve parameters based on key
  Parameters get_parameters(const std::string& name, const std::string& domain_name = "");

  //! Store parameters based on key
  void
  set_parameters(const std::string& name, Parameters params, const std::string& domain_name = "");

  //! Clear parameters based on key
  void clear_parameters(const std::string& name);

  //! Store from subject list to spreadsheet
  void store_subjects();

  //! Get the supported version (this version of the code)
  int get_supported_version() const;

  //! Get the version of the currently loaded project
  int get_version() const;

  //! Return the set of landmarks definitions for a particular domain
  std::vector<LandmarkDefinition> get_landmarks(int domain_id);

  //! Return all landmark definitions
  std::vector<std::vector<LandmarkDefinition>> get_all_landmark_definitions();

  //! Return if landmarks are present
  bool get_landmarks_present();

  //! Set landmarks for this project
  void set_landmarks(int domain_id, std::vector<LandmarkDefinition> landmarks);

  //! Add a new landmark
  void new_landmark(int domain_id);

private:

  void load_landmark_definitions();
  void store_landmark_definitions();

  void set_default_landmark_colors();

  int get_or_create_worksheet(std::string name);
  std::string get_new_file_column(std::string name, int idx);

  // e.g. "la" for "groomed_la"
  std::string get_column_identifier(std::string name);

  // known prefixes
  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* SHAPE_PREFIX = "shape_";
  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "alignment_";
  static constexpr const char* PROCRUSTES_TRANSFORMS_PREFIX = "procrustes_";
  static constexpr const char* FEATURE_PREFIX = "feature_";
  static constexpr const char* LOCAL_PARTICLES = "local_particles";
  static constexpr const char* WORLD_PARTICLES = "world_particles";
  static constexpr const char* GROUP_PREFIX = "group_";
  static constexpr const char* IMAGE_PREFIX = "image_";
  static constexpr const char* NAME = "name";
  static constexpr const char* LANDMARKS_FILE_PREFIX = "landmarks_file_";
  static constexpr const char* CONSTRAINTS_PREFIX = "constraints_";

  std::vector<std::string> get_list(std::vector<std::string> columns, int subject);
  void set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values);

  void
  set_map(int subject, const std::string& prefix, const std::map<std::string, std::string>& map);

  std::vector<std::vector<double>>
  get_transform_list(std::vector<std::string> columns, int subject);

  void set_transform_list(const std::vector<std::string>& columns, int subject,
                          std::vector<std::vector<double>> transforms);

  std::vector<std::string> get_matching_columns(const std::string& prefix);

  std::vector<std::string> get_matching_columns(const std::vector<std::string> prefixes);

  std::vector<std::string> get_extra_columns() const;

  std::string get_value(int column, int subject_id);
  void set_value(int column, int subject_id, const std::string& value);

  void set_value(const std::string& column_name, int subject_id, const std::string& value);
  std::string get_subject_value(int column, int subject_id);

  void load_subjects();

  int get_index_for_column(const std::string& name, bool create_if_not_found = false,
                           int sheet = 0) const;

  void save_string_column(const std::string& name, std::vector<std::string> items);

  std::string get_next_landmark_name(int domain_id);
  std::string get_next_landmark_color(int domain_id);

  int num_domains_per_subject_ = 1;

  std::unique_ptr<xlnt::workbook> wb_;

  std::vector<std::shared_ptr<Subject>> subjects_;

  bool loaded_{false};

  std::string filename_;

  std::vector<std::string> default_landmark_colors_;


  bool segmentations_present_{false};
  bool groomed_present_{false};
  bool particles_present_{false};

  std::set<std::string> matching_columns_;
  std::vector<std::string> mesh_scalars_;

  std::vector<std::string> feature_names_;

  // to avoid re-reading from mesh files each time the list of names is requested
  bool feature_names_read_done_{false};

  std::vector<std::string> input_prefixes_;

  std::vector<std::vector<LandmarkDefinition>> landmark_definitions_;

  const int supported_version_{2};
  int version_{2};
};
}
