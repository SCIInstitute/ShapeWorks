#include "Image.h"
#include "Mesh.h"

namespace shapeworks {

std::unique_ptr<Mesh> Image::toMesh(Image::PixelType isovalue) const
{
  return std::make_unique<Mesh>(getPolyData(*this, isovalue));
}

} // shapeworks
