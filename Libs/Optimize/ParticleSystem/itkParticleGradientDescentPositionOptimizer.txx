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
    //m_GradientFunction->SetParticleSystem(m_ParticleSystem);

    typedef typename DomainType::VnlVectorType NormalType;

    ResetTimeStepVectors();

    const double pi = std::acos(-1.0);
    unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();
    std::vector<double> meantime(numdomains);
    std::vector<double> maxtime(numdomains);
    std::vector<double> mintime(numdomains);

    unsigned int counter = 0;

    for (unsigned int q = 0; q < numdomains; q++)
    {
      meantime[q] = 0.0;
      maxtime[q] = 1.0e30;
      mintime[q] = 1.0;
    }

    double maxchange = 0.0;
    while (m_StopOptimization == false) // iterations loop
    {
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
          meantime[dom] = 0.0;
          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == false)
          {

            const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

            typename GradientFunctionType::Pointer localGradientFunction = m_GradientFunction;
#ifdef SW_USE_OPENMP
            localGradientFunction = m_GradientFunction->Clone();
#endif

            // Tell function which domain we are working on.
            localGradientFunction->SetDomainNumber(dom);

            // Iterate over each particle position
            unsigned int k = 0;
            maxchange = 0.0;
            typename ParticleSystemType::PointContainerType::ConstIterator endit =
              m_ParticleSystem->GetPositions(dom)->GetEnd();
            for (typename ParticleSystemType::PointContainerType::ConstIterator it
              = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
            {
              VectorType gradient;
              VectorType original_gradient;
              // Compute gradient update.
              double energy = 0.0;
              localGradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
              double maximumDTUpdateAllowed;
              original_gradient = localGradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem, maximumDTUpdateAllowed, energy);

              unsigned int idx = it.GetIndex();
              PointType pt = *it;

              // Step 1 Project the gradient vector onto the tangent plane
              VectorType original_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_gradient, pt);

              // Step 2 scale the gradient by the time step
              // Note that time step can only decrease while finding a good update so the gradient computed here is 
              // the largest possible we can get during this update.
              gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

              double newenergy, gradmag;
              while (true)
              { 
                // Step A scale the projected gradient by the current time step
                gradient = original_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

                // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
                m_ParticleSystem->GetDomain(dom)->ApplyVectorConstraints(gradient, m_ParticleSystem->GetPosition(it.GetIndex(), dom));
                gradmag = gradient.magnitude();

                // Step C if the magnitude is larger than the Sampler allows, try again with smaller time step
                if (gradmag > maximumDTUpdateAllowed)
                {
                  m_TimeSteps[dom][k] /= factor;
                  continue;
                }

                // Step D compute the new point position
                //std::cerr << "pt: " << pt[0] << ", " << pt[1] << ", " << pt[2] << "\n";
                //std::cerr << "gradient: " << gradient[0] << ", " << gradient[1] << ", " << gradient[2] << "\n";
                PointType newpoint = domain->UpdateParticlePosition(pt, gradient);
                //std::cerr << "newpoint: " << newpoint[0] << ", " << newpoint[1] << ", " << newpoint[2] << "\n";

                // Step F update the point position in the particle system
                m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);

                // Step G compute the new energy of the particle system 
                newenergy = localGradientFunction->Energy(it.GetIndex(), dom, m_ParticleSystem);

                if (newenergy < energy) // good move, increase timestep for next time
                {
                  meantime[dom] += m_TimeSteps[dom][k];
                  m_TimeSteps[dom][k] *= factor;
                  if (m_TimeSteps[dom][k] > maxtime[dom]) m_TimeSteps[dom][k] = maxtime[dom];
                  if (gradmag > maxchange) maxchange = gradmag;
                  break;
                }
                else
                {// bad move, reset point position and back off on timestep
                  if (m_TimeSteps[dom][k] > mintime[dom])
                  {
                    domain->ApplyConstraints(pt);
                    m_ParticleSystem->SetPosition(pt, it.GetIndex(), dom);

                    m_TimeSteps[dom][k] /= factor;
                  }
                  else // keep the move with timestep 1.0 anyway
                  {
                    break;
                  }
                }
              } // end while(true)
            } // for each particle

            // Compute mean time step
            meantime[dom] /= static_cast<double>(k);

            if (meantime[dom] < 1.0) meantime[dom] = 1.0;
            //                    //        std::cout << "meantime = " << meantime[dom] << std::endl;
            maxtime[dom] = meantime[dom] + meantime[dom] * 0.2;
            mintime[dom] = meantime[dom] - meantime[dom] * 0.1;
          } // if not flagged
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

      this->InvokeEvent(itk::IterationEvent());
      // Check for convergence.  Optimization is considered to have converged if
      // max number of iterations is reached or maximum distance moved by any
      // particle is less than the specified precision.
      //    std::cout << "maxchange = " << maxchange << std::endl;

      if ((m_NumberOfIterations >= m_MaximumNumberOfIterations)
        || (m_Tolerance > 0.0 && maxchange < m_Tolerance))
      {
        m_StopOptimization = true;
      }

    } // end while stop optimization
  }

} // end namespace

#endif
