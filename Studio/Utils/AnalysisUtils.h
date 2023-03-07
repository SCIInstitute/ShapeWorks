#pragma once

#include <Particles/ParticleSystemEvaluation.h>

namespace shapeworks {

  class Session;

  class AnalysisUtils {
  public:

    static ParticleSystemEvaluation get_local_particle_system(Session* session, int domain);


  };

}
