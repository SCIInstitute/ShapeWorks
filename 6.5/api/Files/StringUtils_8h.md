---
title: Libs/Utils/StringUtils.h

---

# Libs/Utils/StringUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

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
  static std::string getBaseFilenameWithoutExtension(std::string const& pathname);
  static std::string toLower(std::string s);
  static bool hasSuffix(std::string const& filename, std::string const& suffix);
  static std::vector<std::string> getFileNamesFromPaths(const std::vector<std::string>& paths);

  static std::string getLowerExtension(std::string const& filename);

  // safely convert a const char* to string, allowing for nullptr
  static std::string safeString(const char* s) { return s == nullptr ? std::string() : s; }

  static std::string replace_string(std::string str, const std::string& search, const std::string& replace);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-08-02 at 17:10:01 +0000