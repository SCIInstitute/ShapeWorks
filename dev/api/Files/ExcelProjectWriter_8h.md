---
title: Libs/Project/ExcelProjectWriter.h

---

# Libs/Project/ExcelProjectWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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
  static bool write_project(Project& project, std::string filename);

private:

};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-08-13 at 05:00:37 +0000
