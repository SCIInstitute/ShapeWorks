---
title: Libs/Project/ProjectUtils.h

---

# Libs/Project/ProjectUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |
| **[shapeworks::project](../Namespaces/namespaceshapeworks_1_1project.md)**  |
| **[shapeworks::project::prefixes](../Namespaces/namespaceshapeworks_1_1project_1_1prefixes.md)**  |
| **[shapeworks::project::types](../Namespaces/namespaceshapeworks_1_1project_1_1types.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ProjectUtils](../Classes/classshapeworks_1_1ProjectUtils.md)**  |




## Source code

```cpp
#pragma once

// std
#include <map>
#include <string>
#include <vector>

// tsl
#include <ordered_map.h>

// vtk
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

// shapeworks
#include <Libs/Optimize/Domain/DomainType.h>

namespace shapeworks {

class Project;
class Subject;

namespace project::prefixes {
static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
static constexpr const char* LANDMARKS_FILE_PREFIX = "landmarks_file_";

static constexpr const char* SHAPE_PREFIX = "shape_";
static constexpr const char* MESH_PREFIX = "mesh_";
static constexpr const char* CONTOUR_PREFIX = "contour_";

static constexpr const char* GROOMED_PREFIX = "groomed_";
static constexpr const char* GROOMED_CONTOUR_PREFIX = "groomed_contour_";

static constexpr const char* CONSTRAINTS_PREFIX = "constraints_";

static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "alignment_";
static constexpr const char* PROCRUSTES_TRANSFORMS_PREFIX = "procrustes_";
static constexpr const char* IMAGE_PREFIX = "image_";
static constexpr const char* FEATURE_PREFIX = "feature_";
static constexpr const char* GROUP_PREFIX = "group_";

static constexpr const char* LOCAL_PARTICLES = "local_particles";
static constexpr const char* WORLD_PARTICLES = "world_particles";

static constexpr const char* LOCAL_PARTICLES_PREFIX = "local_particles_";
static constexpr const char* WORLD_PARTICLES_PREFIX = "world_particles_";

}  // namespace project::prefixes

namespace project::types {
using StringMap = tsl::ordered_map<std::string, std::string>;
using StringList = std::vector<std::string>;
using StringMapList = std::vector<StringMap>;
using StringMultiMap = std::map<std::string, StringMap>;
}  // namespace project::types

class ProjectUtils {
 public:
  using StringList = project::types::StringList;
  using StringMap = project::types::StringMap;

  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  static vtkSmartPointer<vtkTransform> convert_transform(std::string string);

  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);

  static StringList determine_domain_names(StringList keys);

  static void determine_domain_types(Project* project, StringMap key_map);

  static StringList get_input_prefixes();

  static StringList get_groomed_prefixes();

  static StringList get_original_keys(StringList domain_names, StringMap key_map);

  static StringList get_values(StringList prefixes, StringList domain_names, StringMap key_map);

  static std::vector<std::vector<double>> get_transforms(std::string prefix, StringList domain_names,
                                                         StringMap key_map);

  static StringMap get_value_map(std::vector<std::string> prefix, StringMap key_map);

  static StringMap get_extra_columns(StringMap key_map);

  static DomainType determine_domain_type(std::string filename);

  static bool starts_with(std::string str, std::string prefix);

  static std::string transform_to_string(std::vector<double> transform);

  static std::vector<std::string> convert_domain_types(std::vector<DomainType> domain_types);
  static std::vector<std::string> convert_groomed_domain_types(std::vector<DomainType> domain_types);

  static StringMap convert_subject_to_map(Project* project, Subject* subject);
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-18 at 06:53:19 +0000
