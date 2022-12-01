---
title: Libs/Project/Variant.h

---

# Libs/Project/Variant.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

namespace shapeworks {

template <typename T>
std::string variant_to_string(T begin, T end) {
  std::stringstream ss;
  bool first = true;
  for (; begin != end; begin++) {
    if (!first) {
      ss << " ";
    }
    ss << *begin;
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

  operator std::string();
  operator bool();
  operator int();
  operator unsigned int();
  operator long();
  operator unsigned long();
  operator float();
  operator double();
  operator std::vector<double>();
  operator std::vector<int>();
  operator std::vector<bool>();

 private:
  std::string str_;
  bool valid_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-12-01 at 06:24:23 +0000
