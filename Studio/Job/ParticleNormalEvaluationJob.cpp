
#include "ParticleNormalEvaluationJob.h"

#include <Data/Session.h>
#include <Logging.h>

#include "ParticleNormalEvaluation.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleNormalEvaluationJob::ParticleNormalEvaluationJob(QSharedPointer<Session> session, double max_angle_degrees) {
  session_ = session;
  max_angle_degrees_ = max_angle_degrees;
  qRegisterMetaType<std::vector<bool>>("std::vector<bool>");
}

//---------------------------------------------------------------------------
void ParticleNormalEvaluationJob::run() {
  std::vector<bool> good_bad;

  int num_domains = session_->get_domains_per_shape();

  float total = (num_domains * session_->get_shapes().size()) + 1;
  float count = 0;

  for (int domain = 0; domain < num_domains; domain++) {
    ParticleSystemEvaluation particles = session_->get_local_particle_system(domain);

    std::vector<std::shared_ptr<Surface>> meshes;
    for (auto& shape : session_->get_shapes()) {
      meshes.push_back(shape->get_groomed_mesh_wrappers()[domain]);
      count++;
      Q_EMIT progress(count / total);
    }

    auto normals = ParticleNormalEvaluation::compute_particle_normals(particles.get_matrix(), meshes);
    auto angles = ParticleNormalEvaluation::evaluate_particle_normals(particles.get_matrix(), normals);
    auto domain_good_bad = ParticleNormalEvaluation::threshold_particle_normals(angles, max_angle_degrees_);

    good_bad.insert(good_bad.end(), domain_good_bad.begin(), domain_good_bad.end());
  }

  int good_count = std::count(good_bad.begin(), good_bad.end(), true);

  SW_LOG("ParticleNormalEvaluationJob: found " + std::to_string(good_count) + "/" + std::to_string(good_bad.size()) +
         " good particles");

  Q_EMIT progress(1.0);

  Q_EMIT result_ready(good_bad);
  good_bad_ = good_bad;
}

//---------------------------------------------------------------------------
QString ParticleNormalEvaluationJob::name() { return "Particle Normal Evaluation"; }

}  // namespace shapeworks
