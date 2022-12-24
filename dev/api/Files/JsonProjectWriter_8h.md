---
title: Libs/Project/JsonProjectWriter.h

---

# Libs/Project/JsonProjectWriter.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-12-24 at 10:20:12 +0000
