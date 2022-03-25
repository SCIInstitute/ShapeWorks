#include "ParticleNormalEvaluation.h"

#include <Utils.h>
#include <VtkMeshWrapper.h>

namespace shapeworks {

std::vector<bool> ParticleNormalEvaluation::evaluate_particle_normals(
    ParticleSystem local_particles, std::vector<std::shared_ptr<VtkMeshWrapper>> meshes, double max_angle_degrees) {
  std::vector<bool> result;
  Eigen::MatrixXd particles = local_particles.Particles();

  std::vector<std::vector<double>> thetas;
  std::vector<std::vector<double>> phis;

  int num_shapes = local_particles.N();
  int num_particles = local_particles.D() / 3;

  std::cerr << "local_particles.rows() = " << particles.rows() << "\n";
  std::cerr << "local_particles.cols() = " << particles.cols() << "\n";
  std::cerr << "num_shapes = " << num_shapes << "\n";
  std::cerr << "num_particles = " << num_particles << "\n";

  thetas.resize(num_particles);
  phis.resize(num_particles);
  result.resize(num_particles);

  for (size_t j = 0; j < num_particles; j++) {
    thetas[j].resize(num_shapes);
    phis[j].resize(num_shapes);
  }

  for (int i = 0; i < num_shapes; i++) {
    for (size_t j = 0; j < num_particles; j++) {
      double curNormal[3];
      double curNormalSph[3];

      double position[3];
      position[0] = particles(j * 3 + 0, i);
      position[1] = particles(j * 3 + 1, i);
      position[2] = particles(j * 3 + 2, i);

      auto normal = meshes[i]->SampleNormalAtPoint(position);

      curNormal[0] = normal[0];
      curNormal[1] = normal[1];
      curNormal[2] = normal[2];

      Utils::cartesian2spherical(curNormal, curNormalSph);
      phis[j][i] = curNormalSph[1];
      thetas[j][i] = curNormalSph[2];
    }
  }

  // compute mean normal for every particle
  vnl_matrix<double> average_normals(num_particles, 3);
  for (size_t j = 0; j < num_particles; j++) {
    double avgNormal_sph[3];
    double avgNormal_cart[3];
    avgNormal_sph[0] = 1;
    avgNormal_sph[1] = Utils::averageThetaArc(phis[j]);
    avgNormal_sph[2] = Utils::averageThetaArc(thetas[j]);
    Utils::spherical2cartesian(avgNormal_sph, avgNormal_cart);

    average_normals(j, 0) = avgNormal_cart[0];
    average_normals(j, 1) = avgNormal_cart[1];
    average_normals(j, 2) = avgNormal_cart[2];
  }

  for (size_t j = 0; j < num_particles; j++) {
    double cur_cos_appex = 0;
    // the mean normal of the current particle index
    double nx_jj = average_normals(j, 0);
    double ny_jj = average_normals(j, 1);
    double nz_jj = average_normals(j, 2);

    for (int shape = 0; shape < num_shapes; shape++) {
      double position[3];
      position[0] = particles(j * 3 + 0, shape);
      position[1] = particles(j * 3 + 1, shape);
      position[2] = particles(j * 3 + 2, shape);

      auto normal = meshes[shape]->SampleNormalAtPoint(position);

      double nx_kk = normal[0];
      double ny_kk = normal[1];
      double nz_kk = normal[2];

      cur_cos_appex += (nx_jj * nx_kk + ny_jj * ny_kk + nz_jj * nz_kk);
    }

    cur_cos_appex /= num_shapes;
    // AKM: double this appears to put many/most particles well about 1.0, which becomes impossible to mark as bad no
    // matter what the angle.  I'm commentting this out for now.
    // cur_cos_appex *= 2.0;  // due to symmetry about the mean normal

    std::cerr << "cur_cos_appex = " << cur_cos_appex << "\n";
    std::cerr << "max_angle_degrees = " << max_angle_degrees << "\n";
    std::cerr << "check = " << (std::cos(max_angle_degrees * M_PI / 180.)) << "\n";
    result[j] = cur_cos_appex > std::cos(max_angle_degrees * M_PI / 180.);
  }

  return result;
}

}  // namespace shapeworks
