#include <iostream>

#include "ShapeEvaluationWorker.h"
namespace shapeworks {

//-----------------------------------------------------------------------------
ShapeEvaluationWorker::ShapeEvaluationWorker(JobType job_type, ParticleShapeStatistics stats)
  : job_type_(job_type), stats_(stats)
{
  qRegisterMetaType<shapeworks::ShapeEvaluationWorker::JobType>(
    "shapeworks::ShapeEvaluationWorker::JobType");
  qRegisterMetaType<Eigen::VectorXd>("Eigen::VectorXd");
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::run()
{
  auto callback = std::bind(&ShapeEvaluationWorker::receive_progress, this, std::placeholders::_1);
  switch (this->job_type_) {
  case JobType::CompactnessType:
    std::cerr << "Running compactness\n";
    emit result_ready(this->job_type_, this->stats_.get_compactness(callback));
    break;
  case JobType::GeneralizationType:
    std::cerr << "Running generalization\n";
    emit result_ready(this->job_type_, this->stats_.get_generalization(callback));
    break;
  case JobType::SpecificityType:
    std::cerr << "Running specificity\n";
    emit result_ready(this->job_type_, this->stats_.get_specificity(callback));
    break;
  }

  emit finished();
}

//-----------------------------------------------------------------------------
QString ShapeEvaluationWorker::name()
{
  return "Shape Evaluation";
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::receive_progress(float progress)
{
  emit report_progress(this->job_type_, progress);
}
}
