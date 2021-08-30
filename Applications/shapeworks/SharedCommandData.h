#pragma once

#include "Image.h"
#include "Mesh.h"
#include "ParticleSystem.h"
#include "Shapeworks.h"
#include <string>

namespace shapeworks {

struct SharedCommandData
{
  Image image;
  std::unique_ptr<Mesh> mesh;
  PhysicalRegion region;
  ParticleSystem particleSystem;
  Field field;

  bool validImage() const { return image.image != nullptr; }
  bool validMesh() const { return mesh != nullptr; }
  bool validParticleSystem() const {return particleSystem.N() >= 1 && particleSystem.D() >= 1; }
};


} // shapeworks
