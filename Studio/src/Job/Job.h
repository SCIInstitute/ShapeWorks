#pragma once
#include <QObject>
#include <QElapsedTimer>

namespace shapeworks {

class Job : public QObject {
  Q_OBJECT

public:

  Job();
  virtual ~Job();

  virtual void run() = 0;

  virtual QString name() = 0;

  virtual QString get_completion_message();

  void start_timer();
  qint64 timer_elapsed();

public Q_SLOTS:

Q_SIGNALS:
  void error_message(QString);
  void warning_message(QString);
  void progress(double);
  void message(QString);
  void finished();

private:
  QElapsedTimer timer_;
};
}
