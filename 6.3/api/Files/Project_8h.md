---
title: Libs/Project/Project.h

---

# Libs/Project/Project.h



## Namespaces

| Name           |
| -------------- |
| **[xlnt](../Namespaces/namespacexlnt.md)**  |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md)** <br>Landmark class containing properties of each landmark.  |
| class | **[shapeworks::Project](../Classes/classshapeworks_1_1Project.md)** <br>Representation of a project.  |




## Source code

```cpp
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



class Project {

public:

  Project();
  ~Project();

  bool load(const std::string& filename);

  bool save(const std::string& filename);

  std::string get_filename();

  void set_filename(std::string filename);

  std::vector<std::string> get_headers();

  std::vector<std::string> get_string_column(const std::string& name) const;

  int get_number_of_subjects();

  int get_number_of_domains_per_subject();

  std::vector<std::string> get_domain_names();

  std::vector<std::shared_ptr<Subject>>& get_subjects();

  void set_subjects(const std::vector<std::shared_ptr<Subject>>& subjects);

  bool get_originals_present() const;

  bool get_groomed_present() const;

  bool get_particles_present() const;

  bool get_images_present();

  std::vector<std::string> get_feature_names();

  std::vector<std::string> get_image_names();

  std::vector<std::string> get_group_names();

  std::vector<std::string> get_group_values(const std::string& group_name) const;

  Parameters get_parameters(const std::string& name, std::string domain_name = "");

  void
  set_parameters(const std::string& name, Parameters params, std::string domain_name = "");

  void clear_parameters(const std::string& name);

  void store_subjects();

  int get_supported_version() const;

  int get_version() const;

  std::vector<LandmarkDefinition> get_landmarks(int domain_id);

  std::vector<std::vector<LandmarkDefinition>> get_all_landmark_definitions();

  bool get_landmarks_present();

  void set_landmarks(int domain_id, std::vector<LandmarkDefinition> landmarks);

  void new_landmark(int domain_id);

  std::vector<DomainType> get_original_domain_types();

  std::vector<DomainType> get_groomed_domain_types();

  void set_original_domain_types(std::vector<DomainType> domain_types);

  void set_groomed_domain_types(std::vector<DomainType> domain_types);

private:

  void load_landmark_definitions();
  void store_landmark_definitions();

  void set_default_landmark_colors();

  void determine_domain_types();

  DomainType determine_domain_type(std::string filename);

  static bool starts_with(std::string str, std::string prefix);

  int get_or_create_worksheet(std::string name);
  std::string get_new_file_column(std::string name, int idx);

  // e.g. "la" for "groomed_la"
  std::string get_column_identifier(std::string name);

  // known prefixes
  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* SHAPE_PREFIX = "shape_";
  static constexpr const char* MESH_PREFIX = "mesh_";
  static constexpr const char* CONTOUR_PREFIX = "contour_";
  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* GROOMED_CONTOUR_PREFIX = "groomed_contour_";
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

  std::vector<std::string> get_file_list(std::vector<std::string> columns, int subject);
  std::vector<std::string> get_list(std::vector<std::string> columns, int subject);
  void set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values);

  void
  set_map(int subject, const std::string& prefix, const std::map<std::string, std::string>& map);

  std::vector<std::vector<double>>
  get_transform_list(std::vector<std::string> columns, int subject);

  void set_transform_list(const std::vector<std::string>& columns, int subject,
                          std::vector<std::vector<double>> transforms);

  std::vector<std::string> get_matching_columns(const std::string& prefix);

  std::vector<std::string> get_matching_columns(const std::vector<std::string>& prefixes);

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


  bool originals_present_{false};
  bool groomed_present_{false};
  bool particles_present_{false};

  std::set<std::string> matching_columns_;
  std::vector<std::string> mesh_scalars_;

  std::vector<std::string> feature_names_;

  // to avoid re-reading from mesh files each time the list of names is requested
  bool feature_names_read_done_{false};

  std::vector<std::string> input_prefixes_;

  bool landmarks_loaded_{false};
  std::vector<std::vector<LandmarkDefinition>> landmark_definitions_;

  std::vector<DomainType> original_domain_types_;
  std::vector<DomainType> groomed_domain_types_;

  const int supported_version_{2};
  int version_{2};
};
}
```


-------------------------------

Updated on 2022-04-15 at 07:45:38 +0000