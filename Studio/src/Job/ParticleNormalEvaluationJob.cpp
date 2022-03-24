#include "ParticleNormalEvaluationJob.h"

#include <Data/Session.h>

#include "ParticleNormalEvaluation.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleNormalEvaluationJob::ParticleNormalEvaluationJob(QSharedPointer<Session> session, double max_angle_degrees) {
  session_ = session;
  max_angle_degrees_ = max_angle_degrees;
}

//---------------------------------------------------------------------------
void ParticleNormalEvaluationJob::run() {
  std::vector<bool> good_bad;

  int num_domains = session_->get_domains_per_shape();

  float total = (num_domains * session_->get_shapes().size()) + 1;
  float count = 0;

  for (int domain = 0; domain < num_domains; domain++) {
    ParticleSystem particles = session_->get_local_particle_system(domain);

    std::vector<std::shared_ptr<VtkMeshWrapper>> meshes;
    for (auto& shape : session_->get_shapes()) {
      meshes.push_back(shape->get_mesh_wrappers()[domain]);
      count++;
      emit progress(count / total);
    }

    auto domain_good_bad = ParticleNormalEvaluation::evaluate_particle_normals(particles, meshes, max_angle_degrees_);
    good_bad.insert(good_bad.end(), domain_good_bad.begin(), domain_good_bad.end());
  }

  std::cerr << "good_bad.size() = " << good_bad.size() << "\n";
  int count2 = 0;
  for (int i = 0; i < good_bad.size(); i++) {
    if (good_bad[i]) {
      count2++;
    }
  }
  std::cerr << "good count = " << count2 << "\n";

  emit progress(1.0);

  good_bad_ = good_bad;
}

//---------------------------------------------------------------------------
QString ParticleNormalEvaluationJob::name() { return "Particle Normal Evaluation"; }

}  // namespace shapeworks
