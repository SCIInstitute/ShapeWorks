#include "ColorMap.h"

#include <vtkColorSeries.h>
#include <vtkColorTransferFunction.h>
#include <vtkLookupTable.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
void ColorMap::construct_lookup_table(vtkSmartPointer<vtkLookupTable> lut) {
  auto color_transfer = vtkSmartPointer<vtkColorTransferFunction>::New();

  int num_colors = color_series_->GetNumberOfColors();
  for (int i = 0; i < num_colors; i++) {
    double ratio = static_cast<double>(i) / (num_colors - 1);
    vtkColor3ub color = color_series_->GetColor(i);
    std::cerr << i << " : " << color << "\n";
    color_transfer->AddRGBPoint(ratio, color.GetRed() / 255.0, color.GetGreen() / 255.0, color.GetBlue() / 255.0);
  }

  lut->SetNumberOfTableValues(100);
  for (int i = 0; i < 100; i++) {
    double rgb[3];
    color_transfer->GetColor(i / 99.0, rgb);
    lut->SetTableValue(i, rgb[0], rgb[1], rgb[2]);
  }
}

//-----------------------------------------------------------------------------
vtkColor3ub ColorMap::convert(QColor color) { return vtkColor3ub(color.red(), color.green(), color.blue()); }

//-----------------------------------------------------------------------------
ColorMaps::ColorMaps() {
  ColorMap a;

  ColorMap map;
  map.name_ = "Rainbow";
  map.color_series_ = vtkSmartPointer<vtkColorSeries>::New();
  map.color_series_->ClearColors();
  map.color_series_->AddColor(ColorMap::convert(Qt::blue));
  map.color_series_->AddColor(ColorMap::convert(Qt::cyan));
  map.color_series_->AddColor(ColorMap::convert(Qt::green));
  map.color_series_->AddColor(ColorMap::convert(Qt::yellow));
  map.color_series_->AddColor(ColorMap::convert(Qt::red));
  push_back(map);

  auto add_vtk_series = [=](auto series) {
    vtkNew<vtkColorSeries> colorSeries;
    int colorSeriesEnum = series;
    colorSeries->SetColorScheme(colorSeriesEnum);
    ColorMap map;
    map.color_series_ = colorSeries;
    map.name_ = colorSeries->GetColorSchemeName();
    push_back(map);
  };

  add_vtk_series(vtkColorSeries::SPECTRUM);
  add_vtk_series(vtkColorSeries::WARM);
  add_vtk_series(vtkColorSeries::BREWER_DIVERGING_BROWN_BLUE_GREEN_3);
}

}  // namespace shapeworks
