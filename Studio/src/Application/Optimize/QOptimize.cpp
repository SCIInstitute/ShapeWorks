#include "QOptimize.h"

#include <QMutexLocker>

QOptimize::QOptimize(QObject* parent) :
  QObject(parent),
  Optimize() {}

//---------------------------------------------------------------------------
QOptimize::~QOptimize()
{}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetLocalPoints()
{
  QMutexLocker locker(&qmutex);
  return this->m_local_points;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetGlobalPoints()
{
  QMutexLocker locker(&qmutex);
  return this->m_global_points;
}

//---------------------------------------------------------------------------
void QOptimize::UpdateExportablePoints()
{
  QMutexLocker locker(&qmutex);
  Optimize::UpdateExportablePoints();
}

//---------------------------------------------------------------------------
void QOptimize::SetIterationCallback()
{
  this->iterate_command_ = itk::MemberCommand<QOptimize>::New();
  this->iterate_command_->SetCallbackFunction(this, &QOptimize::IterateCallback);
  m_sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), this->iterate_command_);
}

//---------------------------------------------------------------------------
void QOptimize::IterateCallback(itk::Object* caller, const itk::EventObject &e)
{

  if (this->m_aborted) {
    return;
  }

  // run superclass iterateCallback
  Optimize::IterateCallback(caller, e);

  auto transform = this->m_sampler->GetParticleSystem()->GetTransform();

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

//  update = true;

//  if (this->iterCount_ % this->reportInterval_ == 0) {
  if (update) {

    this->time_since_last_update_.start();

    QMutexLocker locker(&qmutex);

    this->m_local_points.clear();
    this->m_global_points.clear();

    // copy particles
    for (size_t d = 0; d < this->m_sampler->
         GetParticleSystem()->GetNumberOfDomains(); d++) {

      // blank set of points
      this->m_local_points.push_back(std::vector<itk::Point<double>>());
      this->m_global_points.push_back(std::vector<itk::Point<double>>());

      // for each particle
      for (size_t j = 0; j < this->m_sampler->
           GetParticleSystem()->GetNumberOfParticles(d); j++) {
        auto pos = this->m_sampler->GetParticleSystem()->GetPosition(j, d);
        auto pos2 = this->m_sampler->GetParticleSystem()->GetTransformedPosition(j, d);
        this->m_local_points[d].push_back(pos);
        this->m_global_points[d].push_back(pos2);
      }
    }

    std::cerr << "iteration: " << m_iteration_count << " of " << this->m_total_iterations << "\n";
    emit progress(this->m_iteration_count * 100 / this->m_total_iterations);
  }
  this->m_iteration_count++;
}
