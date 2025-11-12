#include "QOptimize.h"

#include <Logging.h>

#include <QMutexLocker>

namespace shapeworks {

QOptimize::QOptimize(QObject* parent) : QObject(parent), Optimize() {}

//---------------------------------------------------------------------------
QOptimize::~QOptimize() {}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetLocalPoints() {
  QMutexLocker locker(&qmutex_);
  return m_local_points;
}

//---------------------------------------------------------------------------
std::vector<std::vector<itk::Point<double>>> QOptimize::GetGlobalPoints() {
  QMutexLocker locker(&qmutex_);
  return m_global_points;
}

//---------------------------------------------------------------------------
void QOptimize::UpdateExportablePoints() {
  QMutexLocker locker(&qmutex_);
  Optimize::UpdateExportablePoints();
}

//---------------------------------------------------------------------------
void QOptimize::SetIterationCallback() {
  // First call parent class to set up the base iteration callback
  Optimize::SetIterationCallback();

  // Now extend it to add our Qt-specific behavior
  auto parent_callback = m_sampler->GetOptimizer()->get_iteration_callback();
  m_sampler->GetOptimizer()->set_iteration_callback([this, parent_callback]() {
    // Call the parent callback first (if it exists)
    if (parent_callback) {
      parent_callback();
    }

    // Then call our Qt-specific callback
    this->IterateCallbackInternal();
  });
}

//---------------------------------------------------------------------------
void QOptimize::IterateCallbackInternal() {
  if (m_aborted) {
    return;
  }

  auto transform = m_sampler->GetParticleSystem()->GetTransform();

  if (transform(0, 0) != transform(0, 0)) {
    // throw on NaN
    throw std::runtime_error("Optimize failed! Please try changing parameters.");
  }

  bool update = false;
  if (!time_since_last_update_.isValid()) {
    update = true;
  } else {
    auto time_since = time_since_last_update_.elapsed();
    if (time_since > 100) {
      update = true;
    }
  }

  if (update) {
    time_since_last_update_.start();
    {
      QMutexLocker locker(&qmutex_);

      m_local_points.clear();
      m_global_points.clear();

      // copy particles
      for (size_t d = 0; d < m_sampler->GetParticleSystem()->GetNumberOfDomains(); d++) {
        // blank set of points
        m_local_points.push_back(std::vector<itk::Point<double>>());
        m_global_points.push_back(std::vector<itk::Point<double>>());

        // for each particle
        for (size_t j = 0; j < m_sampler->GetParticleSystem()->GetNumberOfParticles(d); j++) {
          auto pos = m_sampler->GetParticleSystem()->GetPosition(j, d);
          auto pos2 = m_sampler->GetParticleSystem()->GetTransformedPosition(j, d);
          m_local_points[d].push_back(pos);
          m_global_points[d].push_back(pos2);
        }
      }
    }
    Q_EMIT progress(0, "");
  }
}

//---------------------------------------------------------------------------
std::vector<Particles> QOptimize::GetParticles() {
  QMutexLocker locker(&qmutex_);

  std::vector<Particles> particles;

  int num_domains_per_subject = GetDomainsPerShape();
  int num_subjects = GetNumShapes() / num_domains_per_subject;
  particles.resize(num_subjects);

  int subject = 0;
  int domain = 0;
  for (int i = 0; i < m_local_points.size(); i++) {
    particles[subject].set_local_particles(domain, m_local_points[i]);
    particles[subject].set_world_particles(domain, m_global_points[i]);
    domain++;
    if (domain == num_domains_per_subject) {
      subject++;
      domain = 0;
    }
  }

  return particles;
}

//---------------------------------------------------------------------------
std::vector<std::vector<std::vector<double>>> QOptimize::GetProcrustesTransforms() {
  QMutexLocker locker(&qmutex_);
  auto transforms = Optimize::GetProcrustesTransforms();
  return transforms;
}

}  // namespace shapeworks
