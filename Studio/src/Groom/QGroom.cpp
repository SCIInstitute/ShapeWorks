#include "QGroom.h"

namespace shapeworks {

//---------------------------------------------------------------------------
void QGroom::update_progress()
{
  Q_EMIT progress(static_cast<int>(this->progress_));
}

//---------------------------------------------------------------------------
QGroom::QGroom(ProjectHandle project) : Groom(project)
{

}
}