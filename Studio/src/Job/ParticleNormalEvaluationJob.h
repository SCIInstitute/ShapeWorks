#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>
#include <QSharedPointer>

namespace shapeworks {

class Session;

class ParticleNormalEvaluationJob : public Job {
  Q_OBJECT
 public:
  ParticleNormalEvaluationJob(QSharedPointer<Session> session, double max_angle_degrees);
  void run() override;
  QString name() override;

  std::vector<bool> get_good_bad();

signals:

  void result_ready(std::vector<bool> good_bad);

 private:
  QSharedPointer<Session> session_;
  double max_angle_degrees_;

  std::vector<bool> good_bad_;
};
}  // namespace shapeworks
