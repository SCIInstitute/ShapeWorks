---
title: Libs/Analyze/StudioEnums.h

---

# Libs/Analyze/StudioEnums.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |




## Source code

```cpp
#pragma once

#include <string>

namespace shapeworks {

//-----------------------------------------------------------------------------
enum DisplayMode { Original = 0, Groomed = 1, Reconstructed = 2 };

//-----------------------------------------------------------------------------
std::string display_mode_to_string(DisplayMode display_mode);

//-----------------------------------------------------------------------------
DisplayMode string_to_display_mode(std::string str);

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-12-10 at 09:04:10 +0000
