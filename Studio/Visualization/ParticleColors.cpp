#include "ParticleColors.h"

namespace shapeworks {

vtkSmartPointer<vtkLookupTable> ParticleColors::construct_distinct() {
  auto lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(21);

  int v = 0;
  auto add_color = [&](int r, int g, int b) { lut->SetTableValue(v++, r / 255.0, g / 255.0, b / 255.0, 1.0); };

  add_color(230, 25, 75);
  add_color(60, 180, 75);
  add_color(255, 225, 25);
  add_color(0, 130, 200);
  add_color(245, 130, 48);
  add_color(145, 30, 180);
  add_color(70, 240, 240);
  add_color(240, 50, 230);
  add_color(210, 245, 60);
  add_color(250, 190, 212);
  add_color(0, 128, 128);
  add_color(220, 190, 255);
  add_color(170, 110, 40);
  add_color(255, 250, 200);
  add_color(128, 0, 0);
  add_color(170, 255, 195);
  add_color(128, 128, 0);
  add_color(255, 215, 180);
  add_color(0, 0, 128);
  add_color(128, 128, 128);
  add_color(255, 255, 255);

  return lut;
}

}  // namespace shapeworks