

#include "GradientDescentOptimizer.h"

const int global_iteration = 1;

#include <tbb/parallel_for.h>
#include <time.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Libs/Optimize/Domain/ImageDomainWithGradients.h"
#include "Libs/Optimize/Utils/MemoryUsage.h"

#include <Profiling.h>
#include "Libs/Optimize/Function/EarlyStop/EarlyStopping.h"
#include <Logging.h>

namespace shapeworks {
GradientDescentOptimizer::GradientDescentOptimizer() 
  : m_EarlyStopping()
{
  m_StopOptimization = false;
  m_NumberOfIterations = 0;
  m_MaximumNumberOfIterations = 0;
  m_Tolerance = 0.0;
  m_TimeStep = 1.0;
}

void GradientDescentOptimizer::ResetTimeStepVectors() {
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

void GradientDescentOptimizer::SetEarlyStoppingConfig(const EarlyStoppingConfig& config) {
    m_EarlyStopping.SetConfigParams(
        config.frequency,
        config.window_size,
        config.threshold,
        config.strategy,
        config.ema_alpha,
        config.enable_logging,
        config.logger_name,
        config.warmup_iters
    );
    m_EarlyStoppingEnabled = config.enabled;
}

void GradientDescentOptimizer::InitializeEarlyStoppingScoreFunction(
    const ParticleSystemType* p) {
  bool early_stopping_status = m_EarlyStopping.SetControlShapes(p);
  if (early_stopping_status == false) {
    SW_WARN(
        "Early stopping has been forcibly disabled. Possible causes: no fixed "
        "shapes/domains "
        "to fit PCA, or PCA fitting failed. Check logs for details.");
  }
  m_EarlyStoppingScoreFunctionReady = early_stopping_status;
  m_EarlyStoppingEnabled = early_stopping_status;
}

void GradientDescentOptimizer::StartAdaptiveGaussSeidelOptimization() {
  TIME_SCOPE("GradientDescentOptimizer");
  /// uncomment this to run single threaded
  // tbb::task_scheduler_init init(1);

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
  // m_GradientFunction->SetParticleSystem(m_ParticleSystem);

  typedef typename DomainType::VnlVectorType NormalType;

  ResetTimeStepVectors();
  double minimumTimeStep = 1.0;

  const double pi = std::acos(-1.0);
  unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();

  unsigned int counter = 0;

  double maxchange = 0.0;
  if (m_EarlyStoppingEnabled) m_EarlyStopping.reset(); // reset early stopping cache before starting optimization
  while (m_StopOptimization == false)  // iterations loop
  {
    TIME_SCOPE("optimizer_iteration");
    double dampening = 1;
    int startDampening = m_MaximumNumberOfIterations / 2;
    if (m_NumberOfIterations > startDampening) {
      dampening = exp(-double(m_NumberOfIterations - startDampening) * 5.0 /
                      double(m_MaximumNumberOfIterations - startDampening));
    }
    minimumTimeStep = dampening;

    maxchange = 0.0;

    const auto accTimerBegin = std::chrono::steady_clock::now();
    m_GradientFunction->SetParticleSystem(m_ParticleSystem.GetPointer());
    if (m_EarlyStoppingEnabled && !m_EarlyStoppingScoreFunctionReady) {
      bool early_stopping_status = m_EarlyStopping.SetControlShapes(m_ParticleSystem.GetPointer());
      if (early_stopping_status == false) {
        SW_WARN(
            "Early stopping has been forcibly disabled. Possible causes: no fixed shapes/domains "
            "to fit PCA, or PCA fitting failed. Check logs.");
      }
      m_EarlyStoppingEnabled = early_stopping_status; // forcibly turn off early stopping if no fixed domains present
      m_EarlyStoppingScoreFunctionReady = early_stopping_status;
    }

    TIME_START("gradient_before_iteration");
    if (counter % global_iteration == 0) m_GradientFunction->BeforeIteration();
    TIME_STOP("gradient_before_iteration");
    counter++;

    TIME_START("parallel_sampling");
    // Iterate over each domain
    const auto domains_per_shape = m_ParticleSystem->GetDomainsPerShape();
    tbb::parallel_for(
        tbb::blocked_range<size_t>{0, numdomains / domains_per_shape}, [&](const tbb::blocked_range<size_t>& r) {
          for (size_t shape = r.begin(); shape < r.end(); ++shape) {
            for (int shape_dom_idx = 0; shape_dom_idx < domains_per_shape; shape_dom_idx++) {
              auto dom = shape * domains_per_shape + shape_dom_idx;

              // skip any flagged domains
              if (m_ParticleSystem->GetDomainFlag(dom) == true) {
                // note that this is really a 'continue' statement for the loop, but using TBB,
                // we are in an anonymous function, not a loop, so return is equivalent to continue here
                return;
              }

              const shapeworks::ParticleDomain* domain = m_ParticleSystem->GetDomain(dom);

              // must clone this as we are in a thread and the gradient function is not thread-safe
              typename GradientFunctionType::Pointer localGradientFunction = m_GradientFunction->Clone();

              // Tell function which domain we are working on.
              localGradientFunction->SetDomainNumber(dom);

              // Iterate over each particle position
              for (auto k = 0; k < m_ParticleSystem->GetPositions(dom)->GetSize(); k++) {
                if (m_TimeSteps[dom][k] < minimumTimeStep) {
                  m_TimeSteps[dom][k] = minimumTimeStep;
                }
                // Compute gradient update.
                double energy = 0.0;
                localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem.GetPointer());
                // maximumUpdateAllowed is set based on some fraction of the distance between particles
                // This is to avoid particles shooting past their neighbors
                double maximumUpdateAllowed;
                VectorType original_gradient =
                    localGradientFunction->Evaluate(k, dom, m_ParticleSystem.GetPointer(), maximumUpdateAllowed, energy);

                PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

                // Step 1 Project the gradient vector onto the tangent plane
                VectorType original_gradient_projectedOntoTangentSpace =
                    domain->ProjectVectorToSurfaceTangent(original_gradient, pt, k);

                double newenergy, gradmag;
                while (true) {
                  // Step A scale the projected gradient by the current time step
                  VectorType gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

                  // Step B Constrain the gradient so that the resulting position will not violate any domain
                  // constraints
                  if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
                    AugmentedLagrangianConstraints(gradient, pt, dom, maximumUpdateAllowed, k);
                  }

                  gradmag = gradient.magnitude();

                  // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable
                  // magnitude
                  if (gradmag > maximumUpdateAllowed) {
                    gradient = gradient * maximumUpdateAllowed / gradmag;
                    gradmag = gradient.magnitude();
                  }

                  // Step D compute the new point position
                  PointType newpoint = domain->UpdateParticlePosition(pt, k, gradient);

                  // Step F update the point position in the particle system
                  m_ParticleSystem->SetPosition(newpoint, k, dom);

                  // Step G compute the new energy of the particle system
                  newenergy = localGradientFunction->Energy(k, dom, m_ParticleSystem.GetPointer());

                  if (newenergy < energy)  // good move, increase timestep for next time
                  {
                    m_TimeSteps[dom][k] *= factor;
                    if (gradmag > maxchange) maxchange = gradmag;
                    break;
                  } else {  // bad move, reset point position and back off on timestep
                    if (m_TimeSteps[dom][k] > minimumTimeStep) {
                      domain->ApplyConstraints(pt, k);
                      m_ParticleSystem->SetPosition(pt, k, dom);
                      domain->InvalidateParticlePosition(k);

                      m_TimeSteps[dom][k] /= factor;
                    } else  // keep the move with timestep 1.0 anyway
                    {
                      if (gradmag > maxchange) maxchange = gradmag;
                      break;
                    }
                  }
                }  // end while(true)
              }    // for each particle
            }
          }  // for each domain
        });

    TIME_STOP("parallel_sampling");
    m_NumberOfIterations++;
    m_GradientFunction->AfterIteration();

    const auto accTimerEnd = std::chrono::steady_clock::now();
    const auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(accTimerEnd - accTimerBegin).count();

    if (m_verbosity > 2) {
      std::cout << m_NumberOfIterations << ". " << msElapsed << "ms";
#ifdef LOG_MEMORY_USAGE
      double vmUsage, residentSet;
      process_mem_usage(vmUsage, residentSet);
      std::cout << " | Mem=" << residentSet << "KB";
#endif
      std::cout << std::endl;
    } else if (m_verbosity > 1) {
      if (m_NumberOfIterations % (m_MaximumNumberOfIterations / 10) == 0) {
        std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange = " << maxchange
                  << ", minimumTimeStep = " << minimumTimeStep << std::endl;
      }
    }

    // Call iteration callback if set
    if (m_iteration_callback) {
      m_iteration_callback();
    }

    // Check for convergence.  Optimization is considered to have converged if
    // max number of iterations is reached or maximum distance moved by any
    // particle is less than the specified precision.
    if (maxchange < m_Tolerance) {
      std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange = " << maxchange << std::endl;
      m_StopOptimization = true;
    }

    if (m_NumberOfIterations >= m_MaximumNumberOfIterations) {
      m_StopOptimization = true;
    }

    if (m_EarlyStoppingEnabled) {
      m_EarlyStopping.update(m_NumberOfIterations, m_ParticleSystem.GetPointer());
      if (m_EarlyStopping.ShouldStop()) {
        std::cerr << "Early stopping triggered at optimization iteration "
                  << m_NumberOfIterations << std::endl;
        SW_LOG("Early stopping triggered at optimization iteration {}",
                 m_NumberOfIterations);
        m_StopOptimization = true;
      }
    }

  }  // end while stop optimization
}

void GradientDescentOptimizer::AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt,
                                                              const size_t& dom, const double& maximumUpdateAllowed, size_t index) {
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
  VectorType constraint_energy =
      m_ParticleSystem->GetDomain(dom)->GetConstraints()->constraintsLagrangianGradient(upd_pt, pt, c, index);
  if (constraint_energy.magnitude() > multiplier * gradmag) {
    constraint_energy *= multiplier * gradmag / constraint_energy.magnitude();
  }
  // m_ParticleSystem->GetDomain(dom)->GetConstraints()->ViolationReport(pt);
  for (size_t n = 0; n < VDimension; n++) {
    gradient[n] += constraint_energy[n];
  }
  m_ParticleSystem->GetDomain(dom)->GetConstraints()->UpdateMus(upd_pt, c, index);
}

}  // namespace shapeworks
