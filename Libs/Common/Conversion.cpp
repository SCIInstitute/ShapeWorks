#include "Image.h"
#include "Mesh.h"

namespace shapeworks {

std::unique_ptr<Mesh> Image::toMesh(Image::PixelType isovalue) const
{
  return std::make_unique<Mesh>(getPolyData(*this, isovalue));
}

std::unique_ptr<Mesh> Image::toMesh(double levelset, double reduction, double angle, int leveliterations, int meshiterations, bool preservetopology) const
{
  Mesh mesh(march(*this, levelset));
  mesh.smooth(leveliterations).decimate(reduction, angle).smooth(meshiterations);
  return std::make_unique<Mesh>(mesh);
}

} // shapeworks
