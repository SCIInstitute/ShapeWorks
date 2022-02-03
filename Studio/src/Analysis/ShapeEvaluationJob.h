#pragma once

#include <ParticleShapeStatistics.h>

#include <Data/Worker.h>
#include <Job/Job.h>

namespace shapeworks {

class ShapeEvaluationJob : public Job
{
  Q_OBJECT
public:

  enum class JobType {
    CompactnessType, SpecificityType, GeneralizationType, WithinSubspaceCompactness, BetweenSubspaceCompactness
  };

  ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats);

  void run() override;

  QString name() override;

signals:

  void report_progress(shapeworks::ShapeEvaluationJob::JobType job_type, float progress);
  void result_ready(shapeworks::ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);

private:

  void receive_progress(float progress);

  JobType job_type_;
  ParticleShapeStatistics stats_;
};
}

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationJob::JobType);
