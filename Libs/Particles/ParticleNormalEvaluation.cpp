#include "ParticleNormalEvaluation.h"

#include <Logging.h>
#include <Utils.h>

#include "Libs/Optimize/Domain/MeshWrapper.h"

namespace shapeworks {

//---------------------------------------------------------------------------
std::vector<double> ParticleNormalEvaluation::evaluate_particle_normals(const Eigen::MatrixXd &particles,
                                                                        const Eigen::MatrixXd &normals) {
  std::vector<double> result;

  std::vector<std::vector<double>> thetas;
  std::vector<std::vector<double>> phis;

  int num_shapes = particles.cols();
  int num_particles = particles.rows() / 3;

  thetas.resize(num_particles);
  phis.resize(num_particles);
  result.resize(num_particles);

  for (size_t j = 0; j < num_particles; j++) {
    thetas[j].resize(num_shapes);
    phis[j].resize(num_shapes);
  }

  for (int i = 0; i < num_shapes; i++) {
    for (size_t j = 0; j < num_particles; j++) {
      double cur_normal[3];

      double position[3];
      position[0] = particles(j * 3 + 0, i);
      position[1] = particles(j * 3 + 1, i);
      position[2] = particles(j * 3 + 2, i);

      cur_normal[0] = normals(j * 3 + 0, i);
      cur_normal[1] = normals(j * 3 + 1, i);
      cur_normal[2] = normals(j * 3 + 2, i);

      double cur_normal_spherical[3];
      Utils::cartesian2spherical(cur_normal, cur_normal_spherical);
      phis[j][i] = cur_normal_spherical[1];
      thetas[j][i] = cur_normal_spherical[2];
    }
  }

  // compute mean normal for every particle
  vnl_matrix<double> average_normals(num_particles, 3);
  for (size_t j = 0; j < num_particles; j++) {
    double avg_normal_spherical[3];
    double avg_normal_cart[3];
    avg_normal_spherical[0] = 1;
    avg_normal_spherical[1] = Utils::averageThetaArc(phis[j]);
    avg_normal_spherical[2] = Utils::averageThetaArc(thetas[j]);
    Utils::spherical2cartesian(avg_normal_spherical, avg_normal_cart);

    average_normals(j, 0) = avg_normal_cart[0];
    average_normals(j, 1) = avg_normal_cart[1];
    average_normals(j, 2) = avg_normal_cart[2];
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

      double nx_kk = normals(j * 3 + 0, shape);
      double ny_kk = normals(j * 3 + 1, shape);
      double nz_kk = normals(j * 3 + 2, shape);

      cur_cos_appex += (nx_jj * nx_kk + ny_jj * ny_kk + nz_jj * nz_kk);
    }

    cur_cos_appex /= num_shapes;
    // AKM: double this appears to put many/most particles well about 1.0, which becomes impossible to mark as bad no
    // matter what the angle.  I'm commenting this out for now.
    // cur_cos_appex *= 2.0;  // due to symmetry about the mean normal

    // arc cosine
    auto cur_angle = std::acos(cur_cos_appex);

    // convert to degrees
    cur_angle *= 180.0 / M_PI;

    result[j] = cur_angle;
  }

  return result;
}

//---------------------------------------------------------------------------
std::vector<bool> ParticleNormalEvaluation::threshold_particle_normals(std::vector<double> angles,
                                                                       double max_angle_degrees) {
  std::vector<bool> result(angles.size());
  auto num_particles = angles.size();
  for (size_t i = 0; i < num_particles; i++) {
    result[i] = angles[i] < max_angle_degrees;
  }
  return result;
}

//---------------------------------------------------------------------------
Eigen::MatrixXd ParticleNormalEvaluation::compute_particle_normals(
    const Eigen::MatrixXd &particles, std::vector<std::shared_ptr<MeshWrapper>> meshes) {
  Eigen::MatrixXd normals;
  normals.resize(particles.rows(), particles.cols());

  int num_shapes = particles.cols();
  int num_particles = particles.rows() / 3;

  if (num_shapes != meshes.size()) {
    throw std::runtime_error("Number of shapes do not match");
  }
  for (size_t j = 0; j < num_particles; j++) {
    for (int shape = 0; shape < num_shapes; shape++) {
      double position[3];
      position[0] = particles(j * 3 + 0, shape);
      position[1] = particles(j * 3 + 1, shape);
      position[2] = particles(j * 3 + 2, shape);

      auto normal = meshes[shape]->SampleNormalAtPoint(position);
      normals(j * 3 + 0, shape) = normal[0];
      normals(j * 3 + 1, shape) = normal[1];
      normals(j * 3 + 2, shape) = normal[2];
    }
  }
  return normals;
}
//---------------------------------------------------------------------------

}  // namespace shapeworks
