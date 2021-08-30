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
void Worker::run_job()
{
  QThread* thread = new QThread;
  this->moveToThread(thread);
  connect(thread, &QThread::started, this, &Worker::process);


  connect(this, &Worker::finished, thread, &QThread::quit);
  connect(this, &Worker::finished, this, &Worker::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);

  thread->start();
}
}
