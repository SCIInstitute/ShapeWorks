---
title: Libs/Common/Exception.h

---

# Libs/Common/Exception.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::shapeworks_exception](../Classes/classshapeworks_1_1shapeworks__exception.md)**  |




## Source code

```cpp
#pragma once

#include <stdexcept>

namespace shapeworks {

class shapeworks_exception : public std::runtime_error
{
public:
  shapeworks_exception(char const* const message) throw();
  shapeworks_exception(std::string const& message) throw();

};

} // shapeworks
```


-------------------------------

Updated on 2023-05-04 at 20:03:05 +0000