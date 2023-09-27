---
title: Libs/Project/Project.h

---

# Libs/Project/Project.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md)** <br>Landmark class containing properties of each landmark.  |
| class | **[shapeworks::Project](../Classes/classshapeworks_1_1Project.md)** <br>Representation of a project.  |




## Source code

```cpp
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
  using StringMap = std::map<std::string, std::string>;

  Project();
  ~Project();

  bool load(const std::string& filename);

  bool save(const std::string& filename);

  void set_project_path(const std::string& path);

  std::string get_filename();

  void set_filename(std::string filename);

  std::vector<std::string> get_headers();

  std::vector<std::string> get_string_column(const std::string& name) const;

  int get_number_of_subjects();

  int get_number_of_domains_per_subject();

  std::vector<std::string> get_domain_names();

  void set_domain_names(std::vector<std::string> domain_names);

  std::vector<std::shared_ptr<Subject>>& get_subjects();

  void set_subjects(const std::vector<std::shared_ptr<Subject>>& subjects);

  void update_subjects();

  bool get_originals_present() const;

  bool get_groomed_present() const;

  bool get_particles_present() const;

  bool get_images_present();

  bool get_fixed_subjects_present();

  std::vector<std::string> get_feature_names();

  std::vector<std::string> get_image_names();

  std::vector<std::string> get_group_names();

  std::vector<std::string> get_group_values(const std::string& group_name) const;

  Parameters get_parameters(const std::string& name, std::string domain_name = "");

  std::map<std::string, Parameters> get_parameter_map(const std::string& name);

  void set_parameter_map(const std::string& name, std::map<std::string, Parameters> map);

  void set_parameters(const std::string& name, Parameters params, std::string domain_name = "");

  void clear_parameters(const std::string& name);

  int get_supported_version() const;

  int get_version() const;

  std::vector<LandmarkDefinition> get_landmarks(int domain_id);

  std::vector<std::vector<LandmarkDefinition>> get_all_landmark_definitions();

  void set_landmark_definitions(std::vector<std::vector<LandmarkDefinition>> defs);

  bool get_landmarks_present();

  void set_landmarks(int domain_id, std::vector<LandmarkDefinition> landmarks);

  void new_landmark(int domain_id);

  std::vector<DomainType> get_original_domain_types();

  std::vector<DomainType> get_groomed_domain_types();

  void set_original_domain_types(std::vector<DomainType> domain_types);

  void set_groomed_domain_types(std::vector<DomainType> domain_types);

 private:
  void set_default_landmark_colors();

  void determine_feature_names();

  std::string get_next_landmark_name(int domain_id);
  std::string get_next_landmark_color(int domain_id);

  std::vector<std::shared_ptr<Subject>> subjects_;

  bool loaded_{false};

  std::string filename_;
  std::string project_path_;

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
```


-------------------------------

Updated on 2023-09-27 at 04:30:59 +0000
