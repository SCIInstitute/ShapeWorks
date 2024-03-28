#pragma once

#include <Data/Worker.h>
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

namespace shapeworks {

/**
 * @brief The ShapeEvaluationJob class is a worker class that computes shape evaluation metrics of compactness,
 * specificity, and generalization.  It runs asynchronously using the Job and Worker interfaces.
 */
class ShapeEvaluationJob : public Job {
  Q_OBJECT
 public:
  enum class JobType { CompactnessType, SpecificityType, GeneralizationType };

  ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats);

  void run() override;

  QString name() override;

 Q_SIGNALS:

  void report_progress(shapeworks::ShapeEvaluationJob::JobType job_type, float progress);
  void result_ready(shapeworks::ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);

 private:
  void receive_progress(float progress);

  JobType job_type_;
  ParticleShapeStatistics stats_;
};
}  // namespace shapeworks

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationJob::JobType);
