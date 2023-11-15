#pragma once

#include <Job/Job.h>

#include <QObject>
#include <QSharedPointer>

namespace shapeworks {

class Worker : public QObject {
  Q_OBJECT

 public:
  Worker();
  ~Worker();

  static Worker *create_worker();

  void run_job(QSharedPointer<Job> job);

 public Q_SLOTS:
  void process();

 Q_SIGNALS:
  void error_message(QString);
  void warning_message(QString);
  void status_message(QString);
  void progress(int);
  void finished();

 private:
  QSharedPointer<Job> job_;
};
}  // namespace shapeworks
