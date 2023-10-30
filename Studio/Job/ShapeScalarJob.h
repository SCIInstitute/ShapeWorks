#pragma once
#include <Data/Session.h>
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

#include <QPixmap>

namespace shapeworks {

class Project;

class ShapeScalarJob : public Job {
  Q_OBJECT
 public:
  ShapeScalarJob(QSharedPointer<Session> session, QString target_feature);
  void run() override;
  QString name() override;

  QPixmap get_plot();

 private:
  QSharedPointer<Session> session_;

  ParticleShapeStatistics stats_;

  QString target_feature_;

  Eigen::VectorXd p1_hat_;
  QPixmap plot_;
};
}  // namespace shapeworks
