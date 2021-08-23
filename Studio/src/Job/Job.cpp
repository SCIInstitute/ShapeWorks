#include <Job/Job.h>
namespace shapeworks {

//---------------------------------------------------------------------------
Job::Job()
{}

//---------------------------------------------------------------------------
Job::~Job()
{}

//---------------------------------------------------------------------------
QString Job::get_completion_message()
{
  QString duration = QString::number(this->timer_.elapsed() / 1000.0, 'f', 1);
  return this->name() + " complete.  Duration: " + duration + " seconds";
}

//---------------------------------------------------------------------------
void Job::start_timer()
{
  this->timer_.start();
}

//---------------------------------------------------------------------------
qint64 Job::timer_elapsed()
{
  return this->timer_.elapsed();
}
}
