#include "ParticleColors.h"

namespace shapeworks {

vtkSmartPointer<vtkLookupTable> ParticleColors::construct(ParticleColorsType type, int num) {
  auto lut = vtkSmartPointer<vtkLookupTable>::New();
  lut->SetNumberOfTableValues(21);

  return lut;
}

}  // namespace shapeworks