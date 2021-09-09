#pragma once

#include <QObject>
#include <QSharedPointer>

#include <Job/Job.h>

namespace shapeworks {
class PythonLogger;

class PythonWorker : public QObject {
  Q_OBJECT

public:

  constexpr static const char* python_api_version = "6.2";

  PythonWorker();
  ~PythonWorker();

  void run_job(QSharedPointer<Job> job);

  void incoming_python_message(std::string message_string);
  void incoming_python_progress(double value);

  void end_python();

  void abort_job();

public Q_SLOTS:

  bool init();

  void start_job(QSharedPointer<Job> job);

  void finalize_python();

Q_SIGNALS:

  void result_ready();
  void error_message(QString);
  void warning_message(QString);
  void message(QString);
  void finished();

private:

  bool initialized_ = false;
  bool initialized_success_ = false;

  QSharedPointer<PythonLogger> python_logger_;

  QSharedPointer<Job> current_job_;

  QThread* thread_;
};
}
