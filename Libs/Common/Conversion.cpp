#include "Image.h"
#include "Mesh.h"

namespace shapeworks {

// NOTE: needs to be here to avoid a dependency loop since Mesh already depends on Image.

Mesh Image::toMesh(Image::PixelType isovalue) const
{
  return getPolyData(*this, isovalue);
}

} // shapeworks
