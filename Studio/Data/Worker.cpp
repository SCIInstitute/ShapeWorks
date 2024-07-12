#include <Data/Worker.h>
#include <Logging.h>

#include <QThread>

namespace shapeworks {

//---------------------------------------------------------------------------
Worker::Worker() {}

//---------------------------------------------------------------------------
Worker::~Worker() {}

//---------------------------------------------------------------------------
Worker* Worker::create_worker() {
  Worker* worker = new Worker{};
  return worker;
}

//---------------------------------------------------------------------------
void Worker::run_job(QSharedPointer<Job> job) {
  job_ = job;
  thread_ = new QThread;
  this->moveToThread(thread_);
  job_->moveToThread(thread_);
  connect(thread_, &QThread::started, this, &Worker::process);
  connect(this, &Worker::finished, thread_, &QThread::quit);
  connect(this, &Worker::finished, this, &Worker::deleteLater);
  connect(thread_, &QThread::finished, thread_, &QThread::deleteLater);
  thread_->start();
}

//---------------------------------------------------------------------------
void Worker::stop() {
  if (thread_ != nullptr && job_ != nullptr) {
    job_->abort();
    thread_->quit();
  }
}

//---------------------------------------------------------------------------
void Worker::process() {
  try {
    job_->start_timer();
    job_->run();
    job_->set_complete(true);
  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
  }
  if (job_->is_aborted()) {
    SW_LOG(job_->get_abort_message().toStdString());
  } else {
    SW_LOG(job_->get_completion_message().toStdString());
  }
  Q_EMIT job_->progress(1.0);
  Q_EMIT job_->finished();
}
}  // namespace shapeworks
