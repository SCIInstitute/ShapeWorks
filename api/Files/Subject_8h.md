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

#include <string>
#include <vector>
#include <map>

#include <Libs/Optimize/ParticleSystem/DomainType.h>

namespace shapeworks {


class Subject {

public:

  Subject();
  ~Subject();

  void set_segmentation_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_segmentation_filenames();

  std::vector<DomainType> get_domain_types();

  void set_groomed_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_groomed_filenames();

  void set_local_particle_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_local_particle_filenames();

  void set_world_particle_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_world_particle_filenames();

  void set_landmarks_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_landmarks_filenames();

  void set_number_of_domains(int number_of_domains);
  int get_number_of_domains();

  void set_image_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_image_filenames();

  std::map<std::string, std::string> get_feature_filenames() const;
  void set_feature_filenames(const std::map<std::string, std::string>& feature_filenames);

  std::vector<std::vector<double>> get_groomed_transforms() const;
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);

  std::vector<std::vector<double>> get_procrustes_transforms() const;
  void set_procrustes_transforms(std::vector<std::vector<double>> transforms);

  std::map<std::string, std::string> get_group_values() const;
  std::string get_group_value(std::string group_name);

  void set_group_values(const std::map<std::string, std::string>& group_values);

  std::map<std::string, std::string> get_extra_values() const;
  void set_extra_values(std::map<std::string, std::string> extra_values);

  std::string get_display_name();
  void set_display_name(std::string display_name);

private:

  int number_of_domains_ = 0;

  std::string display_name_;
  std::vector<std::string> image_filenames_;
  std::vector<std::string> segmentation_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::vector<std::string> local_particle_filenames_;
  std::vector<std::string> world_particle_filenames_;
  std::vector<std::string> landmarks_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;
  std::vector<std::vector<double>> procrustes_transforms_;

  std::map<std::string, std::string> feature_filenames_;
  std::map<std::string, std::string> group_values_;
  std::map<std::string, std::string> extra_values_;
};
}
```


-------------------------------

Updated on 2021-11-15 at 01:30:08 +0000
