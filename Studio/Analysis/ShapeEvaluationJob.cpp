#include "ShapeEvaluationJob.h"

#include <Logging.h>

namespace shapeworks {

//-----------------------------------------------------------------------------
ShapeEvaluationJob::ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats)
    : job_type_(job_type), stats_(stats) {
  qRegisterMetaType<shapeworks::ShapeEvaluationJob::JobType>("shapeworks::ShapeEvaluationWorker::JobType");
  qRegisterMetaType<Eigen::VectorXd>("Eigen::VectorXd");
}

//-----------------------------------------------------------------------------
void ShapeEvaluationJob::run() {
  auto callback = std::bind(&ShapeEvaluationJob::receive_progress, this, std::placeholders::_1);
  switch (job_type_) {
    case JobType::CompactnessType:
      Q_EMIT result_ready(job_type_, stats_.get_compactness(callback));
      break;
    case JobType::GeneralizationType:
      Q_EMIT result_ready(job_type_, stats_.get_generalization(callback));
      break;
    case JobType::SpecificityType:
      Q_EMIT result_ready(job_type_, stats_.get_specificity(callback));
      break;
  }
}

//-----------------------------------------------------------------------------
QString ShapeEvaluationJob::name() { return "Shape Evaluation"; }

//-----------------------------------------------------------------------------
void ShapeEvaluationJob::receive_progress(float progress) { Q_EMIT report_progress(job_type_, progress); }

//-----------------------------------------------------------------------------

}  // namespace shapeworks
