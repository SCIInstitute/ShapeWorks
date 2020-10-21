/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGradientDescentPositionOptimizer_txx
#define __itkParticleGradientDescentPositionOptimizer_txx

#ifdef SW_USE_OPENMP
#include <omp.h>
#endif /* SW_USE_OPENMP */
const int global_iteration = 1;

#include <algorithm>
#include <ctime>
#include <time.h>
#include <string>
#include "itkParticleImageDomainWithGradients.h"
#include <vector>
#include <fstream>
#include <sstream>
#include "MemoryUsage.h"
#include <chrono>

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

#pragma omp parallel
      {
        // Iterate over each domain
#pragma omp for
        for (int dom = 0; dom < numdomains; dom++)
        {
          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == true)
          {
            continue;
          }

          const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

          typename GradientFunctionType::Pointer localGradientFunction = m_GradientFunction;
#ifdef SW_USE_OPENMP
          localGradientFunction = m_GradientFunction->Clone();
#endif

          // Tell function which domain we are working on.
          localGradientFunction->SetDomainNumber(dom);

          // Iterate over each particle position
          unsigned int k = 0;
          typename ParticleSystemType::PointContainerType::ConstIterator endit =
            m_ParticleSystem->GetPositions(dom)->GetEnd();
          for (typename ParticleSystemType::PointContainerType::ConstIterator it
            = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
          {
            if (m_TimeSteps[dom][k] < minimumTimeStep) {
              m_TimeSteps[dom][k] = minimumTimeStep;
            }
            // Compute gradient update.
            double energy = 0.0;
            localGradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
            // maximumUpdateAllowed is set based on some fraction of the distance between particles
            // This is to avoid particles shooting past their neighbors
            double maximumUpdateAllowed;
            VectorType original_gradient = localGradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem, maximumUpdateAllowed, energy);

            unsigned int idx = it.GetIndex();
            PointType pt = *it;

            // Step 1 Project the gradient vector onto the tangent plane
            VectorType original_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_gradient, pt);

            double newenergy, gradmag;
            while (true) {
              // Step A scale the projected gradient by the current time step
              VectorType gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

              // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
              m_ParticleSystem->GetDomain(dom)->GetConstraints()->applyBoundaryConstraints(gradient, m_ParticleSystem->GetPosition(it.GetIndex(), dom));

              gradmag = gradient.magnitude();

              // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
              if (gradmag > maximumUpdateAllowed) {
                gradient = gradient * maximumUpdateAllowed / gradmag;
                gradmag = gradient.magnitude();
              }

              // Step D compute the new point position
              PointType newpoint = domain->UpdateParticlePosition(pt, gradient);

              // Step F update the point position in the particle system
              m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);

              // Step G compute the new energy of the particle system
              newenergy = localGradientFunction->Energy(it.GetIndex(), dom, m_ParticleSystem);

              if (newenergy < energy) // good move, increase timestep for next time
              {
                m_TimeSteps[dom][k] *= factor;
                if (gradmag > maxchange) maxchange = gradmag;
                break;
              }
              else
              {// bad move, reset point position and back off on timestep
                if (m_TimeSteps[dom][k] > minimumTimeStep)
                {
                  domain->ApplyConstraints(pt);
                  m_ParticleSystem->SetPosition(pt, it.GetIndex(), dom);

                  m_TimeSteps[dom][k] /= factor;
                }
                else // keep the move with timestep 1.0 anyway
                {
                  if (gradmag > maxchange) maxchange = gradmag;
                  break;
                }
              }
            } // end while(true)
          } // for each particle
        }// for each domain
      }

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

} // end namespace

#endif
