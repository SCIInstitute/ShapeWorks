#pragma once

#include <Data/Worker.h>

// ShapeWorks
#include <ParticleShapeStatistics.h>

#include <Job/Job.h>

namespace shapeworks {

class ShapeEvaluationWorker : public Job
{
  Q_OBJECT
public:

  enum class JobType {
    CompactnessType, SpecificityType, GeneralizationType
  };



  ShapeEvaluationWorker(JobType job_type, ParticleShapeStatistics stats);

   void run() override;

  QString name() override;


signals:

  void report_progress(shapeworks::ShapeEvaluationWorker::JobType job_type, float progress);
  void result_ready(shapeworks::ShapeEvaluationWorker::JobType job_type, Eigen::VectorXd data);

private:

  void receive_progress(float progress);

  ParticleShapeStatistics stats_;
  JobType job_type_;
};
}

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationWorker::JobType);
