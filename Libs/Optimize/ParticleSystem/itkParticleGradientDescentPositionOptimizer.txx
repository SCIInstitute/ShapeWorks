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
    time_t timerBefore, timerAfter;

    double maxchange = 0.0;
    while (m_StopOptimization == false) // iterations loop
    {
      m_GradientFunction->SetParticleSystem(m_ParticleSystem);
      timerBefore = time(NULL);
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
            double maxdt;

            VectorType gradient;
            PointType newpoint;

            const DomainType* domain = static_cast<const DomainType*>(m_ParticleSystem->GetDomain(dom));

            typename GradientFunctionType::Pointer localGradientFunction;

            localGradientFunction = m_GradientFunction;

#ifdef SW_USE_OPENMP
            localGradientFunction = m_GradientFunction->Clone();
#endif /* SW_USE_OPENMP */


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
              bool done = false;

              // Compute gradient update.
              double energy = 0.0;
              localGradientFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
              gradient = localGradientFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem, maxdt, energy);

              unsigned int idx = it.GetIndex();
              PointType pt = *it;

              double newenergy, gradmag;
              while (!done)
              {
                gradient = gradient * m_TimeSteps[dom][k];

                domain->ApplyVectorConstraints(gradient, *it);

                gradmag = gradient.magnitude();

                if (gradmag > maxdt)
                {
                  m_TimeSteps[dom][k] /= factor;
                }
                else // Move is not too large
                {
                  // Make a move and compute new energy
                  for (unsigned int i = 0; i < VDimension; i++)
                  {
                    newpoint[i] = pt[i] - gradient[i];
                  }

                  domain->ApplyConstraints(newpoint);

                  m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);

                  newenergy = localGradientFunction->Energy(it.GetIndex(), dom, m_ParticleSystem);

                  if (newenergy < energy) // good move, increase timestep for next time
                  {
                    meantime[dom] += m_TimeSteps[dom][k];
                    m_TimeSteps[dom][k] *= factor;
                    if (m_TimeSteps[dom][k] > maxtime[dom]) m_TimeSteps[dom][k] = maxtime[dom];
                    if (gradmag > maxchange) maxchange = gradmag;
                    done = true;
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
                      done = true;
                    }
                  }
                } //gradmag check
              } // end while not done

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

      timerAfter = time(NULL);
      double seconds = difftime(timerAfter, timerBefore);

      if (m_verbosity > 2)
      {
        std::cout << m_NumberOfIterations << ". " << seconds << " seconds.. ";
        std::cout.flush();
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
