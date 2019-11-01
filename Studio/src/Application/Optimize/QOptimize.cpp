#include "QOptimize.h"

QOptimize::QOptimize(QObject* parent) :
  QObject(parent),
  ShapeWorksOptimize() {}

//---------------------------------------------------------------------------
QOptimize::~QOptimize()
{}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::localPoints()
{
  QMutexLocker locker(&mutex);
  return this->localPoints_;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::globalPoints()
{
  QMutexLocker locker(&mutex);
  return this->globalPoints_;
}

//---------------------------------------------------------------------------
void QOptimize::SetIterationCommand()
{
  this->iterate_command_ = itk::MemberCommand<QOptimize>::New();
  this->iterate_command_->SetCallbackFunction(this, &QOptimize::iterateCallback);
  m_Sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), this->iterate_command_);
}

//---------------------------------------------------------------------------
void QOptimize::iterateCallback(itk::Object* caller, const itk::EventObject &e)
{
  // run superclass iterateCallback
  ShapeWorksOptimize::iterateCallback(caller, e);

  auto transform = this->m_Sampler->GetParticleSystem()->GetTransform();

  if (transform(0, 0) != transform(0, 0)) {
    //throw on NaN
    throw std::runtime_error("Optimize failed! Please try changing parameters.");
  }

  bool update = false;
  if (!this->time_since_last_update_.isValid()) {
    update = true;
  }
  else {
    auto time_since = this->time_since_last_update_.elapsed();
    if (time_since > 100) {
      update = true;
    }
  }

  this->reportInterval_ = 100;

//  if (this->iterCount_ % this->reportInterval_ == 0) {
  if (update) {
    this->time_since_last_update_.start();

    QMutexLocker locker(&mutex);

    this->localPoints_.clear();
    this->globalPoints_.clear();

    emit progress(this->iterCount_ * 100 / this->m_total_iterations);
  }
  this->iterCount_++;
}
