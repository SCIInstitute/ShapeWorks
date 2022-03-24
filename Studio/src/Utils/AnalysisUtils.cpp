#include "AnalysisUtils.h"

#include <Data/Session.h>
namespace shapeworks {

//---------------------------------------------------------------------------
ParticleSystem AnalysisUtils::get_local_particle_system(Session* session, int domain) {
  Eigen::MatrixXd matrix;
  int num_shapes = session->get_num_shapes();
  int num_particles = session->get_num_particles();
  matrix.resize(num_particles * 3, num_shapes);
  for (auto& shape : session->get_shapes()) {
    auto particles = shape->get_particles().get_local_particles(domain);
  }

  auto shapes = session->get_shapes();
  for (int i = 0; i < num_shapes; i++) {
    auto particles = shapes[i]->get_particles().get_local_particles(domain);
    for (int j = 0; j < num_particles; j++) {
      matrix(j, i) = particles[j];
    }
  }

  return ParticleSystem{matrix};
}
//---------------------------------------------------------------------------

}  // namespace shapeworks
