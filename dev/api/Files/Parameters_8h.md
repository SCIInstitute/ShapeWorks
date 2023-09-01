---
title: Libs/Project/Parameters.h

---

# Libs/Project/Parameters.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Parameters](../Classes/classshapeworks_1_1Parameters.md)** <br>Parameter settings.  |




## Source code

```cpp
#pragma once

#include "ProjectUtils.h"

#include "Variant.h"

namespace shapeworks {


class Parameters {
  using StringMap = project::types::StringMap;

 public:
  static constexpr const char* ANALYSIS_PARAMS = "analysis";
  static constexpr const char* GROOM_PARAMS = "groom";
  static constexpr const char* OPTIMIZE_PARAMS = "optimize";
  static constexpr const char* STUDIO_PARAMS = "studio";
  static constexpr const char* PROJECT_PARAMS = "project";
  static constexpr const char* DEEPSSM_PARAMS = "deepssm";

  Parameters();

  explicit Parameters(StringMap map);

  Variant get(std::string key, Variant default_value);

  bool key_exists(std::string key);

  void set(std::string key, Variant value);

  void remove_entry(std::string key);

  void set_map(StringMap map);

  StringMap get_map() const;

  void reset_parameters();

 private:
  StringMap map_;

};

}  // namespace shapeworks
```


-------------------------------

Updated on 2023-09-01 at 05:26:30 +0000
