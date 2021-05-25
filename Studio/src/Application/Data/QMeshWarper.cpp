#include "QMeshWarper.h"

namespace shapeworks {

//---------------------------------------------------------------------------
QMeshWarper::QMeshWarper(QObject* parent) : QObject(parent)
{}

//---------------------------------------------------------------------------
void QMeshWarper::update_progress(float p)
{
  this->current_progress_ = p;
  emit progress();
}

//---------------------------------------------------------------------------
float QMeshWarper::get_progress()
{
  return this->current_progress_;
}

}