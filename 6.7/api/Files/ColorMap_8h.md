---
title: Studio/Visualization/ColorMap.h

---

# Studio/Visualization/ColorMap.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ColorMap](../Classes/classshapeworks_1_1ColorMap.md)**  |
| class | **[shapeworks::ColorMaps](../Classes/classshapeworks_1_1ColorMaps.md)**  |




## Source code

```cpp
#pragma once

#include <vtkColorSeries.h>
#include <vtkSmartPointer.h>

#include <QColor>
#include <QStringList>
#include <vector>

namespace shapeworks {

class ColorMap {
 public:
  QString name_;
  vtkSmartPointer<vtkColorSeries> color_series_;

  void construct_lookup_table(vtkSmartPointer<vtkLookupTable> lut);

  static vtkColor3ub convert(QColor color);

  void set_discrete_mode(bool discrete_mode) { discrete_mode_ = discrete_mode; }
  void set_reverse_mode(bool reverse_mode) { reverse_mode_ = reverse_mode; }

 private:
  bool discrete_mode_ = false;
  bool reverse_mode_ = false;
};

class ColorMaps : public std::vector<ColorMap> {
 public:
  ColorMaps();

  vtkSmartPointer<vtkColorSeries> get_color_series(int index) {
    if (index < 0) {
      index = 0;
    }
    if (index > size() - 1) {
      index = size() - 1;
    }

    return (*this)[index].color_series_;
  }

  ColorMap get_color_map(int index) {
    if (index < 0) {
      index = 0;
    }
    if (index > size() - 1) {
      index = size() - 1;
    }

    return (*this)[index];
  }
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-10-11 at 04:45:09 +0000
