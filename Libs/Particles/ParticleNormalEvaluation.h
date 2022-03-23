#pragma once

#include <memory>

#include <Libs/Particles/ParticleSystem.h>

namespace shapeworks {

//! Evaluation of particle using surface normals to determine which particles have inconsistent normals across subjects
/*!
 * The ParticleNormalEvaluation class evaluates the surface normals at each particle and determines if they are
 * consistent across subjects (e.g. good particles), or inconsistent across subject (e.g. bad particles).
 *
 */

class VtkMeshWrapper;
class ParticleNormalEvaluation {
 public:
  static std::vector<bool> evalaute_particle_normals(ParticleSystem local_particles, std::vector<std::shared_ptr<VtkMeshWrapper>> meshes);

 private:
};

}  // namespace shapeworks
