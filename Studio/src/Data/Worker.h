#pragma once

#include <QObject>

namespace shapeworks {


class Worker : public QObject {
  Q_OBJECT

public:
  Worker();
  ~Worker();

 void run_job();

public Q_SLOTS:
  virtual void process() = 0;

Q_SIGNALS:
  void error_message(QString);
  void warning_message(QString);
  void status_message(QString);
  void progress(int);
  void finished();

private:
};
}
