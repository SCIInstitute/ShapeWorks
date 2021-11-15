---
title: Libs/Utils/StringUtils.h

---

# Libs/Utils/StringUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StringUtils](../Classes/classshapeworks_1_1StringUtils.md)** <br>String utility functions.  |




## Source code

```cpp
#pragma once

#include <string>
#include <vector>

namespace shapeworks {

class StringUtils {

public:
  static std::string removeExtension(std::string const& filename);
  static std::string getPath(std::string const& filename);
  static std::string getFilename(std::string const& pathname);
  static std::string toLower(std::string s);
  static bool hasSuffix(std::string const& filename, std::string const& suffix);
  static std::vector<std::string> getFileNamesFromPaths(const std::vector<std::string>& paths);

  static std::string getFileNameWithoutExtension(std::string path);

};

}
```


-------------------------------

Updated on 2021-11-15 at 01:30:08 +0000
