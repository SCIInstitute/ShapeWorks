---
title: Libs/Project/ExcelProjectWriter.h

---

# Libs/Project/ExcelProjectWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ExcelProjectWriter](../Classes/classshapeworks_1_1ExcelProjectWriter.md)** <br>Excel file format writer for [Project]().  |




## Source code

```cpp
#pragma once

#include "Project.h"

namespace shapeworks {


class ExcelProjectWriter {
 public:
  static bool write_project(Project& project, const std::string& filename);

 private:
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-04-29 at 18:09:09 +0000