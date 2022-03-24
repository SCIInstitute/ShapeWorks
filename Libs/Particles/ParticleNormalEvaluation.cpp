#include "ParticleNormalEvaluation.h"

namespace shapeworks {

std::vector<bool> ParticleNormalEvaluation::evalaute_particle_normals(
    ParticleSystem local_particles, std::vector<std::shared_ptr<VtkMeshWrapper>> meshes) {
  std::vector<bool> result;
  Eigen::MatrixXd particles = local_particles.Particles();

  std::vector<std::vector<double>> thetas;
  std::vector<std::vector<double>> phis;

  std::cerr << "local_particles.rows() = " << particles.rows() << "\n";
  std::cerr << "local_particles.cols() = " << particles.cols() << "\n";



  return result;
}

}  // namespace shapeworks
