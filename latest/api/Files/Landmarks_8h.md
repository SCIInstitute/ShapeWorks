---
title: Libs/Project/Landmarks.h

---

# Libs/Project/Landmarks.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Landmark](../Classes/classshapeworks_1_1Landmark.md)**  |
| class | **[shapeworks::Landmarks](../Classes/classshapeworks_1_1Landmarks.md)** <br>Representation of the project's landmarks.  |




## Source code

```cpp
#pragma once

#include <map>
#include <string>
#include <vector>

namespace shapeworks {

class Landmark {
 public:
  std::string name_;
};


class Landmarks {
 public:
  Landmarks();
  ~Landmarks();

  int get_number_of_landmarks();

  Landmark get_landmark(int i);

  void set_landmark(int i, const Landmark& landmark);

  void new_landmark();

 private:
  std::vector<Landmark> landmarks_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-23 at 16:40:07 -0600
