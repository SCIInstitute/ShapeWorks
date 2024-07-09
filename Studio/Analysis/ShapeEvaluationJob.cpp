#include "ShapeEvaluationJob.h"

#include <Data/Session.h>
#include <Logging.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
ShapeEvaluationJob::ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats, QSharedPointer<Session> session)
    : job_type_(job_type), stats_(stats), session_(session) {
  qRegisterMetaType<shapeworks::ShapeEvaluationJob::JobType>("shapeworks::ShapeEvaluationWorker::JobType");
  qRegisterMetaType<Eigen::VectorXd>("Eigen::VectorXd");
}

//-----------------------------------------------------------------------------
void ShapeEvaluationJob::run() {
  auto callback = std::bind(&ShapeEvaluationJob::receive_progress, this, std::placeholders::_1);
  auto check_abort = std::bind(&Job::is_aborted, this);
  prep_meshes();
  switch (job_type_) {
    case JobType::CompactnessType:
      Q_EMIT result_ready(job_type_, stats_.get_compactness(callback));
      break;
    case JobType::GeneralizationType:
      Q_EMIT result_ready(job_type_, stats_.get_generalization(callback, check_abort));
      break;
    case JobType::SpecificityType:
      Q_EMIT result_ready(job_type_, stats_.get_specificity(callback, check_abort));
      break;
  }
}

//-----------------------------------------------------------------------------
QString ShapeEvaluationJob::name() { return "Shape Evaluation"; }

//-----------------------------------------------------------------------------
void ShapeEvaluationJob::receive_progress(float progress) { Q_EMIT report_progress(job_type_, progress); }

//-----------------------------------------------------------------------------
void ShapeEvaluationJob::prep_meshes() {
  if (stats_.get_particle_to_surface_mode() &&
      (job_type_ == JobType::GeneralizationType || job_type_ == JobType::SpecificityType)) {
    std::vector<Mesh> meshes;
    for (auto& shape : session_->get_shapes()) {
      Mesh mesh = shape->get_groomed_meshes(true).get_combined_poly_data();

      auto transform = shape->get_alignment(session_->get_current_alignment());
      // Apply the transform to the mesh
      mesh.applyTransform(transform);

      meshes.emplace_back(mesh);
    }
    stats_.set_meshes(meshes);
  }
}

//-----------------------------------------------------------------------------

}  // namespace shapeworks
