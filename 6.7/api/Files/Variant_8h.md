---
title: Libs/Project/Variant.h

---

# Libs/Project/Variant.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Variant](../Classes/classshapeworks_1_1Variant.md)** <br>[Variant]() class to represent multiple types.  |




## Source code

```cpp
#pragma once

#include <sstream>
#include <string>
#include <vector>
#include <Utils/StringUtils.h>

namespace shapeworks {

template <typename T>
std::string variant_to_string(T begin, T end) {
  std::stringstream ss;
  bool first = true;
  for (; begin != end; begin++) {
    if (!first) {
      ss << " ";
    }
    if constexpr (std::is_same<T, std::string>::value) {
      // cast to string and replace spaces
      std::string s(*begin);
      ss << StringUtils::replace_string(s," ", "%20");
    } else {
      ss << *begin;
    }
    first = false;
  }
  return ss.str();
}


class Variant {
 public:
  Variant() : str_(), valid_(false) {}
  Variant(const std::string& v) : str_(v), valid_(true) {}
  Variant(int v) : str_(std::to_string(v)), valid_(true) {}
  Variant(double v) : str_(std::to_string(v)), valid_(true) {}
  Variant(const char* v) : str_(v), valid_(true) {}
  Variant(bool v) : str_(v ? "true" : "false"), valid_(true) {}
  Variant(std::vector<double> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}
  Variant(std::vector<int> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}
  Variant(std::vector<bool> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}
  Variant(std::vector<std::string> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}

  operator std::string() const;
  operator bool() const;
  operator int() const;
  operator unsigned int() const;
  operator long() const;
  operator unsigned long() const;
  operator float() const;
  operator double() const;
  operator std::vector<double>() const;
  operator std::vector<int>() const;
  operator std::vector<bool>() const;
  operator std::vector<std::string>() const;

 private:
  std::string str_;
  bool valid_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-08-22 at 08:23:43 +0000
