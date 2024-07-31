---
title: Libs/Project/ExcelProjectReader.h

---

# Libs/Project/ExcelProjectReader.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ExcelProjectReader](../Classes/classshapeworks_1_1ExcelProjectReader.md)** <br>Excel file format reader for [Project]().  |




## Source code

```cpp
#pragma once

#include "ProjectReader.h"

namespace shapeworks {


class ExcelProjectReader : public ProjectReader {
 public:
  ExcelProjectReader(Project &project);
  ~ExcelProjectReader();

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

Updated on 2024-07-31 at 00:01:36 +0000
