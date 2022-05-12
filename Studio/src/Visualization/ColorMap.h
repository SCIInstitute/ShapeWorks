#pragma once

#include <vtkColorSeries.h>
#include <vtkSmartPointer.h>

#include <QStringList>
#include <QColor>
#include <vector>

namespace shapeworks {

class ColorMap {
 public:
  QString name_;
  vtkSmartPointer<vtkColorSeries> color_series_;

  void construct_lookup_table(vtkSmartPointer<vtkLookupTable> lut);

  static vtkColor3ub convert(QColor color);

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
