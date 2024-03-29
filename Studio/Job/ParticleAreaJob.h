#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

#include <QSharedPointer>

class vtkLookupTable;

namespace shapeworks {

class Session;

class ParticleAreaJob : public Job {
  Q_OBJECT
 public:
  ParticleAreaJob(QSharedPointer<Session> session);
  void run() override;
  QString name() override { return "Particle Normal Evaluation"; }

  const Eigen::VectorXd& get_mean_areas() const { return mean_areas_; }
  const Eigen::VectorXd& get_stddev_areas() const { return stddev_areas_; }

 Q_SIGNALS:

 private:
  QSharedPointer<Session> session_;
  Eigen::VectorXd mean_areas_;
  Eigen::VectorXd stddev_areas_;
};
}  // namespace shapeworks
