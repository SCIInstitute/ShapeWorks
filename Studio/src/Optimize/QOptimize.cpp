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
    this->check_for_swap();
    emit progress(this->m_iteration_count * 100 / this->m_total_iterations, message);
  }
}

//---------------------------------------------------------------------------
void QOptimize::check_for_swap()
{
  std::cerr << "------------------------\n";
  //std::cerr << "check for swap\n";
  std::vector<itk::Point<double>> mean;
  if (m_global_points.size() < 1) {
    return;
  }
  int num_particles = m_global_points[0].size();
  int num_shapes = m_global_points.size();
  for (int i = 0; i < num_particles; i++) {
    itk::Point<double> point;
    point[0] = 0;
    point[1] = 0;
    point[2] = 0;

    for (int d = 0; d < num_shapes; d++) {
      auto shape = m_global_points[d];
      point[0] += shape[i][0];
      point[1] += shape[i][1];
      point[2] += shape[i][2];
    }
    point[0] /= num_shapes;
    point[1] /= num_shapes;
    point[2] /= num_shapes;
    mean.push_back(point);
  }

  double mean_nearest = 0;
  for (int i = 0; i < num_particles; i++) {
    double nearest = std::numeric_limits<double>::max();
    for (int j = 0; j < num_particles; j++) {
      if (i == j) {continue;}
      double dist = mean[i].EuclideanDistanceTo(mean[j]);
      //std::cerr << "dist = " << dist << "\n";
      nearest = std::min<double>(nearest, dist);
    }
    mean_nearest += nearest;
  }
  mean_nearest /= num_particles;
  std::cerr << "mean_nearest = " << mean_nearest << "\n";

  double radius = mean_nearest * 1.5;
  for (int i = 0; i < num_particles; i++) {
    std::vector<int> neighbors;
    for (int j = 0; j < num_particles; j++) {
      if (i == j) {continue;}
      double dist = mean[i].EuclideanDistanceTo(mean[j]);
      if (dist < radius) {
        neighbors.push_back(j);
      }
    }


    // neighbors contains the neighbors of particle i


    using VectorType = itk::Vector<double, 3>;

/*
    for (int k = 0; k < neighbors.size(); k++) {

      VectorType u;
      u[0] = mean[i][0] - mean[neighbors[k]][0];
      u[1] = mean[i][1] - mean[neighbors[k]][1];
      u[2] = mean[i][2] - mean[neighbors[k]][2];

      VectorType v;
      for (int d = 0; d < num_shapes; d++) {
        v[0] = m_global_points[d][i][0] - m_global_points[d][k][0];
        v[1] = m_global_points[d][i][1] - m_global_points[d][k][1];
        v[2] = m_global_points[d][i][2] - m_global_points[d][k][2];
        double dot = u * v;
        if (i == 5) {
          std::cerr << "dot for " << i << " and " << k << " on d:" << d << " = " << dot << "\n";
        }
        if (dot < 0) {
          std::cerr << "uh oh, perhaps domain " << d << ", particle " << i << " and " << k << " is flipped?\n";
        }
      }

      std::cerr << neighbors[k] << " ";
    }
*/


    if (neighbors.size() > 2) {

      if (i == 5) {
        std::cerr << "5's neighbors on mean are: ";
        for (int k = 0; k < neighbors.size(); k++) {
          std::cerr << neighbors[k] << " ";
        }
        std::cerr << "\n";
      }

      for (int d = 0; d < num_shapes; d++) {

        std::vector<int> neighbors_local;

        for (int j = 0; j < num_particles; j++) {
          if (i == j) { continue; }
          double dist = m_global_points[d][i].EuclideanDistanceTo(m_global_points[d][j]);
          if (dist < mean_nearest * 1.25) {
            if (d == 1 && i == 5) {
              std::cerr << "1:5 neighbor = " << j << "\n";
            }
            neighbors_local.push_back(j);
            bool found = false;
            for (int k = 0; k < neighbors.size(); k++) {
              if (neighbors[k] == j) {
                found = true;
              }
            }
            if (!found) {
              std::cerr << "perhaps domain " << d << ", particle " << j << " is flipped?\n";
            }
          }
        }
      }
    }

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

//---------------------------------------------------------------------------
std::vector<std::vector<std::vector<double>>> QOptimize::GetProcrustesTransforms()
{
  QMutexLocker locker(&qmutex_);

  std::vector<std::vector<std::vector<double>>> transforms;

  int num_domains_per_subject = this->GetDomainsPerShape();
  int num_subjects = this->GetNumShapes() / num_domains_per_subject;
  transforms.resize(num_subjects);

  auto ps = this->GetSampler()->GetParticleSystem();

  int subject = 0;
  int domain = 0;
  std::vector<std::vector<double>> subject_transform;
  for (int i = 0; i < this->m_local_points.size(); i++) {  // iterate over all domains

    auto procrustes = ps->GetTransform(i);
    std::vector<double> transform;
    for (int i = 0; i < procrustes.cols(); i++) {
      for (int j = 0; j < procrustes.rows(); j++) {
        transform.push_back(procrustes(i, j));
      }
    }
    subject_transform.push_back(transform);

    domain++;
    if (domain == num_domains_per_subject) {
      transforms[subject] = subject_transform;
      subject++;
      domain = 0;
      subject_transform = std::vector<std::vector<double>>();
    }
  }

  return transforms;
}
}
