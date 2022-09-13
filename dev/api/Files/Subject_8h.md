---
title: Libs/Project/Subject.h

---

# Libs/Project/Subject.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md)** <br>Representation of a single subject.  |




## Source code

```cpp
#pragma once

#include <Libs/Optimize/ParticleSystem/DomainType.h>

#include <map>
#include <string>
#include <vector>

namespace shapeworks {


class Subject {
 public:
  Subject();
  ~Subject();

  void set_original_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_original_filenames();

  void set_groomed_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_groomed_filenames();

  void set_local_particle_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_local_particle_filenames();

  void set_world_particle_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_world_particle_filenames();

  void set_landmarks_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_landmarks_filenames();

  void set_constraints_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_constraints_filenames();

  void set_number_of_domains(int number_of_domains);
  int get_number_of_domains();

  std::map<std::string, std::string> get_feature_filenames() const;
  void set_feature_filenames(const std::map<std::string, std::string>& feature_filenames);

  std::vector<std::vector<double>> get_groomed_transforms() const;
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);
  void set_groomed_transform(int i, std::vector<double> transform);

  std::vector<std::vector<double>> get_procrustes_transforms() const;
  void set_procrustes_transforms(std::vector<std::vector<double>> transforms);

  std::map<std::string, std::string> get_group_values() const;
  std::string get_group_value(std::string group_name);

  void set_group_values(const std::map<std::string, std::string>& group_values);

  std::map<std::string, std::string> get_extra_values() const;
  void set_extra_values(std::map<std::string, std::string> extra_values);

  std::map<std::string, std::string> get_table_values() const;
  void set_table_values(std::map<std::string, std::string> table_values);

  std::string get_display_name();
  void set_display_name(std::string display_name);

 private:
  int number_of_domains_ = 0;

  std::string display_name_;
  std::vector<std::string> original_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::vector<std::string> local_particle_filenames_;
  std::vector<std::string> world_particle_filenames_;
  std::vector<std::string> landmarks_filenames_;
  std::vector<std::string> constraints_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;
  std::vector<std::vector<double>> procrustes_transforms_;

  std::map<std::string, std::string> feature_filenames_;
  std::map<std::string, std::string> group_values_;
  std::map<std::string, std::string> extra_values_;
  std::map<std::string, std::string> table_values_;

  std::vector<DomainType> original_domain_types_;
  std::vector<DomainType> groomed_domain_types_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-09-13 at 16:52:36 +0000
