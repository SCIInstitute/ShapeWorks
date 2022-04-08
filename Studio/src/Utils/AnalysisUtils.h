#pragma once

#include <Libs/Particles/ParticleSystem.h>

namespace shapeworks {

  class Session;

  class AnalysisUtils {
  public:

    static ParticleSystem get_local_particle_system(Session* session, int domain);


  };

}
