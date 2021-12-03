---
title: Libs/Project/Parameters.h

---

# Libs/Project/Parameters.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Parameters](../Classes/classshapeworks_1_1Parameters.md)** <br>Parameter settings.  |




## Source code

```cpp
#pragma once

#include <map>
#include <string>

#include "Variant.h"

namespace shapeworks {


class Parameters {

public:

  static constexpr const char* ANALYSIS_PARAMS = "analysis";
  static constexpr const char* GROOM_PARAMS = "groom";
  static constexpr const char* OPTIMIZE_PARAMS = "optimize";
  static constexpr const char* STUDIO_PARAMS = "studio";
  static constexpr const char* PROJECT_PARAMS = "project";
  static constexpr const char* DEEPSSM_PARAMS = "deepssm";

  Variant get(std::string key, Variant default_value);

  bool key_exists(std::string key);

  void set(std::string key, Variant value);

  void remove_entry(std::string key);

  void set_map(std::map<std::string, std::string> map);

  std::map<std::string, std::string> get_map();

  void reset_parameters();

private:

  std::map<std::string, std::string> map_;
};
}
```


-------------------------------

Updated on 2021-12-03 at 20:11:58 +0000
