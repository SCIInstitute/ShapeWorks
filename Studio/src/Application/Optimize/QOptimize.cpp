#include "QOptimize.h"

#include <QMutexLocker>

namespace shapeworks {

QOptimize::QOptimize(QObject* parent) :
  QObject(parent),
  Optimize()
{}

//---------------------------------------------------------------------------
QOptimize::~QOptimize()
{}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetLocalPoints()
{
  QMutexLocker locker(&qmutex_);
  return this->m_local_points;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetGlobalPoints()
{
  QMutexLocker locker(&qmutex_);
  return this->m_global_points;
}

//---------------------------------------------------------------------------
void QOptimize::UpdateExportablePoints()
{
  QMutexLocker locker(&qmutex_);
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
void QOptimize::IterateCallback(itk::Object* caller, const itk::EventObject& e)
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

  int stage_num_iterations = m_sampler->GetOptimizer()->GetNumberOfIterations();
  int stage_total_iterations = m_sampler->GetOptimizer()->GetMaximumNumberOfIterations();
  int num_particles = m_sampler->GetParticleSystem()->GetNumberOfParticles(0);
  QString message;
  if (this->m_optimizing) {
    message = "Optimizing: ";
  }
  else {
    message = "Initializing: ";
  }

  message = message + "Particles: " + QString::number(num_particles) + ", Iteration: " +
            QString::number(stage_num_iterations) + " / " + QString::number(stage_total_iterations);

  if (update) {
    this->time_since_last_update_.start();
    {
      QMutexLocker locker(&qmutex_);

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
    }
    emit progress(this->m_iteration_count * 100 / this->m_total_iterations, message);
  }
}

//---------------------------------------------------------------------------
std::vector<StudioParticles> QOptimize::GetParticles()
{
  QMutexLocker locker(&qmutex_);

  std::vector<StudioParticles> particles;

  int num_domains_per_subject = this->GetDomainsPerShape();
  int num_subjects = this->GetNumShapes() / num_domains_per_subject;
  particles.resize(num_subjects);

  int subject = 0;
  int domain = 0;
  for (int i = 0; i < this->m_local_points.size(); i++) {
    particles[subject].set_local_particles(domain, this->m_local_points[i]);
    particles[subject].set_world_particles(domain, this->m_global_points[i]);
    domain++;
    if (domain == num_domains_per_subject) {
      subject++;
      domain = 0;
    }
  }

  return particles;
}
}