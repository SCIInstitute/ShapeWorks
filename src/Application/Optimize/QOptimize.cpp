#include "QOptimize.h"

QOptimize::QOptimize(QObject * parent,
  std::vector<ImageType::Pointer> inputs,
  size_t numScales, std::vector<double> start_reg,
  std::vector<double> end_reg, std::vector<unsigned int> iters,
  std::vector<double> decay_span, double weight, bool verbose)
  : QObject(parent), ShapeWorksOptimize(inputs, numScales,
    start_reg, end_reg, iters, decay_span, weight, verbose) {}

void QOptimize::iterateCallback(itk::Object * caller, const itk::EventObject &e) {
  itk::PSMEntropyModelFilter<ImageType> *o =
    reinterpret_cast<itk::PSMEntropyModelFilter<ImageType> *>(caller);
  ShapeWorksOptimize::iterateCallback(caller, e);
  if (o->GetNumberOfElapsedIterations() % this->reportInterval_ == 0) {
    this->iterCount_ += this->reportInterval_;
    emit progress(this->iterCount_ * 100 / this->totalIters_);
  }
}
