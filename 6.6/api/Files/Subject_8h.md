---
title: Libs/Project/Subject.h

---

# Libs/Project/Subject.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md)** <br>Representation of a single subject.  |




## Source code

```cpp
#pragma once

#include <Libs/Optimize/Domain/DomainType.h>

#include <map>
#include <string>
#include <vector>

#include "ProjectUtils.h"

namespace shapeworks {


class Subject {
 public:
  using StringMap = project::types::StringMap;
  using StringList = project::types::StringList;

  Subject();
  ~Subject();

  void set_original_filenames(StringList filenames);
  StringList get_original_filenames();

  void set_groomed_filenames(StringList filenames);
  StringList get_groomed_filenames();

  void set_local_particle_filenames(StringList filenames);
  StringList get_local_particle_filenames();

  void set_world_particle_filenames(StringList filenames);
  StringList get_world_particle_filenames();

  void set_landmarks_filenames(StringList filenames);
  StringList get_landmarks_filenames();

  void set_constraints_filenames(StringList filenames);
  StringList get_constraints_filenames();

  void set_number_of_domains(int number_of_domains);
  int get_number_of_domains();

  StringMap get_feature_filenames() const;
  void set_feature_filenames(const StringMap& feature_filenames);

  std::vector<std::vector<double>> get_groomed_transforms() const;
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);
  void set_groomed_transform(int i, std::vector<double> transform);

  std::vector<std::vector<double>> get_procrustes_transforms() const;
  void set_procrustes_transforms(std::vector<std::vector<double>> transforms);

  StringMap get_group_values() const;
  std::string get_group_value(std::string group_name);

  void set_group_values(const StringMap& group_values);

  StringMap get_extra_values() const;
  void set_extra_values(StringMap extra_values);

  StringMap get_table_values() const;
  void set_table_values(StringMap table_values);

  std::string get_display_name();
  void set_display_name(std::string display_name);

  bool is_fixed();
  void set_fixed(bool fixed);

  bool is_excluded();
  void set_excluded(bool excluded);

  std::string get_notes();
  void set_notes(std::string notes);

 private:
  int number_of_domains_ = 0;

  std::string display_name_;
  bool fixed_ = false;
  bool excluded_ = false;
  StringList original_filenames_;
  StringList groomed_filenames_;
  StringList local_particle_filenames_;
  StringList world_particle_filenames_;
  StringList landmarks_filenames_;
  StringList constraints_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;
  std::vector<std::vector<double>> procrustes_transforms_;

  StringMap feature_filenames_;
  StringMap group_values_;
  StringMap extra_values_;
  StringMap table_values_;
  std::string notes_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-02 at 21:06:13 +0000