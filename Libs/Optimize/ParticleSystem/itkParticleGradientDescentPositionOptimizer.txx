#pragma once

const int global_iteration = 1;

#include <algorithm>
#include <ctime>
#include <time.h>
#include <string>
#include "ParticleImageDomainWithGradients.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "MemoryUsage.h"
#include <chrono>

#include <tbb/parallel_for.h>
#include <tbb/task_scheduler_init.h>


namespace itk
{
  template <class TGradientNumericType, unsigned int VDimension>
  ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
    ::ParticleGradientDescentPositionOptimizer()
  {
    m_StopOptimization = false;
    m_NumberOfIterations = 0;
    m_MaximumNumberOfIterations = 0;
    m_Tolerance = 0.0;
    m_TimeStep = 1.0;
  }

  template <class TGradientNumericType, unsigned int VDimension>
  void ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>::ResetTimeStepVectors()
  {
    // Make sure the time step vector is the right size
    while (m_TimeSteps.size() != m_ParticleSystem->GetNumberOfDomains())
    {
      std::vector<double> tmp;
      m_TimeSteps.push_back(tmp);
    }

    for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++)
    {
      unsigned int np = m_ParticleSystem->GetPositions(i)->GetSize();
      if (m_TimeSteps[i].size() != np)
      {
        // resize and initialize everything to 1.0
        m_TimeSteps[i].resize(np);
      }
      for (unsigned int j = 0; j < np; j++)
      {
        m_TimeSteps[i][j] = 1.0;
      }
    }
  }

  template <class TGradientNumericType, unsigned int VDimension>
  void ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
    ::StartAdaptiveGaussSeidelOptimization()
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

    const double pi = std::acos(-1.0);
    unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();

    unsigned int counter = 0;

    double maxchange = 0.0;
    while (m_StopOptimization == false) // iterations loop
    {

      double dampening = 1;
      int startDampening = m_MaximumNumberOfIterations / 2;
      if (m_NumberOfIterations > startDampening) {
        dampening = exp(-double(m_NumberOfIterations - startDampening) * 5.0 / double(m_MaximumNumberOfIterations - startDampening));
      }
      minimumTimeStep = dampening;

      maxchange = 0.0;

      const auto accTimerBegin = std::chrono::steady_clock::now();
      m_GradientFunction->SetParticleSystem(m_ParticleSystem);
        if (counter % global_iteration == 0)
            m_GradientFunction->BeforeIteration();
        counter++;

        // Iterate over each domain
      const auto domains_per_shape = m_ParticleSystem->GetDomainsPerShape();
      tbb::parallel_for(
        tbb::blocked_range<size_t>{0, numdomains / domains_per_shape},
        [&](const tbb::blocked_range<size_t>& r) {
          for (size_t shape = r.begin(); shape < r.end(); ++shape) {
            for (int shape_dom_idx = 0; shape_dom_idx < domains_per_shape; shape_dom_idx++) {
              auto dom = shape*domains_per_shape + shape_dom_idx;

              // skip any flagged domains
              if (m_ParticleSystem->GetDomainFlag(dom) == true) {
                // note that this is really a 'continue' statement for the loop, but using TBB,
                // we are in an anonymous function, not a loop, so return is equivalent to continue here
                return;
              }

          const shapeworks::ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

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
              VectorType original_gradient = localGradientFunction->Evaluate(k, dom, m_ParticleSystem,
                                                                             maximumUpdateAllowed, energy);

              PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

              // Step 1 Project the gradient vector onto the tangent plane
              VectorType original_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(
                      original_gradient, pt, k);

              double newenergy, gradmag;
              while (true) {
                // Step A scale the projected gradient by the current time step
                VectorType gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

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

                if (newenergy < energy) // good move, increase timestep for next time
                {
                  m_TimeSteps[dom][k] *= factor;
                  if (gradmag > maxchange) maxchange = gradmag;
                  break;
                } else {// bad move, reset point position and back off on timestep
                  if (m_TimeSteps[dom][k] > minimumTimeStep) {
                    domain->ApplyConstraints(pt, k);
                    m_ParticleSystem->SetPosition(pt, k, dom);
                    domain->InvalidateParticlePosition(k);

                    m_TimeSteps[dom][k] /= factor;
                  } else // keep the move with timestep 1.0 anyway
                  {
                    if (gradmag > maxchange) maxchange = gradmag;
                    break;
                  }
                }
              } // end while(true)
            } // for each particle
          }
        }// for each domain
      });

      m_NumberOfIterations++;
      m_GradientFunction->AfterIteration();

      const auto accTimerEnd = std::chrono::steady_clock::now();
      const auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(accTimerEnd - accTimerBegin).count();

      if (m_verbosity > 2)
      {
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
          std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange = " << maxchange << ", minimumTimeStep = " << minimumTimeStep << std::endl;
        }
      }

      this->InvokeEvent(itk::IterationEvent());

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

    } // end while stop optimization
  }

template<class TGradientNumericType, unsigned int VDimension>
void
ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>::AugmentedLagrangianConstraints(
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
    dom)->GetConstraints()->constraintsLagrangianGradient(upd_pt, pt, c);
  if (constraint_energy.magnitude() > multiplier * gradmag) {
    constraint_energy *= multiplier * gradmag / constraint_energy.magnitude();
  }
  //m_ParticleSystem->GetDomain(dom)->GetConstraints()->ViolationReport(pt);
  for (size_t n = 0; n < VDimension; n++) {
    gradient[n] += constraint_energy[n];
  }
  m_ParticleSystem->GetDomain(dom)->GetConstraints()->UpdateMus(upd_pt, c);
}

// ------------------------------------------------------------------------
template<class TGradientNumericType, unsigned int VDimension>
std::vector<std::vector<size_t> > ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>::ComputeNeighborhoodsForAllParticles(std::vector<PointType> list){

}

template<class TGradientNumericType, unsigned int VDimension>
bool ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>:: CorrespondenceCorrection(){
    // Parameters
    double cutoff_factor = 2.;

    size_t num_doms = this->GetNumberOfDomains();

    std::vector< std::vector <std::vector<PointType> > >lists;

    // For each shape, for corresponding domains, populate all particles. i over shapes, j over domains in each shape, k over particles
    for (size_t i = 0; i < m_DomainsPerShape; i++){
        std::vector <std::vector<PointType> > lists_for_shape;

        for (size_t j = i; j < num_doms; j += m_DomainsPerShape) {
          std::vector<PointType> list;

          for (auto k = 0; k < GetPositions(j)->GetSize(); k++) {
            list.push_back(GetPositions(j)->Get(k));
          }
          lists_for_shape.push_back(list);
        }

        lists.push_back(lists_for_shape);
    }

    // Compute mean shape coordinates
    std::vector <std::vector<PointType> > mean;
    for (size_t i = 0; i < m_DomainsPerShape; i++){

        std::vector<PointType> mean_dom_i;

        if(m_DomainsPerShape>=1) for (auto k = 0; k < GetPositions(0)->GetSize(); k++) mean_dom_i.push_back(0.);

        for (size_t j = i; j < num_doms; j += m_DomainsPerShape) {

          for (auto k = 0; k < GetPositions(j)->GetSize(); k++) {
            auto pt = lists[i][j][k];
             mean_dom_i[k][0] += pt[0]; mean_dom_i[k][1] += pt[1]; mean_dom_i[k][2] += pt[2];
          }
        }

        if(m_DomainsPerShape>=1) {
            for (auto k = 0; k < GetPositions(0)->GetSize(); k++){
                mean_dom_i[k][0] = mean_dom_i[k][0]/GetPositions(0)->GetSize();
                mean_dom_i[k][1] = mean_dom_i[k][1]/GetPositions(0)->GetSize();
                mean_dom_i[k][2] = mean_dom_i[k][2]/GetPositions(0)->GetSize();
            }
        }

        mean.push_back(mean_dom_i);
    }

    // Compute neighborhood metric
    double units_per_domain;
    std::vector< std::vector <std::vector<PointType> > > pairwise_dists_per_domain;
    for (size_t i = 0; i < m_DomainsPerShape; i++){
        std::vector<PointType> list = mean[i];
        std::vector <std::vector<PointType> > pairwise_dists;

        double maxDistNN = 0;

        for(size_t i = 0; i < list.size(); i++){
            std::vector<PointType> i_dists;
            size_t closest_particle = 0;
            double closest_particle_distance = (list[i]-list[0]).GetNorm();
            for(size_t j = 1; j < list.size(); j++){
                double dist = (list[i]-list[j]).GetNorm();
                i_dists.push_back(dist);
                if(dist < closest_particle_distance){
                    closest_particle_distance = dist;
                    closest_particle = j;
                }
            }
            pairwise_dists.push_back(i_dists);
            if(closest_particle_distance > maxDistNN){
                maxDistNN = closest_particle_distance;
            }
        }
        pairwise_dists_per_domain.push_back(pairwise_dists);

        units_by_domain.push_back(maxDistNN);
    }

    // Compute cuttoffs
    double cuttoffs;

    // Compute neighborhoods
    std::vector< std::vector< std::vector <std::pair<double, size_t> > > > neighbors_by_domain;
    for (size_t i = 0; i < m_DomainsPerShape; i++){
        cuttoff.push_back(units_per_domain[i]*factor);
        std::vector <std::vector<PointType> > pairwise_dists = pairwise_dists_of_domain[i];
        std::vector< std::vector <std::pair<double, size_t> > > neighbors_of_domain;

        for(size_t i = 0; i < pairwise_dists.size(); i++){
            std::vector <std::pair<double, size_t> > neighbors_of_particle_i;
            for(size_t j = 1; j < pairwise_dists.size(); j++){
                if(pairwise_dists[i][j] < cuttoffs[i]){
                    std::pair<double, size_t> pair;
                    pair.first = pairwise_dists[i][j];
                    pair.second = j;
                    neighbors_of_particle_i.push_back(pair);
                }
            }
            std::sort(neighbors_of_particle_i.begin(), neighbors_of_particle_i.end());
        }
    }
}

} // end namespace
