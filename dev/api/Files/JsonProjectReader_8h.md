---
title: Libs/Project/JsonProjectReader.h

---

# Libs/Project/JsonProjectReader.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::JsonProjectReader](../Classes/classshapeworks_1_1JsonProjectReader.md)** <br>JSON file format reader for [Project]().  |




## Source code

```cpp
#pragma once

#include "ProjectReader.h"

namespace shapeworks {


class JsonProjectReader : public ProjectReader {
 public:
  JsonProjectReader(Project &project);
  ~JsonProjectReader();

  bool read_project(std::string filename) override;

  StringMap get_parameters(std::string name) override;

  StringMultiMap get_multi_parameters(std::string name) override;

 private:
  // pimpl container
  class Container;
  std::unique_ptr<Container> container_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-05-04 at 20:03:05 +0000