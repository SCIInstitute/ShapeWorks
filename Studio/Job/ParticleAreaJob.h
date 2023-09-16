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
  QString name() override;

 Q_SIGNALS:

  void result_ready(std::vector<bool> good_bad);

 private:
  QSharedPointer<Session> session_;

};
}  // namespace shapeworks
