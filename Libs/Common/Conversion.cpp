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

Image Mesh::toImage(Vector3 spacing, Dims size, Point3 origin) const
{
  return rasterize(*this, spacing, size, origin);
}

Image Mesh::toDistanceTransform(Vector3 spacing, Dims size, Point3 origin) const
{
  return rasterize(*this, spacing, size, origin).antialias(30, 0.0).computeDT();
}

} // shapeworks
