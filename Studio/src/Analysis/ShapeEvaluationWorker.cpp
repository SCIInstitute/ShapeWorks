#include <iostream>

#include "ShapeEvaluationWorker.h"
namespace shapeworks {

//-----------------------------------------------------------------------------
ShapeEvaluationWorker::ShapeEvaluationWorker()
{
  qRegisterMetaType<shapeworks::ShapeEvaluationWorker::JobType>("shapeworks::ShapeEvaluationWorker::JobType");
  qRegisterMetaType<Eigen::VectorXd>("Eigen::VectorXd");

}

//-----------------------------------------------------------------------------
ShapeEvaluationWorker* ShapeEvaluationWorker::create_worker(ParticleShapeStatistics stats, JobType job_type)
{
  ShapeEvaluationWorker* shape_evaluation_worker = new ShapeEvaluationWorker();
  shape_evaluation_worker->stats_ = std::move(stats);
  shape_evaluation_worker->job_type_ = job_type;
  return shape_evaluation_worker;
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::async_run_generalization()
{

}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::async_evaluate_shape()
{
  this->run_job();
}

//-----------------------------------------------------------------------------
void ShapeEvaluationWorker::process()
{

  switch (this->job_type_) {
    case JobType::CompactnessType:
      std::cerr << "Running compactness\n";
      emit result_ready(this->job_type_,this->stats_.get_compactness());
      break;
    case JobType::GeneralizationType:
      std::cerr << "Running generalization\n";
      emit result_ready(this->job_type_,this->stats_.get_generalization());
      break;
    case JobType::SpecificityType:
      std::cerr << "Running specificity\n";
      emit result_ready(this->job_type_,this->stats_.get_specificity());
      break;
  }

  emit finished();
}
}
