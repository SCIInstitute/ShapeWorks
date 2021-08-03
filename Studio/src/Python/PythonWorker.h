#pragma once

#include <QObject>

#include <Data/Session.h>

namespace shapeworks {
class Optimize;
class QGroom;
class QDeepSSM;
class OptimizeParameters;
class PythonLogger;

class PythonWorker : public QObject {
Q_OBJECT

public:
  enum class JobType {
    DeepSSM_AugmentationType, DeepSSM_TrainingType, DeepSSM_TestingType
  };

  PythonWorker();
  ~PythonWorker();

  void set_deep_ssm(QSharedPointer<QDeepSSM> deep_ssm);

  void run_job(JobType job);

  void incoming_python_message(std::string message_string);
  void incoming_python_progress(double value);

  void end_python();

  void abort_job();

public Q_SLOTS:

  void init();

  void start_deepssm_augmentation();
  void start_deepssm_training();
  void finalize_python();

Q_SIGNALS:

  void deepssm_augmentation_complete();
  void deepssm_training_complete();
  void job_finished();

  void result_ready();
  void error_message(QString);
  void warning_message(QString);
  void progress(double);
  void message(QString);
  void finished();

private:

  void finish_job();

  QSharedPointer<QDeepSSM> deep_ssm_;
  QSharedPointer<PythonLogger> python_logger_;

  QThread* thread_;
};
}
