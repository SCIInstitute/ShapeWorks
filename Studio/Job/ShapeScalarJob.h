#pragma once
#include <Data/Session.h>
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

namespace shapeworks {

class Project;

class ShapeScalarJob : public Job {
  Q_OBJECT
 public:
  ShapeScalarJob(QSharedPointer<Session> session, QString target_feature);
  void run() override;
  QString name() override;

 private:
  QSharedPointer<Session> session_;

  ParticleShapeStatistics stats_;

  QString target_feature_;

  Eigen::VectorXd p1_hat_;
};
}  // namespace shapeworks
