#pragma once
#include <Job/Job.h>

#include <QSharedPointer>

namespace shapeworks {

class Session;

class SharedBoundaryJob : public Job {
  Q_OBJECT
 public:
  SharedBoundaryJob(QSharedPointer<Session> session, int first_domain, int second_domain);
  void run() override;
  QString name() override { return "Shared Boundary"; }

 Q_SIGNALS:

 private:
  int first_domain_{0};
  int second_domain_{0};
  QSharedPointer<Session> session_;
};
}  // namespace shapeworks
