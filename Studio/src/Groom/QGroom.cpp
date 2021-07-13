#include "QGroom.h"

namespace shapeworks {

void QGroom::update_progress()
{
  emit progress(static_cast<int>(this->progress_));
}

QGroom::QGroom(ProjectHandle project) : Groom(project)
{

}
}