#include "ColorMap.h"

#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
void ColorMap::construct_lookup_table(vtkSmartPointer<vtkLookupTable> lut) {
  auto colors = color_series_;

  if (reverse_mode_) {
    colors = vtkSmartPointer<vtkColorSeries>::New();
    colors->ClearColors();
    for (int i = color_series_->GetNumberOfColors() - 1; i >= 0; i--) {
      colors->AddColor(color_series_->GetColor(i));
    }
  }

  if (discrete_mode_) {
    colors->BuildLookupTable(lut, vtkColorSeries::LUTMode::ORDINAL);
  } else {
    auto color_transfer = vtkSmartPointer<vtkColorTransferFunction>::New();

    int num_colors = colors->GetNumberOfColors();
    for (int i = 0; i < num_colors; i++) {
      double ratio = static_cast<double>(i) / (num_colors - 1);
      vtkColor3ub color = colors->GetColor(i);
      color_transfer->AddRGBPoint(ratio, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
    }

    constexpr double num_values = 250;
    lut->SetNumberOfTableValues(num_values);
    for (int i = 0; i < num_values; i++) {
      double rgb[3];
      color_transfer->GetColor(i / (num_values-1), rgb);
      lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
    }
  }
  lut->Modified();
}

//-----------------------------------------------------------------------------
vtkColor3ub ColorMap::convert(QColor color) { return vtkColor3ub(color.red(), color.green(), color.blue()); }

//-----------------------------------------------------------------------------
ColorMaps::ColorMaps() {
  auto add_custom_series = [&](auto name, std::vector<QColor> colors) {
    ColorMap map;
    map.name_ = name;
    map.color_series_ = vtkSmartPointer<vtkColorSeries>::New();
    map.color_series_->ClearColors();
    for (const auto& color : colors) {
      map.color_series_->AddColor(ColorMap::convert(color));
    }
    push_back(map);
  };

  add_custom_series("Rainbow", {Qt::blue, Qt::cyan, Qt::green, Qt::yellow, Qt::red});
  add_custom_series("Grayscale", {Qt::black, Qt::darkGray, Qt::lightGray, Qt::white});
  add_custom_series("Blue White Red", {Qt::blue, Qt::white, Qt::red});
  add_custom_series("Magenta to Green", {QColor(191, 53, 136), QColor(208, 121, 178), Qt::white, QColor(155, 196, 128),
                                         QColor(102, 167, 61)});
  add_custom_series("Black-Body Radiation", {Qt::black, Qt::red, Qt::yellow, Qt::white});
  add_custom_series("Blue to Yellow", {Qt::blue, Qt::yellow});

  auto add_vtk_series = [&](auto series) {
    vtkNew<vtkColorSeries> colorSeries;
    int colorSeriesEnum = series;
    colorSeries->SetColorScheme(colorSeriesEnum);
    ColorMap map;
    map.color_series_ = colorSeries;
    map.name_ = colorSeries->GetColorSchemeName();
    push_back(map);
  };

  for (int i = vtkColorSeries::SPECTRUM; i < vtkColorSeries::CUSTOM; i++) {
    add_vtk_series(i);
  }
  add_custom_series("Blue and Red", {Qt::blue, Qt::red});
}

}  // namespace shapeworks
