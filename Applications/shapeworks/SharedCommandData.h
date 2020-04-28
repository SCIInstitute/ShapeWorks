#pragma once

#include "Image.h"
#include "Mesh.h"
#include "ParticleSystem.h"
#include <string>

namespace shapeworks {

struct SharedCommandData
{
  Image image;
  Image::Region region;
  Mesh mesh;
  ParticleSystem particleSystem;
  Transform::Pointer transform;

  SharedCommandData() { transform = Transform::New(); }

};

} // shapeworks
