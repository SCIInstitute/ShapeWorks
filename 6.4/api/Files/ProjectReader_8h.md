---
title: Libs/Project/ProjectReader.h

---

# Libs/Project/ProjectReader.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ProjectReader](../Classes/classshapeworks_1_1ProjectReader.md)** <br>Base class for [Project](../Classes/classshapeworks_1_1Project.md) readers.  |




## Source code

```cpp
#pragma once

#include "Project.h"
#include "ProjectUtils.h"

namespace shapeworks {


class ProjectReader {
 public:
  using StringMapList = project::types::StringMapList;
  using StringMap = project::types::StringMap;
  using StringList = project::types::StringList;
  using StringMultiMap = project::types::StringMultiMap;

  ProjectReader(Project &project);

  virtual ~ProjectReader() = default;

  virtual bool read_project(std::string filename) = 0;

  virtual StringMap get_parameters(std::string name) = 0;

  virtual StringMultiMap get_multi_parameters(std::string name) = 0;

 protected:
  void load_subjects(StringMapList list);

  void load_parameters();

  void load_parameter(std::string name, StringMap map);

  void load_landmark_definitions(StringMapList list);

  static StringList get_keys(StringMap map);

  bool contains(StringMap map, std::string key);

  Project &project_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-03-11 at 06:26:44 +0000
