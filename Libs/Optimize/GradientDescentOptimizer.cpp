

#include "GradientDescentOptimizer.h"

const int global_iteration = 1;

#include <Logging.h>
#include <Profiling.h>
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
#include "Libs/Optimize/Function/EarlyStop/EarlyStopping.h"
#include "Libs/Optimize/Utils/MemoryUsage.h"

namespace shapeworks {
GradientDescentOptimizer::GradientDescentOptimizer() : early_stopping_() {}

void GradientDescentOptimizer::ResetTimeStepVectors() {
  // Make sure the time step vector is the right size
  while (time_steps_.size() != particle_system_->GetNumberOfDomains()) {
    std::vector<double> tmp;
    time_steps_.push_back(tmp);
  }

  for (unsigned int i = 0; i < particle_system_->GetNumberOfDomains(); i++) {
    unsigned int np = particle_system_->GetPositions(i)->GetSize();
    if (time_steps_[i].size() != np) {
      // resize and initialize everything to 1.0
      time_steps_[i].resize(np);
    }
    for (unsigned int j = 0; j < np; j++) {
      time_steps_[i][j] = 1.0;
    }
  }
}

void GradientDescentOptimizer::SetEarlyStoppingConfig(const EarlyStoppingConfig& config) {
  early_stopping_.SetConfigParams(config.frequency, config.window_size, config.threshold, config.strategy,
                                  config.ema_alpha, config.enable_logging, config.logger_name, config.warmup_iters);
  early_stopping_enabled_ = config.enabled;
}

void GradientDescentOptimizer::InitializeEarlyStoppingScoreFunction(const ParticleSystem* p) {
  bool early_stopping_status = early_stopping_.SetControlShapes(p);
  if (early_stopping_status == false) {
    SW_WARN(
        "Early stopping has been forcibly disabled. Possible causes: no fixed "
        "shapes/domains "
        "to fit PCA, or PCA fitting failed. Check logs for details.");
  }
  early_stopping_score_ready_ = early_stopping_status;
  early_stopping_enabled_ = early_stopping_status;
}

void GradientDescentOptimizer::StartAdaptiveGaussSeidelOptimization() {
  TIME_SCOPE("GradientDescentOptimizer");
  /// uncomment this to run single threaded
  // tbb::task_scheduler_init init(1);

  if (this->abort_processing_) {
    return;
  }
  const double factor = 1.1;

  // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
  // ADDED TO THE SYSTEM DURING OPTIMIZATION.

  stop_optimization_ = false;
  if (number_of_iterations_ >= max_iterations_) {
    stop_optimization_ = true;
  }
  // gradient_function_->SetParticleSystem(particle_system_);

  ResetTimeStepVectors();
  double minimumTimeStep = 1.0;

  const double pi = std::acos(-1.0);
  unsigned int numdomains = particle_system_->GetNumberOfDomains();

  unsigned int counter = 0;

  double maxchange = 0.0;
  if (early_stopping_enabled_) early_stopping_.reset();  // reset early stopping cache before starting optimization
  while (stop_optimization_ == false)                   // iterations loop
  {
    TIME_SCOPE("optimizer_iteration");
    double dampening = 1;
    int startDampening = max_iterations_ / 2;
    if (number_of_iterations_ > startDampening) {
      dampening = exp(-double(number_of_iterations_ - startDampening) * 5.0 /
                      double(max_iterations_ - startDampening));
    }
    minimumTimeStep = dampening;

    maxchange = 0.0;

    const auto accTimerBegin = std::chrono::steady_clock::now();
    gradient_function_->SetParticleSystem(particle_system_.GetPointer());
    if (early_stopping_enabled_ && !early_stopping_score_ready_) {
      bool early_stopping_status = early_stopping_.SetControlShapes(particle_system_.GetPointer());
      if (early_stopping_status == false) {
        SW_WARN(
            "Early stopping has been forcibly disabled. Possible causes: no fixed shapes/domains "
            "to fit PCA, or PCA fitting failed. Check logs.");
      }
      early_stopping_enabled_ = early_stopping_status;  // forcibly turn off early stopping if no fixed domains present
      early_stopping_score_ready_ = early_stopping_status;
    }

    TIME_START("gradient_before_iteration");
    if (counter % global_iteration == 0) gradient_function_->BeforeIteration();
    TIME_STOP("gradient_before_iteration");
    counter++;

    TIME_START("parallel_sampling");
    // Iterate over each domain
    const auto domains_per_shape = particle_system_->GetDomainsPerShape();
    tbb::parallel_for(tbb::blocked_range<size_t>{0, numdomains / domains_per_shape},
                      [&](const tbb::blocked_range<size_t>& r) {
                        for (size_t shape = r.begin(); shape < r.end(); ++shape) {
                          for (int shape_dom_idx = 0; shape_dom_idx < domains_per_shape; shape_dom_idx++) {
                            auto dom = shape * domains_per_shape + shape_dom_idx;

                            // skip any flagged domains
                            if (particle_system_->GetDomainFlag(dom) == true) {
                              // note that this is really a 'continue' statement for the loop, but using TBB,
                              // we are in an anonymous function, not a loop, so return is equivalent to continue here
                              return;
                            }

                            const ParticleDomain* domain = particle_system_->GetDomain(dom);

                            // must clone this as we are in a thread and the gradient function is not thread-safe
                            VectorFunction::Pointer localGradientFunction = gradient_function_->Clone();

                            // Tell function which domain we are working on.
                            localGradientFunction->SetDomainNumber(dom);

                            // Iterate over each particle position
                            for (auto k = 0; k < particle_system_->GetPositions(dom)->GetSize(); k++) {
                              if (time_steps_[dom][k] < minimumTimeStep) {
                                time_steps_[dom][k] = minimumTimeStep;
                              }
                              // Compute gradient update.
                              double energy = 0.0;
                              localGradientFunction->BeforeEvaluate(k, dom, particle_system_.GetPointer());
                              // maximumUpdateAllowed is set based on some fraction of the distance between particles
                              // This is to avoid particles shooting past their neighbors
                              double maximumUpdateAllowed;
                              VectorType original_gradient = localGradientFunction->Evaluate(
                                  k, dom, particle_system_.GetPointer(), maximumUpdateAllowed, energy);

                              PointType pt = particle_system_->GetPositions(dom)->Get(k);

                              // Step 1 Project the gradient vector onto the tangent plane
                              VectorType original_gradient_projectedOntoTangentSpace =
                                  domain->ProjectVectorToSurfaceTangent(original_gradient, pt, k);

                              double newenergy, gradmag;
                              while (true) {
                                // Step A scale the projected gradient by the current time step
                                VectorType gradient = original_gradient_projectedOntoTangentSpace * time_steps_[dom][k];

                                // Step B Constrain the gradient so that the resulting position will not violate any
                                // domain constraints
                                if (particle_system_->GetDomain(dom)->GetConstraints()->GetActive()) {
                                  AugmentedLagrangianConstraints(gradient, pt, dom, maximumUpdateAllowed, k);
                                }

                                gradmag = gradient.magnitude();

                                // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to
                                // an acceptable magnitude
                                if (gradmag > maximumUpdateAllowed) {
                                  gradient = gradient * maximumUpdateAllowed / gradmag;
                                  gradmag = gradient.magnitude();
                                }

                                // Step D compute the new point position
                                PointType newpoint = domain->UpdateParticlePosition(pt, k, gradient);

                                // Step F update the point position in the particle system
                                particle_system_->SetPosition(newpoint, k, dom);

                                // Step G compute the new energy of the particle system
                                newenergy = localGradientFunction->Energy(k, dom, particle_system_.GetPointer());

                                if (newenergy < energy)  // good move, increase timestep for next time
                                {
                                  time_steps_[dom][k] *= factor;
                                  if (gradmag > maxchange) maxchange = gradmag;
                                  break;
                                } else {  // bad move, reset point position and back off on timestep
                                  if (time_steps_[dom][k] > minimumTimeStep) {
                                    domain->ApplyConstraints(pt, k);
                                    particle_system_->SetPosition(pt, k, dom);
                                    domain->InvalidateParticlePosition(k);

                                    time_steps_[dom][k] /= factor;
                                  } else  // keep the move with timestep 1.0 anyway
                                  {
                                    if (gradmag > maxchange) maxchange = gradmag;
                                    break;
                                  }
                                }
                              }  // end while(true)
                            }  // for each particle
                          }
                        }  // for each domain
                      });

    TIME_STOP("parallel_sampling");
    number_of_iterations_++;
    gradient_function_->AfterIteration();

    const auto accTimerEnd = std::chrono::steady_clock::now();
    const auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(accTimerEnd - accTimerBegin).count();

    if (verbosity_ > 2) {
      std::cout << number_of_iterations_ << ". " << msElapsed << "ms";
#ifdef LOG_MEMORY_USAGE
      double vmUsage, residentSet;
      process_mem_usage(vmUsage, residentSet);
      std::cout << " | Mem=" << residentSet << "KB";
#endif
      std::cout << std::endl;
    } else if (verbosity_ > 1) {
      if (number_of_iterations_ % (max_iterations_ / 10) == 0) {
        std::cerr << "Iteration " << number_of_iterations_ << ", maxchange = " << maxchange
                  << ", minimumTimeStep = " << minimumTimeStep << std::endl;
      }
    }

    // Call iteration callback if set
    if (iteration_callback_) {
      iteration_callback_();
    }

    // Check for convergence.  Optimization is considered to have converged if
    // max number of iterations is reached or maximum distance moved by any
    // particle is less than the specified precision.
    if (maxchange < tolerance_) {
      std::cerr << "Iteration " << number_of_iterations_ << ", maxchange = " << maxchange << std::endl;
      stop_optimization_ = true;
    }

    if (number_of_iterations_ >= max_iterations_) {
      stop_optimization_ = true;
    }

    if (early_stopping_enabled_) {
      early_stopping_.update(number_of_iterations_, particle_system_.GetPointer());
      if (early_stopping_.ShouldStop()) {
        std::cerr << "Early stopping triggered at optimization iteration " << number_of_iterations_ << std::endl;
        SW_LOG("Early stopping triggered at optimization iteration {}", number_of_iterations_);
        stop_optimization_ = true;
      }
    }

  }  // end while stop optimization
}

void GradientDescentOptimizer::AugmentedLagrangianConstraints(VectorType& gradient, const PointType& pt,
                                                              const size_t& dom, const double& maximumUpdateAllowed,
                                                              size_t index) {
  // Step B 2: Augmented lagrangian constraint method
  double gradmag = gradient.magnitude();

  if (gradmag > maximumUpdateAllowed) {
    gradient = gradient * maximumUpdateAllowed / gradmag;
    gradmag = gradient.magnitude();
  }

  PointType upd_pt;
  for (size_t n = 0; n < Dimension; n++) {
    upd_pt[n] = pt[n] - gradient[n];
  }

  double c = 1e0;
  double multiplier = 2;
  VectorType constraint_energy =
      particle_system_->GetDomain(dom)->GetConstraints()->constraintsLagrangianGradient(upd_pt, pt, c, index);
  if (constraint_energy.magnitude() > multiplier * gradmag) {
    constraint_energy *= multiplier * gradmag / constraint_energy.magnitude();
  }
  // particle_system_->GetDomain(dom)->GetConstraints()->ViolationReport(pt);
  for (size_t n = 0; n < Dimension; n++) {
    gradient[n] += constraint_energy[n];
  }
  particle_system_->GetDomain(dom)->GetConstraints()->UpdateMus(upd_pt, c, index);
}

}  // namespace shapeworks
