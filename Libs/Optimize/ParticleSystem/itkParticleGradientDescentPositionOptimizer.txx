/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleGradientDescentPositionOptimizer_txx
#define __itkParticleGradientDescentPositionOptimizer_txx

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

    if(m_mlpca_optimize == true){
      while (m_TimeStepsWithin.size() != m_ParticleSystem->GetNumberOfDomains())
      {
        std::vector<double> tmp1;
        m_TimeStepsWithin.push_back(tmp1);
        std::vector<double> tmp2;
        m_TimeStepsBetween.push_back(tmp2);
      }

      for (unsigned int i = 0; i < m_ParticleSystem->GetNumberOfDomains(); i++)
      {
        unsigned int np = m_ParticleSystem->GetPositions(i)->GetSize();
        if (m_TimeStepsWithin[i].size() != np)
        {
          // resize and initialize everything to 1.0
          m_TimeStepsWithin[i].resize(np);
          m_TimeStepsBetween[i].resize(np);
        }
        for (unsigned int j = 0; j < np; j++)
        {
          m_TimeStepsWithin[i][j] = 1.0;
          m_TimeStepsBetween[i][j] = 1.0;
        }
      }


    }
  }

  template <class TGradientNumericType, unsigned int VDimension>
  void ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
    ::StartAdaptiveGaussSeidelOptimization()
  {
    /// uncomment this to run single threaded
    //tbb::task_scheduler_init init(1);
    std::cout << " Inside original Optimization function " << std::endl;
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
      tbb::parallel_for(
        tbb::blocked_range<size_t>{0, numdomains},
        [&](const tbb::blocked_range<size_t>& r) {
          for (size_t dom = r.begin(); dom < r.end(); ++dom) {

          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == true)
          {
            // note that this is really a 'continue' statement for the loop, but using TBB,
            // we are in an anonymous function, not a loop, so return is equivalent to continue here
            return;
          }

          const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

          typename GradientFunctionType::Pointer localGradientFunction = m_GradientFunction;

          // must clone this as we are in a thread and the gradient function is not thread-safe
          localGradientFunction = m_GradientFunction->Clone();

          // Tell function which domain we are working on.
          localGradientFunction->SetDomainNumber(dom);

          // Iterate over each particle position
          for (auto k=0; k<m_ParticleSystem->GetPositions(dom)->GetSize(); k++)
          {
            if (m_TimeSteps[dom][k] < minimumTimeStep) {
              m_TimeSteps[dom][k] = minimumTimeStep;
            }
            // Compute gradient update.
            double energy = 0.0;
            localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);
            // maximumUpdateAllowed is set based on some fraction of the distance between particles
            // This is to avoid particles shooting past their neighbors
            double maximumUpdateAllowed;
            VectorType original_gradient = localGradientFunction->Evaluate(k, dom, m_ParticleSystem, maximumUpdateAllowed, energy);

            PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

            // Step 1 Project the gradient vector onto the tangent plane
            VectorType original_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_gradient, pt, k);

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
              }
              else
              {// bad move, reset point position and back off on timestep
                if (m_TimeSteps[dom][k] > minimumTimeStep)
                {
                  domain->ApplyConstraints(pt, k);
                  m_ParticleSystem->SetPosition(pt, k, dom);
                  domain->InvalidateParticlePosition(k);

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


//   template <class TGradientNumericType, unsigned int VDimension>
//   void ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
//     ::StartMlpcaBasedAdaptiveGaussSeidelOptimization(bool flag_b)
//   {
//     /// uncomment this to run single threaded
//     // tbb::task_scheduler_init init(1);
//     std::cout << " Inside New MLPCA Optimization function " << std::endl;

//     if (this->m_AbortProcessing) {
//       return;
//     }
//     const double factor = 1.1;
//     // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
//     // ADDED TO THE SYSTEM DURING OPTIMIZATION.
//     m_StopOptimization = false;
//     if (m_NumberOfIterations >= m_MaximumNumberOfIterations) {
//       m_StopOptimization = true;
//     }
//     //m_GradientFunction->SetParticleSystem(m_ParticleSystem);

//     typedef typename DomainType::VnlVectorType NormalType;

//     ResetTimeStepVectors();
//     double minimumTimeStep = 1.0;

//     const double pi = std::acos(-1.0);
//     unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();

//     unsigned int counter = 0;

//     double maxchange_within = 0.0;
//     double maxchange_between = 0.0;
//     while (m_StopOptimization == false) // iterations loop
//     {

//       double dampening = 1;
//       int startDampening = m_MaximumNumberOfIterations / 2;
//       if (m_NumberOfIterations > startDampening) {
//         dampening = exp(-double(m_NumberOfIterations - startDampening) * 5.0 / double(m_MaximumNumberOfIterations - startDampening));
//       }
//       minimumTimeStep = dampening;

//       maxchange_within = 0.0;
//       maxchange_between = 0.0;

//       const auto accTimerBegin = std::chrono::steady_clock::now();
//       m_GradientFunction->SetParticleSystem(m_ParticleSystem);
//         if (counter % global_iteration == 0)
//             m_GradientFunction->BeforeIteration(); // TODO: Compute the within and between covariance matrices before each iteration
//         counter++;

//         // Iterate over each domain
//       tbb::parallel_for(
//         tbb::blocked_range<size_t>{0, numdomains},
//         [&](const tbb::blocked_range<size_t>& r) {
//           for (size_t dom = r.begin(); dom < r.end(); ++dom) {

//           // skip any flagged domains
//           if (m_ParticleSystem->GetDomainFlag(dom) == true)
//           {
//             // note that this is really a 'continue' statement for the loop, but using TBB,
//             // we are in an anonymous function, not a loop, so return is equivalent to continue here
//             return;
//           }

//           const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

//           typename GradientFunctionType::Pointer localGradientFunction_ = m_GradientFunction;

//           // must clone this as we are in a thread and the gradient function is not thread-safe
//           localGradientFunction_ = m_GradientFunction->Clone();
//           // typename DualGradientFunctionType::Pointer localGradientFunction = dynamic_cast<typename DualGradientFunctionType::Pointer> (localGradientFunction_.GetPointer());

//           itk::ParticleDualVectorFunction<VDimension>* localGradientFunction = dynamic_cast <itk::ParticleDualVectorFunction<VDimension>*> (localGradientFunction_.GetPointer());


//           // Tell function which domain we are working on.
//           localGradientFunction->SetDomainNumber(dom);

//           // Iterate over each particle position
//           for (auto k=0; k<m_ParticleSystem->GetPositions(dom)->GetSize(); k++)
//           {
//             if (m_TimeSteps[dom][k] < minimumTimeStep) {
//               m_TimeSteps[dom][k] = minimumTimeStep;
//             }
//             std::cout << "------optimizing within now-----" << std::endl;
//             // Compute gradient update.
//             double within_energy = 0.0;
//             localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);

//             std::cout << "Before Evaluated done" << std::endl;

//             // maximumUpdateAllowed is set based on some fraction of the distance between particles
//             // This is to avoid particles shooting past their neighbors
//             double maximumUpdateAllowed_within;
//             // VectorType original_gradient = localGradientFunction->Evaluate(k, dom, m_ParticleSystem, maximumUpdateAllowed, energy);
//             VectorType original_within_gradient = localGradientFunction->EvaluateWithin(k, dom, m_ParticleSystem, maximumUpdateAllowed_within, within_energy);
//             std::cout << " Orig Within grad done " << std::endl;

//             PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

//             // Step 1 Project the gradient vector onto the tangent plane
//             VectorType original_within_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_within_gradient, pt, k);
//             std::cout << "Orig within projection done " << std::endl;
//             // Update with within gradient
//             double newenergy_within, within_gradmag;
//             while (true) {
//               // Step A scale the projected gradient by the current time step
//               VectorType within_gradient = original_within_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k]; // TODO: check time steps proper usage
//               std::cout << "Within new gradient done " << std::endl;

//               // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
//               if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
//                 AugmentedLagrangianConstraints(within_gradient, pt, dom, maximumUpdateAllowed_within);
//               }

//               within_gradmag = within_gradient.magnitude();

//               // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
//               if (within_gradmag > maximumUpdateAllowed_within) {
//                 within_gradient = within_gradient * maximumUpdateAllowed_within / within_gradmag;
//                 within_gradmag = within_gradient.magnitude();
//                 std::cout << " within grad mag value changed" << std::endl;

//               }

//               // Step D compute the new point position
//               PointType newpoint_after_within_update = domain->UpdateParticlePosition(pt, k, within_gradient);
//               std::cout << " New point after within update done" << std::endl;

//               // Step F update the point position in the particle system
//               m_ParticleSystem->SetPosition(newpoint_after_within_update, k, dom);
//               std::cout << " Position set" << std::endl;

//               // Step G compute the new energy of the particle system
//               newenergy_within = localGradientFunction->EnergyWithin(k, dom, m_ParticleSystem);
//               std::cout << " Updated WITHIN ENERGY _____ " << std::endl;


//               if (newenergy_within < within_energy) // good move, increase timestep for next time
//               {
//                 m_TimeSteps[dom][k] *= factor;
//                 if (within_gradmag > maxchange_within) maxchange_within = within_gradmag;
//                 break;
//               }
//               else
//               {// bad move, reset point position and back off on timestep
//                 if (m_TimeSteps[dom][k] > minimumTimeStep)
//                 {
//                   domain->ApplyConstraints(pt, k);
//                   m_ParticleSystem->SetPosition(pt, k, dom);
//                   domain->InvalidateParticlePosition(k);

//                   m_TimeSteps[dom][k] /= factor;
//                 }
//                 else // keep the move with timestep 1.0 anyway
//                 {
//                   if (within_gradmag > maxchange_within) maxchange_within = within_gradmag;
//                   break;
//                 }
//               }
//             } // end while(true)
//               std::cout << " Within while done" << std::endl;



//             // 2. Optimize between part
//             if(flag_b){
//               // Ensure this check to avoid repeating the surface energy calculation when correspondence is turned off
//               std::cout << "------optimizing between now-----" << std::endl;
//               double between_energy = 0.0;
//               // localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);
//               double maximumUpdateAllowed_between;
//               VectorType original_between_gradient = localGradientFunction->EvaluateBetween(k, dom, m_ParticleSystem, maximumUpdateAllowed_between, between_energy);
//               std::cout << "B1" << std::endl;
//               PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);
//               std::cout << "B2" << std::endl;

//               // Step 1 Project the gradient vector onto the tangent plane
//               VectorType original_between_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_between_gradient, pt, k);
//               // Update with within gradient
//               double newenergy_between, between_gradmag;
//               while (true) {
//               // Step A scale the projected gradient by the current time step
//               VectorType between_gradient = original_between_gradient_projectedOntoTangentSpace * m_TimeSteps[dom][k];

//               // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
//               if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
//                 AugmentedLagrangianConstraints(between_gradient, pt, dom, maximumUpdateAllowed_between);
//               }

//               between_gradmag = between_gradient.magnitude();
//               std::cout << "B3" << std::endl;


//               // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
//               if (between_gradmag > maximumUpdateAllowed_between) {
//                 between_gradient = between_gradient * maximumUpdateAllowed_between / between_gradmag;
//                 between_gradmag = between_gradient.magnitude();
//               }

//               // Step D compute the new point position
//               PointType newpoint_after_between_update = domain->UpdateParticlePosition(pt, k, between_gradient);

//               // Step F update the point position in the particle system
//               m_ParticleSystem->SetPosition(newpoint_after_between_update, k, dom);

//               // Step G compute the new energy of the particle system
//               newenergy_between = localGradientFunction->EnergyBetween(k, dom, m_ParticleSystem);
//               std::cout << "B5" << std::endl;


//               if (newenergy_between < between_energy) // good move, increase timestep for next time
//               {
//                 m_TimeSteps[dom][k] *= factor;
//                 if (between_gradmag > maxchange_between) maxchange_between = between_gradmag;
//                 break;
//               }
//               else
//               {// bad move, reset point position and back off on timestep
//                 if (m_TimeSteps[dom][k] > minimumTimeStep)
//                 {
//                   domain->ApplyConstraints(pt, k);
//                   m_ParticleSystem->SetPosition(pt, k, dom);
//                   domain->InvalidateParticlePosition(k);

//                   m_TimeSteps[dom][k] /= factor;
//                 }
//                 else // keep the move with timestep 1.0 anyway
//                 {
//                   if (between_gradmag > maxchange_between) maxchange_between = between_gradmag;
//                   break;
//                 }
//               }
//             } // end while(true)

//             } 
//             // end between optimize
//             std::cout << "O1" << std::endl;

//           } // for each particle
//         }// for each domain
//       });

//       m_NumberOfIterations++;
//       m_GradientFunction->AfterIteration();

//       const auto accTimerEnd = std::chrono::steady_clock::now();
//       const auto msElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(accTimerEnd - accTimerBegin).count();

//       if (m_verbosity > 2)
//       {
//         std::cout << m_NumberOfIterations << ". " << msElapsed << "ms";
// #ifdef LOG_MEMORY_USAGE
//         double vmUsage, residentSet;
//         process_mem_usage(vmUsage, residentSet);
//         std::cout << " | Mem=" << residentSet << "KB";
// #endif
//         std::cout << std::endl;
//       }
//       else if (m_verbosity > 1) {
//         if (m_NumberOfIterations % (m_MaximumNumberOfIterations / 10) == 0) {
//           std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange_within = " << maxchange_within << "maxchange_between "<< maxchange_between << ", minimumTimeStep = " << minimumTimeStep << std::endl;
//         }
//       }

//       this->InvokeEvent(itk::IterationEvent());

//       // Check for convergence.  Optimization is considered to have converged if
//       // max number of iterations is reached or maximum distance moved by any
//       // particle is less than the specified precision.
//       if (maxchange_within + maxchange_between < m_Tolerance) { //TODO:ensure proper usage here
//         std::cerr << "Iteration " << m_NumberOfIterations << " maxchange_within = " << maxchange_within << "maxchange_between "<< maxchange_between << std::endl;
//         m_StopOptimization = true;
//       }

//       if (m_NumberOfIterations >= m_MaximumNumberOfIterations) {
//         m_StopOptimization = true;
//       }

//     } // end while stop optimization
//   }


  template <class TGradientNumericType, unsigned int VDimension>
  void ParticleGradientDescentPositionOptimizer<TGradientNumericType, VDimension>
    ::StartMlpcaBasedAdaptiveGaussSeidelOptimization(bool flag_b)
  {
    //Updated GD Optimizer
    /// uncomment this to run single threaded
    // tbb::task_scheduler_init init(1);
    std::cout << " Inside New MLPCA Optimization function " << std::endl;

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
    // double minimumTimeStep = 1.0;
    double minimumTimeStepWithin = 1.0;
    double minimumTimeStepBetween = 1.0;



    const double pi = std::acos(-1.0);
    unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();
    std::cout << "numdomains is " << numdomains << std::endl;

    unsigned int counter = 0;

    double maxchange_within = 0.0;
    double maxchange_between = 0.0;
    while (m_StopOptimization == false) // Optimization Iterations Main loop
    {//START OPTIMIZATION

      double dampening = 1;
      int startDampening = m_MaximumNumberOfIterations / 2;
      if (m_NumberOfIterations > startDampening) {
        dampening = exp(-double(m_NumberOfIterations - startDampening) * 5.0 / double(m_MaximumNumberOfIterations - startDampening));
      }
      minimumTimeStepWithin = dampening;
      minimumTimeStepBetween = dampening;

      maxchange_within = 0.0;
      maxchange_between = 0.0;

      const auto accTimerBegin = std::chrono::steady_clock::now();
      m_GradientFunction->SetParticleSystem(m_ParticleSystem);
      if (counter % global_iteration == 0)
          m_GradientFunction->BeforeIteration(); // Compute the MLPCA Params, Covariance Matrices over here
      counter++;

      // Iterate over each domain
      // 1. Optimize in Parallel for within
      tbb::parallel_for(
        tbb::blocked_range<size_t>{0, numdomains},
        [&](const tbb::blocked_range<size_t>& r) {
          for (size_t dom = r.begin(); dom < r.end(); ++dom) {

          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == true)
          {
            // note that this is really a 'continue' statement for the loop, but using TBB,
            // we are in an anonymous function, not a loop, so return is equivalent to continue here
            return;
          }

          const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

          typename GradientFunctionType::Pointer localGradientFunction_ = m_GradientFunction;

          // must clone this as we are in a thread and the gradient function is not thread-safe
          localGradientFunction_ = m_GradientFunction->Clone();
          // typename DualGradientFunctionType::Pointer localGradientFunction = dynamic_cast<typename DualGradientFunctionType::Pointer> (localGradientFunction_.GetPointer());

          itk::ParticleDualVectorFunction<VDimension>* localGradientFunction = dynamic_cast <itk::ParticleDualVectorFunction<VDimension>*> (localGradientFunction_.GetPointer());


          // Tell function which domain we are working on.
          localGradientFunction->SetDomainNumber(dom);

          // Iterate over each particle position
          for (auto k=0; k<m_ParticleSystem->GetPositions(dom)->GetSize(); k++)
          {
            if (m_TimeStepsWithin[dom][k] < minimumTimeStepWithin) {
              m_TimeStepsWithin[dom][k] = minimumTimeStepWithin;
            }
            // std::cout << "------optimizing within now-----" << std::endl;
            // Compute gradient update.
            double within_energy = 0.0;
            localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);

            // std::cout << "Before Evaluated done" << std::endl;

            // maximumUpdateAllowed is set based on some fraction of the distance between particles
            // This is to avoid particles shooting past their neighbors
            double maximumUpdateAllowed_within;
            VectorType original_within_gradient = localGradientFunction->EvaluateWithin(k, dom, m_ParticleSystem, maximumUpdateAllowed_within, within_energy);
            // std::cout << " Orig Within grad done " << std::endl;

            PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

            // Step 1 Project the gradient vector onto the tangent plane
            VectorType original_within_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_within_gradient, pt, k);
            // std::cout << "Orig within projection done " << std::endl;
            // Update with within gradient
            double newenergy_within, within_gradmag;
            while (true) {
              // Step A scale the projected gradient by the current time step
              VectorType within_gradient = original_within_gradient_projectedOntoTangentSpace * m_TimeStepsWithin[dom][k];

              // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
              if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
                AugmentedLagrangianConstraints(within_gradient, pt, dom, maximumUpdateAllowed_within);
              }

              within_gradmag = within_gradient.magnitude();

              // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
              if (within_gradmag > maximumUpdateAllowed_within) {
                within_gradient = within_gradient * maximumUpdateAllowed_within / within_gradmag;
                within_gradmag = within_gradient.magnitude();
              }

              // Step D compute the new point position
              PointType newpoint_after_within_update = domain->UpdateParticlePosition(pt, k, within_gradient);

              // Step F update the point position in the particle system
              m_ParticleSystem->SetPosition(newpoint_after_within_update, k, dom);

              // Step G compute the new energy of the particle system
              newenergy_within = localGradientFunction->EnergyWithin(k, dom, m_ParticleSystem);


              if (newenergy_within < within_energy) // good move, increase timestep for next time
              {
                m_TimeStepsWithin[dom][k] *= factor;
                if (within_gradmag > maxchange_within) maxchange_within = within_gradmag;
                break;
              }
              else
              {// bad move, reset point position and back off on timestep
                if (m_TimeStepsWithin[dom][k] > minimumTimeStepWithin)
                {
                  domain->ApplyConstraints(pt, k);
                  m_ParticleSystem->SetPosition(pt, k, dom);
                  domain->InvalidateParticlePosition(k);

                  m_TimeStepsWithin[dom][k] /= factor;
                }
                else // keep the move with timestep 1.0 anyway
                {
                  if (within_gradmag > maxchange_within) maxchange_within = within_gradmag;
                  break;
                }
              }
            } // end while(true)
          } // for each particle
        }// for each domain
      });//end within optimization

      // 2. Optimize in Parallel for Between
      if(flag_b){
      m_GradientFunction->BeforeIteration(); // Update the MLPCA params, covariance matrices to reflect within part
      tbb::parallel_for(
        tbb::blocked_range<size_t>{0, numdomains},
        [&](const tbb::blocked_range<size_t>& r) {
          for (size_t dom = r.begin(); dom < r.end(); ++dom) {

          // skip any flagged domains
          if (m_ParticleSystem->GetDomainFlag(dom) == true)
          {
            return;
          }

          const ParticleDomain *domain = m_ParticleSystem->GetDomain(dom);

          typename GradientFunctionType::Pointer localGradientFunction_ = m_GradientFunction;
          localGradientFunction_ = m_GradientFunction->Clone();
          itk::ParticleDualVectorFunction<VDimension>* localGradientFunction = dynamic_cast <itk::ParticleDualVectorFunction<VDimension>*> (localGradientFunction_.GetPointer());

          // Tell function which domain we are working on.
          localGradientFunction->SetDomainNumber(dom);

          // Iterate over each particle position
          for (auto k=0; k<m_ParticleSystem->GetPositions(dom)->GetSize(); k++)
          {
            if (m_TimeStepsBetween[dom][k] < minimumTimeStepBetween) {
              m_TimeStepsBetween[dom][k] = minimumTimeStepBetween;
            }
            // std::cout << "------optimizing BETWEEN now-----" << std::endl;
            // Compute gradient update.
            localGradientFunction->BeforeEvaluate(k, dom, m_ParticleSystem);

            {
              // std::cout << "------optimizing between now-----" << std::endl;
              double between_energy = 0.0;
              double maximumUpdateAllowed_between;
              VectorType original_between_gradient = localGradientFunction->EvaluateBetween(k, dom, m_ParticleSystem, maximumUpdateAllowed_between, between_energy);
              PointType pt = m_ParticleSystem->GetPositions(dom)->Get(k);

              // Step 1 Project the gradient vector onto the tangent plane
              VectorType original_between_gradient_projectedOntoTangentSpace = domain->ProjectVectorToSurfaceTangent(original_between_gradient, pt, k);
              // Update with within gradient
              double newenergy_between, between_gradmag;
              while (true) {
              // Step A scale the projected gradient by the current time step
              VectorType between_gradient = original_between_gradient_projectedOntoTangentSpace * m_TimeStepsBetween[dom][k];

              // Step B Constrain the gradient so that the resulting position will not violate any domain constraints
              if (m_ParticleSystem->GetDomain(dom)->GetConstraints()->GetActive()) {
                AugmentedLagrangianConstraints(between_gradient, pt, dom, maximumUpdateAllowed_between);
              }

              between_gradmag = between_gradient.magnitude();


              // Step C if the magnitude is larger than the Sampler allows, scale the gradient down to an acceptable magnitude
              if (between_gradmag > maximumUpdateAllowed_between) {
                between_gradient = between_gradient * maximumUpdateAllowed_between / between_gradmag;
                between_gradmag = between_gradient.magnitude();
              }

              // Step D compute the new point position
              PointType newpoint_after_between_update = domain->UpdateParticlePosition(pt, k, between_gradient);

              // Step F update the point position in the particle system
              m_ParticleSystem->SetPosition(newpoint_after_between_update, k, dom);

              // Step G compute the new Between energy of the particle system
              newenergy_between = localGradientFunction->EnergyBetween(k, dom, m_ParticleSystem);


              if (newenergy_between < between_energy) // good move, increase timestep for next time
              {
                m_TimeStepsBetween[dom][k] *= factor;
                if (between_gradmag > maxchange_between) maxchange_between = between_gradmag;
                break;
              }
              else
              {// bad move, reset point position and back off on timestep
                if (m_TimeStepsBetween[dom][k] > minimumTimeStepBetween)
                {
                  domain->ApplyConstraints(pt, k);
                  m_ParticleSystem->SetPosition(pt, k, dom);
                  domain->InvalidateParticlePosition(k);

                  m_TimeStepsBetween[dom][k] /= factor;
                }
                else // keep the move with timestep 1.0 anyway
                {
                  if (between_gradmag > maxchange_between) maxchange_between = between_gradmag;
                  break;
                }
              }
            } // end while(true)
            } 
            // end between optimize
          } // for each particle
        }// for each domain
      });}//end Between optimization

      m_NumberOfIterations++;
      m_GradientFunction->AfterIteration(); // recompute params 

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
          std::cerr << "Iteration " << m_NumberOfIterations << ", maxchange_within = " << maxchange_within << "maxchange_between "<< maxchange_between << ", minimumTimeStepWithin = " << minimumTimeStepWithin << "minimumTimeStepBetween = " << minimumTimeStepBetween << std::endl;
        }
      }

      this->InvokeEvent(itk::IterationEvent());

      // Check for convergence.  Optimization is considered to have converged if
      // max number of iterations is reached or maximum distance moved by any
      // particle is less than the specified precision.
      if (maxchange_within + maxchange_between < m_Tolerance) { //TODO:ensure proper usage of tolerance here, and how does it affect the method?
        std::cerr << "Iteration " << m_NumberOfIterations << " maxchange_within = " << maxchange_within << "maxchange_between "<< maxchange_between << std::endl;
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

} // end namespace

#endif
