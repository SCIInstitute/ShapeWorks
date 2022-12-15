
#include "ParticleGoodBadAssessment.h"

#include <vector>

namespace shapeworks {

std::vector<std::vector<int>> ParticleGoodBadAssessment::run_assessment(const itk::ParticleSystem* ps,
                                                                        MeanCurvatureCacheType* mean_curvature_cache) {
  const int total_domains = ps->GetNumberOfDomains();
  const int num_shapes = total_domains / domains_per_shape_;
  std::vector<std::vector<int>> bad_ids;
  bad_ids.resize(domains_per_shape_);
  for (int i = 0; i < domains_per_shape_; i++) {
    std::vector<vnl_matrix<double>> normals;
    normals.resize(num_shapes);
    for (int j = 0; j < num_shapes; j++) {
      int d = j * domains_per_shape_ + i;
      normals[j] = compute_particles_normals(d, ps);
    }

    for (int n = 0; n < ps->GetNumberOfParticles(i); n++) {
      bool flag = true;
      for (int a = 0; a < num_shapes; a++) {
        if (!flag) {
          break;
        }
        for (int b = a + 1; b < num_shapes; b++) {
          double dot_product = normals[a][n][0] * normals[b][n][0] + normals[a][n][1] * normals[b][n][1] +
                               normals[a][n][2] * normals[b][n][2];

          int dom_a = a * domains_per_shape_ + i;
          int dom_b = b * domains_per_shape_ + i;
          double curv_a_n = mean_curvature_cache->operator[](dom_a)->operator[](n);
          double curv_b_n = mean_curvature_cache->operator[](dom_b)->operator[](n);
          double curv_a = mean_curvature_cache->GetMeanCurvature(dom_a);
          double curv_b = mean_curvature_cache->GetMeanCurvature(dom_b);

          double val = criterion_angle_ * 0.5 * (curv_a_n / curv_a + curv_b_n / curv_b);

          if (dot_product < std::cos(val)) {
            bad_ids[i].push_back(n);
            flag = false;
            break;
          }
        }
      }
    }
  }

  return bad_ids;
}

vnl_matrix<double> ParticleGoodBadAssessment::compute_particles_normals(int d, const itk::ParticleSystem* ps) {
  using PointType = itk::ParticleSystem::PointType;
  using NormalType = vnl_vector_fixed<float, 3>;

  int num = ps->GetNumberOfParticles(d);
  vnl_matrix<double> normals(num, 3);

  for (int i = 0; i < num; i++) {
    PointType p = ps->GetPosition(i, d);
    NormalType n = ps->GetDomain(d)->SampleNormalAtPoint(p, -1);
    for (int j = 0; j < 3; j++) {
      normals[i][j] = n[j];
    }
  }
  return normals;
}
}  // namespace shapeworks
