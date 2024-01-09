#pragma once
#include <QElapsedTimer>
#include <QObject>

namespace shapeworks {

class Job : public QObject {
  Q_OBJECT

 public:
  Job();
  virtual ~Job();

  //! run the job
  virtual void run() = 0;

  //! get the name of the job
  virtual QString name() = 0;

  //! get a message to display when the job is complete
  virtual QString get_completion_message();

  //! get a message to display when the job is aborted
  virtual QString get_abort_message();

  //! start the timer
  void start_timer();

  //! how much time has elapsed since the timer was started
  qint64 timer_elapsed();

  //! set the job as complete
  void set_complete(bool complete) { complete_ = complete; }

  //! is the job complete?
  bool is_complete() const { return complete_; }

  //! abort the job
  void abort() { abort_ = true; }

  //! was the job aborted?
  bool is_aborted() const { return abort_; }

  //! set to quiet mode (no progress messages)
  void set_quiet_mode(bool quiet) { quiet_mode_ = quiet; }

  //! get quiet mode
  bool get_quiet_mode() { return quiet_mode_; }

 public Q_SLOTS:

 Q_SIGNALS:

  void progress(double, QString = "");
  void finished();

 private:
  std::atomic<bool> complete_ = false;
  std::atomic<bool> abort_ = false;
  std::atomic<bool> quiet_mode_ = false;

  QElapsedTimer timer_;
};
}  // namespace shapeworks
