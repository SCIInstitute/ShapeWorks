#pragma once

#include <Data/Worker.h>

// ShapeWorks
#include <ParticleShapeStatistics.h>


namespace shapeworks {


class ShapeEvaluationWorker : public Worker
{
  Q_OBJECT
public:

  enum class JobType {
    CompactnessType, SpecificityType, GeneralizationType
  };


  ShapeEvaluationWorker();

  static ShapeEvaluationWorker* create_worker(ParticleShapeStatistics stats, JobType job_type);

  void async_run_generalization();

  void async_evaluate_shape();

  void process() override;

signals:

  void result_ready(shapeworks::ShapeEvaluationWorker::JobType job_type, Eigen::VectorXd data);

private:

  ParticleShapeStatistics stats_;
  JobType job_type_;
};
}

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationWorker::JobType);
