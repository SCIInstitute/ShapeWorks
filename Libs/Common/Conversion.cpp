#include "Image.h"
#include "Mesh.h"

namespace shapeworks {

// NOTE: these two functions from Image need to be here to avoid a dependency loop since Mesh
// already depends on Image.

Mesh Image::toMesh(Image::PixelType isovalue) const
{
  return getPolyData(*this, isovalue);
}

Mesh Image::toMesh(double levelset, double reduction, double angle, int leveliterations, int meshiterations, bool preservetopology) const
{
  Mesh mesh(march(*this, levelset));
  mesh.smooth(leveliterations).decimate(reduction, angle, preservetopology).smooth(meshiterations);
  return mesh;
}

} // shapeworks
