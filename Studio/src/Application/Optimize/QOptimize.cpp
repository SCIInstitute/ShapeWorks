#include "QOptimize.h"

QOptimize::QOptimize(QObject* parent) :
  QObject(parent),
  ShapeWorksOptimize() {}

//---------------------------------------------------------------------------
void QOptimize::iterateCallback(itk::Object* caller, const itk::EventObject &e)
{
  //itk::PSMEntropyModelFilter<ImageType>* o =
  //reinterpret_cast<itk::PSMEntropyModelFilter<ImageType>*>(caller);

  // run superclass iterateCallback
  ShapeWorksOptimize::iterateCallback(caller, e);

  auto transform = this->m_Sampler->GetParticleSystem()->GetTransform();

  if (transform(0, 0) != transform(0, 0)) {
    //throw on NaN
    throw std::runtime_error("Optimize failed! Please try changing parameters.");
  }

  if (this->m_optimization_iterations_completed % this->reportInterval_ == 0) {
    this->iterCount_ += this->reportInterval_;
    emit progress(this->iterCount_ * 100 / this->totalIters_);
  }
}
