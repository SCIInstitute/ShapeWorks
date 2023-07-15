---
title: Libs/Project/JsonProjectWriter.h

---

# Libs/Project/JsonProjectWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::JsonProjectWriter](../Classes/classshapeworks_1_1JsonProjectWriter.md)** <br>JSON file format writer for [Project]().  |




## Source code

```cpp
#pragma once

#include "Project.h"

namespace shapeworks {


class JsonProjectWriter {
 public:
  static bool write_project(Project& project, std::string filename);

 private:
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-07-15 at 03:21:18 +0000
