#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Parameters.h"
#include "Subject.h"

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

  //! Return the number of subjects in the project
  int get_number_of_subjects();

  //! Return the number of domains
  int get_number_of_domains_per_subject();

  //! Return the domain names (e.g. femur, pelvis, etc)
  std::vector<std::string> get_domain_names();

  //! Set the domain names
  void set_domain_names(std::vector<std::string> domain_names);

  //! Return the Subjects (as a reference)
  std::vector<std::shared_ptr<Subject>>& get_subjects();

  //! Set the Subjects
  void set_subjects(const std::vector<std::shared_ptr<Subject>>& subjects);

  void update_subjects();

  //! Return if originals are present
  bool get_originals_present() const;

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
  Parameters get_parameters(const std::string& name, std::string domain_name = "");

  //! Retrieve full parameter map for a given name
  std::map<std::string, Parameters> get_parameter_map(const std::string& name);

  //! Set parameter map for a given name
  void set_parameter_map(const std::string& name, std::map<std::string, Parameters> map);

  //! Store parameters based on key
  void set_parameters(const std::string& name, Parameters params, std::string domain_name = "");

  //! Clear parameters based on key
  void clear_parameters(const std::string& name);

  //! Get the supported version (this version of the code)
  int get_supported_version() const;

  //! Get the version of the currently loaded project
  int get_version() const;

  //! Return the set of landmarks definitions for a particular domain
  std::vector<LandmarkDefinition> get_landmarks(int domain_id);

  //! Return all landmark definitions
  std::vector<std::vector<LandmarkDefinition>> get_all_landmark_definitions();

  //! Set all landmark definitions
  void set_landmark_definitions(std::vector<std::vector<LandmarkDefinition>> defs);

  //! Return if landmarks are present
  bool get_landmarks_present();

  //! Set landmarks for this project
  void set_landmarks(int domain_id, std::vector<LandmarkDefinition> landmarks);

  //! Add a new landmark
  void new_landmark(int domain_id);

  //! Return the original domain types
  std::vector<DomainType> get_original_domain_types();

  //! Return the groomed domain types
  std::vector<DomainType> get_groomed_domain_types();

  //! Set the original domain types
  void set_original_domain_types(std::vector<DomainType> domain_types);

  //! Get the groomed domain types
  void set_groomed_domain_types(std::vector<DomainType> domain_types);

 private:
  void set_default_landmark_colors();

  void determine_feature_names();

  std::string get_next_landmark_name(int domain_id);
  std::string get_next_landmark_color(int domain_id);

  int num_domains_per_subject_ = 1;

  std::vector<std::shared_ptr<Subject>> subjects_;

  bool loaded_{false};

  std::string filename_;

  std::vector<std::string> default_landmark_colors_;

  bool originals_present_{false};
  bool groomed_present_{false};
  bool particles_present_{false};
  bool images_present_{false};

  std::vector<std::string> feature_names_;
  std::vector<std::string> image_names_;

  std::vector<std::vector<LandmarkDefinition>> landmark_definitions_;

  std::vector<std::string> domain_names_;
  std::vector<DomainType> original_domain_types_;
  std::vector<DomainType> groomed_domain_types_;

  // map of type (e.g. groom, optimize) to map (domain->Parameters)
  std::map<std::string, std::map<std::string, Parameters>> parameters_;

  const int supported_version_{2};
  int version_{2};
};
}  // namespace shapeworks
