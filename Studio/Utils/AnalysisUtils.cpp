#include "AnalysisUtils.h"

#include <Data/Session.h>
namespace shapeworks {

//---------------------------------------------------------------------------
ParticleSystemEvaluation AnalysisUtils::get_local_particle_system(Session* session, int domain) {
  Eigen::MatrixXd matrix;
  int num_shapes = session->get_num_shapes();
  int num_total_particles = session->get_num_particles();
  if (num_shapes == 0 || num_total_particles == 0) {
    return ParticleSystemEvaluation{matrix};
  }

  auto shapes = session->get_shapes();
  int num_particles_domain = shapes[0]->get_particles().get_local_particles(domain).size() / 3;
  matrix.resize(num_particles_domain * 3, num_shapes);

  for (int i = 0; i < num_shapes; i++) {
    auto particles = shapes[i]->get_particles().get_local_particles(domain);
    for (int j = 0; j < num_particles_domain; j++) {
      matrix(j * 3 + 0, i) = particles[j * 3 + 0];
      matrix(j * 3 + 1, i) = particles[j * 3 + 1];
      matrix(j * 3 + 2, i) = particles[j * 3 + 2];
    }
  }

  return ParticleSystemEvaluation{matrix};
}
//---------------------------------------------------------------------------

}  // namespace shapeworks
