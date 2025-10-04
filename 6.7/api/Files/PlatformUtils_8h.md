---
title: Libs/Utils/PlatformUtils.h

---

# Libs/Utils/PlatformUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PlatformUtils](../Classes/classshapeworks_1_1PlatformUtils.md)** <br>Platform utility functions.  |




## Source code

```cpp

namespace shapeworks {

class PlatformUtils {
 public:
  static bool is_windows() {
#ifdef _WIN32
    return true;
#endif
    return false;
  }

  static bool is_linux() {
#ifdef __linux__
    return true;
#endif
    return false;
  }

  static bool is_macos() {
#ifdef __APPLE__
    return true;
#endif
    return false;
  }
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-04 at 01:42:58 +0000
