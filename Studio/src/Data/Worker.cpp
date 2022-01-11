#include <QThread>

#include <Data/Worker.h>

namespace shapeworks {

//---------------------------------------------------------------------------
Worker::Worker()
{}

//---------------------------------------------------------------------------
Worker::~Worker()
{}

//---------------------------------------------------------------------------
Worker* Worker::create_worker()
{
  Worker* worker = new Worker{};
  return worker;
}

//---------------------------------------------------------------------------
void Worker::run_job(QSharedPointer<Job> job)
{
  this->job_ = job;
  QThread* thread = new QThread;
  this->moveToThread(thread);
  this->job_->moveToThread(thread);

  connect(thread, &QThread::started, this, &Worker::process);
  connect(this, &Worker::finished, thread, &QThread::quit);
  connect(this, &Worker::finished, this, &Worker::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  thread->start();
}

//---------------------------------------------------------------------------
void Worker::process()
{
  try {
    this->job_->run();
  } catch (std::exception& e) {
    emit this->job_->error_message(e.what());
  }
  emit this->job_->finished();
}
} // shapeworks
