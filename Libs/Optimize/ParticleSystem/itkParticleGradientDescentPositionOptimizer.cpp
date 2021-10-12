const int global_iteration = 1;

#include <algorithm>
#include "itkParticleImageDomainWithGradients.h"
#include "itkParticleGradientDescentPositionOptimizer.h"
#include <vector>
#include <sstream>
#include "MemoryUsage.h"
#include <chrono>

#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>

#include <Libs/Optimize/Global.h>


namespace itk {

//-----------------------------------------------------------------------------
ParticleGradientDescentPositionOptimizer::ParticleGradientDescentPositionOptimizer()
{
  m_StopOptimization = false;
  m_NumberOfIterations = 0;
  m_MaximumNumberOfIterations = 0;
  m_Tolerance = 0.0;
  m_TimeStep = 1.0;
  m_verbosity = 0;
}

//-----------------------------------------------------------------------------
void ParticleGradientDescentPositionOptimizer::ResetTimeStepVectors()
{
  // Make sure the time step vector is the right size
  while (m_TimeSteps.size() != m_ParticleSystem->GetNumberOfDomains()) {
    std::vector<double> tmp;
    m_TimeSteps.push_back(tmp);
  }

  for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++) {
    unsigned int np = m_ParticleSystem->GetPositions(i)->GetSize();
    if (m_TimeSteps[i].size() != np) {
      // resize and initialize everything to 1.0
      m_TimeSteps[i].resize(np);
    }
    for (unsigned int j = 0; j < np; j++) {
      m_TimeSteps[i][j] = 1.0;
    }
  }
}

//-----------------------------------------------------------------------------
void ParticleGradientDescentPositionOptimizer::StartAdaptiveGaussSeidelOptimization()
{
  /// uncomment this to run single threaded
  //tbb::task_scheduler_init init(1);

  if (this->m_AbortProcessing) {
    return;
  }
  const double factor = 1.1;

  // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
  // ADDED TO THE SYSTEM DURING OPTIMIZATION.

  m_StopOptimization = false;
  if (m_NumberOfIterations >= m_MaximumNumberOfIterations) {
    m_StopOptimization = true;
  }
  //m_GradientFunction->SetParticleSystem(m_ParticleSystem);

  typedef typename DomainType::VnlVectorType NormalType;

  ResetTimeStepVectors();
  double minimumTimeStep = 1.0;

  unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();

  unsigned int counter = 0;

  double maxchange = 0.0;
  while (m_StopOptimization == false) { // iterations loop

    double dampening = 1;
    int startDampening = m_MaximumNumberOfIterations / 2;
    if (m_NumberOfIterations > startDampening) {
      dampening =
        exp(-double(m_NumberOfIterations - startDampening) * 5.0 /
            double(m_MaximumNumberOfIterations - startDampening));
    }
    minimumTimeStep = dampening;

    maxchange = 0.0;

    const auto accTimerBegin = std::chrono::steady_clock::now();
    m_GradientFunction->SetParticleSystem(m_ParticleSystem);
    if (counter % global_iteration == 0) {
      m_GradientFunction->BeforeIteration();
    }
    counter++;

    if (counter % 1 == 0) {
      std::cerr << "about to check for swaps\n";
      this->CheckForSwaps();
    }

    // Iterate over each domain
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, numdomains},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t dom = r.begin(); dom < r.end(); ++dom) {

          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == true) {
            // note that this is really a 'continue' statement for the loop, but using TBB,
            // we are in an anonymous function, not a loop, so return is equivalent to continue here
            return;
          }

          const ParticleDomain* domain = m_ParticleSystem->GetDomain(dom);

          typename GradientFunctionType::Pointer localGradientFunction = m_GradientFunction;

          // must clone this as we are in a thread and the gradient function is not thread-safe
          localGradientFunction = m_GradientFunction->Clone();

          // Tell function which domain we are working on.
          localGradientFunction->SetDomainNumber(dom);

          // Iterate over each particle position
          for (auto k = 0; k < m_ParticleSystem->GetPositions(dom)->GetSize(); k++) {
            if (m_TimeSteps[dom][k] < minimumTimeStep) {
              m_TimeSteps[dom][k] = minimumTimeStep;
            }
            // Compute gradient update.
            double energy = 0.0;
            localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);
            // maximumUpdateAllowed is set based on some fraction of the distance between particles
            // This is to avoid particles shooting past their neighbors
            double maximumUpdateAllowed;
            VectorType original_gradient =
              localGradientFunction->Evaluate(k, dom, m_ParticleSystem, maximumUpdateAllowed,
                                              energy);

            PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

            // Step 1 Project the gradient vector onto the tangent plane
            VectorType original_gradient_projectedOntoTangentSpace =
              domain->ProjectVectorToSurfaceTangent(original_gradient, pt, k);

            double newenergy, gradmag;
            while (true) {
              // Step A scale the projected gradient by the current time step
              VectorType gradient =
                original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

              // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
              if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
                AugmentedLagrangianConstraints(gradient, pt, dom, maximumUpdateAllowed);
              }

              gradmag = gradient.magnitude();

              // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
              if (gradmag > maximumUpdateAllowed) {
                gradient = gradient * maximumUpdateAllowed / gradmag;
                gradmag = gradient.magnitude();
              }

              // Step D compute the new point position
              PointType newpoint = domain->UpdateParticlePosition(pt, k, gradient);

              // Step F update the point position in the particle system
              m_ParticleSystem->SetPosition(newpoint, k, dom);

              // Step G compute the new energy of the particle system
              newenergy = localGradientFunction->Energy(k, dom, m_ParticleSystem);

              if (newenergy < energy) { // good move, increase timestep for next time
                m_TimeSteps[dom][k] *= factor;
                if (gradmag > maxchange) { maxchange = gradmag; }
                break;
              }
              else {// bad move, reset point position and back off on timestep
                if (m_TimeSteps[dom][k] > minimumTimeStep) {
                  domain->ApplyConstraints(pt, k);
                  m_ParticleSystem->SetPosition(pt, k, dom);
                  domain->InvalidateParticlePosition(k);

                  m_TimeSteps[dom][k] /= factor;
                }
                else { // keep the move with timestep 1.0 anyway
                  if (gradmag > maxchange) { maxchange = gradmag; }
                  break;
                }
              }
            } // end while(true)
          } // for each particle
        }// for each domain
      });

    m_NumberOfIterations++;
    m_GradientFunction->AfterIteration();

    const auto accTimerEnd = std::chrono::steady_clock::now();
    const auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      accTimerEnd - accTimerBegin).count();

    if (m_verbosity > 2) {
      std::cout << m_NumberOfIterations << ". " << msElapsed << "ms";
#ifdef LOG_MEMORY_USAGE
      double vmUsage, residentSet;
      process_mem_usage(vmUsage, residentSet);
      std::cout << " | Mem=" << residentSet << "KB";
#endif
      std::cout << std::endl;
    }
    else if (m_verbosity > 1) {
      if (m_NumberOfIterations % (m_MaximumNumberOfIterations / 10) == 0) {
        std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange = " << maxchange
                  << ", minimumTimeStep = " <<
                  minimumTimeStep << std::endl;
      }
    }

    this->InvokeEvent(itk::IterationEvent());

    // Check for convergence.  Optimization is considered to have converged if
    // max number of iterations is reached or maximum distance moved by any
    // particle is less than the specified precision.
    if (maxchange < m_Tolerance) {
      std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange = " << maxchange
                << std::endl;
      m_StopOptimization = true;
    }

    if (m_NumberOfIterations >= m_MaximumNumberOfIterations) {
      m_StopOptimization = true;
    }
  }   // end while stop optimization
}

//-----------------------------------------------------------------------------
void
ParticleGradientDescentPositionOptimizer::AugmentedLagrangianConstraints(
  VectorType& gradient, const PointType& pt, const size_t& dom, const double& maximumUpdateAllowed)
{
  // Step B 2: Augmented lagrangian constraint method
  double gradmag = gradient.magnitude();

  if (gradmag > maximumUpdateAllowed) {
    gradient = gradient * maximumUpdateAllowed / gradmag;
    gradmag = gradient.magnitude();
  }

  PointType upd_pt;
  for (size_t n = 0; n < VDimension; n++) {
    upd_pt[n] = pt[n] - gradient[n];
  }

  double c = 1e0;
  double multiplier = 2;
  m_ParticleSystem->GetDomain(dom)->GetConstraints()->UpdateZs(upd_pt, c);
  VectorType constraint_energy = m_ParticleSystem->GetDomain(
    dom)->GetConstraints()->ConstraintsLagrangianGradient(upd_pt, pt, c);
  if (constraint_energy.magnitude() > multiplier * gradmag) {
    constraint_energy *= multiplier * gradmag / constraint_energy.magnitude();
  }
  //m_ParticleSystem->GetDomain(dom)->GetConstraints()->ViolationReport(pt);
  for (size_t n = 0; n < VDimension; n++) {
    gradient[n] += constraint_energy[n];
  }
  m_ParticleSystem->GetDomain(dom)->GetConstraints()->UpdateMus(upd_pt, c);
}

//-----------------------------------------------------------------------------
void ParticleGradientDescentPositionOptimizer::CheckForSwaps()
{
  auto shape_point = [=](int domain, int particle_id) {
    itk::Point<double> p1;
    int idx = particle_id * 6;
    for (int k = 0; k < 3; k++) {
      p1[k] = (*m_ShapeData)(idx + k, domain);
    }
    return p1;
  };

  std::cerr << "ZZZZZZZZZZZZZZZ " << m_ShapeData->rows() << " x " << m_ShapeData->cols() << "\n";


  int num_dims = m_ShapeData->rows();  // includes dimensions for normals, etc

  // compute the mean particles
  typedef vnl_matrix<double> vnl_matrix_type;
  vnl_matrix_type m_points_mean;
  m_points_mean.set_size(num_dims, 1);
  m_points_mean.fill(0.0);
  for (unsigned int i = 0; i < num_dims; i++) {
    m_points_mean.put(i, 0, (m_ShapeData->get_n_rows(i, 1)).mean());
  }

  auto mean = [=](int particle_id) {
    itk::Point<double> p1;
    int idx = particle_id * 6;
    for (int k = 0; k < 3; k++) {
      p1[k] = m_points_mean(idx + k, 0);
    }
    return p1;
  };


  int rows = 0;
  for (int i = 0; i < m_ParticleSystem->GetDomainsPerShape(); i++) {
    rows += VDimension * m_ParticleSystem->GetNumberOfParticles(i);  // rows will only include X/Y/Z
  }

  int num_particles = rows / 3;
  int num_samples = m_ShapeData->cols();

  double mean_nearest = 0;
  for (int i = 0; i < num_particles; i++) {
    double nearest = std::numeric_limits<double>::max();

    itk::Point<double> p1 = mean(i);

    for (int j = 0; j < num_particles; j++) {
      if (i == j) { continue; }

      itk::Point<double> p2 = mean(j);

      double dist = p1.EuclideanDistanceTo(p2);
      nearest = std::min<double>(nearest, dist);
    }
    mean_nearest += nearest;
  }

  mean_nearest /= num_particles;

  int num_shapes = num_samples;
  double neighborhood_radius = mean_nearest * 1.2;

  std::cerr << "Descent: mean_nearest = " << mean_nearest << "\n";

  auto ps = m_ParticleSystem;

  if (num_particles < 6) {
    return;
  }
  //for (int i = 0; i < num_particles; i++) {
  for (int p = 5; p < 6; p++) {
    using Neighborhood = ParticleSystemType::PointVectorType;
    std::vector<Neighborhood> neighborhoods;

    std::vector<int> counts(num_particles, 0);
    for (int d = 0; d < num_shapes; d++) {
      std::vector<double> m_CurrentWeights;
      std::vector<double> distances;

      Neighborhood n = ps->FindNeighborhoodPoints(ps->GetPosition(p, d), p, m_CurrentWeights,
                                                  distances, neighborhood_radius, d);
      neighborhoods.push_back(n);
      for (const auto& pair : n) {
        if (pair.Index == p) {
          std::cerr << "Why does it give itself?\n";
        } else {
          counts[pair.Index]++;
        }
      }
    }

    int max_val = *(max_element(std::begin(counts), std::end(counts))); // at most max_val agreement

    double threshold = num_shapes / 2.0;
    //auto count = std::count_if(v.begin(), v.end(),[&](auto const& val){ return val > threshold; });


    std::cerr << "neighborhood: ";
    std::vector<int> neighborhood;
    for (int i = 0; i < counts.size(); i++) {
      if (counts[i] > threshold) {
        std::cerr << i << " ";
        neighborhood.push_back(i);
      }
    }
    std::cerr << "\n";

    if (neighborhood.size() > 3) { // need at least 3 particles with agreement >50%
      for (int d = 0; d < num_shapes; d++) {
        if (d >= neighborhoods.size()) {
          std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
          return;
        }
        auto n = neighborhoods[d];
        int probably_bad = 0;
        for (int i=0;i<n.size();i++) {
          int particle_id = n[i].Index;
          if (particle_id >= counts.size()) {
            std::cerr << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n";
            return;
          }
          if (counts[particle_id] < threshold) {
            probably_bad++;
          }
        }
        if (probably_bad > n.size() / 3.0) {
          std::cerr << "Descent: perhaps domain " << d << ", particle " << p << " is flipped?\n";


          Global& global = Global::global();
          tbb::mutex::scoped_lock lock(global.mutex_);
          global.flipped_particles.clear();
          global.flipped_domains.clear();


          itk::Point<double> center;
          center[0] = 0;
          center[1] = 0;
          center[2] = 0;

          global.targets.push_back(center);
          global.flipped_particles.push_back(p);
          global.flipped_domains.push_back(d);
          global.neighbors = neighborhood;

        }
      }
    }

  }

}

} // end namespace
