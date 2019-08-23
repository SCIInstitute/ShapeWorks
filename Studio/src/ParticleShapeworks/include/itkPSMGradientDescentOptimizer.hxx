/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMGradientDescentOptimizer_hxx
#define __itkPSMGradientDescentOptimizer_hxx
#include "itkPSMGradientDescentOptimizer.h"

namespace itk
{
template <class TGradientNumericType, unsigned int VDimension>
PSMGradientDescentOptimizer<TGradientNumericType, VDimension>
::PSMGradientDescentOptimizer()
{
  m_StopOptimization = false;
  m_NumberOfIterations = 0;
  m_MaximumNumberOfIterations = 0;
  m_Tolerance = 1.0e-8;
  m_TimeStep = 1.0;
  m_OptimizationMode = 0;
} 

/*** ADPATIVE GAUSS SEIDEL ***/
template <class TGradientNumericType, unsigned int VDimension>
void
PSMGradientDescentOptimizer<TGradientNumericType, VDimension>
::StartAdaptiveGaussSeidelOptimization()
{
    std::cout << "USING ADAPTIVE TIME STEP !!!!" << std::endl << std::flush;
    double factor        = 1.1;

    // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
    // ADDED TO THE SYSTEM DURING OPTIMIZATION.
    m_StopOptimization = false;
    m_NumberOfIterations = 0;
    m_CostFunction->SetParticleSystem(m_ParticleSystem);

    // Initialization of the times steps
    unsigned int numdomains = m_ParticleSystem->GetNumberOfDomains();

    // Make sure the time step vector is the right size
    // maintain a time step for each particle which will be automatically tuned along iterations
    while (m_TimeSteps.size() != numdomains )
    {
        std::vector<double> tmp;
        m_TimeSteps.push_back( tmp );
    }

    // Initialize the time step of each particle per domain
    for (unsigned int dom = 0; dom < numdomains; dom++)
    {
        unsigned int np = m_ParticleSystem->GetPositions(dom)->GetSize();

        if (m_TimeSteps[dom].size() != np)
        {
            // resize and initialize everything to 1.0
            m_TimeSteps[dom].resize(np);
            for (unsigned int idx = 0; idx < np; idx++)
            {
                m_TimeSteps[dom][idx] = 1.0;
            }
        }
    }

    // maintain mean, max and min time step per domain (over all its particles)
    std::vector<double> meantime(numdomains);
    std::vector<double> maxtime(numdomains);
    std::vector<double> mintime(numdomains);

    // initialize mean, max and min time step per domain
    for (unsigned int dom = 0; dom < numdomains; dom++)
    {
        meantime[dom] = 0.0;
        maxtime[dom]  = 1.0e30;
        mintime[dom]  = 1.0;
    }

    VectorType original_gradient;
    VectorType gradient;
    PointType  newpoint;

    while (m_StopOptimization == false)
    {
        m_CostFunction->BeforeIteration();
        double maxdt;
        double movement = 0.0f;

        // Iterate over each domain
        for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
        {
            // initialize the mean time step for the current domain
            meantime[dom] = 0.0;

            // skip any flagged domains
            if (m_ParticleSystem->GetDomainFlag(dom) == false)
            {
                // Tell function which domain we are working on.
                m_CostFunction->SetDomainNumber(dom);

                // Iterate over each particle position
                unsigned int k    = 0;   // particle index

                typename ParticleSystemType::PointContainerType::ConstIterator endit =
                        m_ParticleSystem->GetPositions(dom)->GetEnd();

                for (typename ParticleSystemType::PointContainerType::ConstIterator it
                     = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
                {
                    bool done = false;

                    // Compute gradient update.
                    double energy = 0.0;

                    m_CostFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
                    original_gradient = m_CostFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem,
                                                                 maxdt, energy);

                    PointType pt = *it; // the current particle

                    double newenergy, gradmag;
                    double cur_movement = 0.0f;

                    while ( !done )
                    {
                        // compute the current gradient based on the current time step
                        gradient = original_gradient * m_TimeSteps[dom][k];

                        // May modify gradient and maxmove
                        dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))
                                ->ApplyVectorConstraints(gradient,
                                                         m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt);

                        gradmag = gradient.magnitude();

                        // Prevent a move which is too large
                        // the system will be slow if we allow particles to jump over one another
                        // so we penalize a particle with a very high energy when it attempts to move too large distance
                        // the max allowed movement by the current particle is managed by the minimum allowed move evaluated
                        // from the cost functions
                        // the surface sampling (particle entropy) is controled by the particles sigma while the correspondence part is
                        // controled by the minimum eigen value of the regularized covariance matrix
                        if (gradmag * m_TimeSteps[dom][k] > maxdt)
                        {
                            m_TimeSteps[dom][k] /= factor;
                        }
                        else // Move is not too large
                        {
                            // Make a move and compute new energy
                            for (unsigned int i = 0; i < VDimension; i++)
                            {  newpoint[i] = pt[i] - gradient[i]; }

                            // project the particle onto the zero-level (the current implicit surface)
                            dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(newpoint);

                            // update the particle system (temporarily)
                            m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);

                            // compute the new energy based on the udpated position for the current particle
                            newenergy = m_CostFunction->Energy(it.GetIndex(), dom, m_ParticleSystem);

                            // acceptance or rejection of the current move is according to its effect on the total energy
                            if (newenergy < energy) // good move, increase timestep for next time
                            {
                                // Compute mean time step (incremental way of computing the mean)
                                double prev_mean = meantime[dom];
                                meantime[dom]    = prev_mean + (m_TimeSteps[dom][k] - prev_mean)/(k+1); // k+1 is the current number of particles being visited

                                m_TimeSteps[dom][k] *= factor;

                                if (m_TimeSteps[dom][k] > maxtime[dom])
                                    m_TimeSteps[dom][k] = maxtime[dom];

                                // keep track of the current particle movement
                                cur_movement = gradmag;

                                // we are done with this particle
                                done = true;
                            }
                            else
                            {
                                // bad move, reset point position

                                dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))->ApplyConstraints(pt);

                                // reset point position
                                m_ParticleSystem->SetPosition(pt, it.GetIndex(), dom);

                                // keep track of the current particle movement
                                cur_movement = 0.0;

                                if (m_TimeSteps[dom][k] > mintime[dom])// back off on timestep
                                {
                                    m_TimeSteps[dom][k] /= factor;
                                }
                                else // can find a good move for the current particle
                                {
                                    // never accept a bad move no matter what ...
                                    done = true;
                                }
                            }
                        }

                    } // end while not done

                    if (meantime[dom] < 1.0) meantime[dom] = 1.0;
                    maxtime[dom] = meantime[dom] + meantime[dom] * 0.2;
                    mintime[dom] = meantime[dom] - meantime[dom] * 0.1;

                    // Keep the biggest particle movement to keep track of convergence
                    if ( cur_movement > movement) movement = cur_movement;

                } // for each particle
            } // if not flagged
        }// for each domain

        m_NumberOfIterations++;
        m_CostFunction->AfterIteration();
        this->InvokeEvent(itk::IterationEvent());

        // Has a maximum number of iterations been specified?
        if ((m_MaximumNumberOfIterations != 0) && (m_NumberOfIterations >= m_MaximumNumberOfIterations))
        {
            m_StopOptimization = true;
        }

        if (movement <= m_Tolerance) m_StopOptimization = true;

    } // end while

}

/*** GAUSS SEIDEL ***/
template <class TGradientNumericType, unsigned int VDimension>
void
PSMGradientDescentOptimizer<TGradientNumericType, VDimension>
::StartGaussSeidelOptimization()
{
  // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
  // ADDED TO THE SYSTEM DURING OPTIMIZATION.
  m_StopOptimization = false;
  m_NumberOfIterations = 0;
  m_CostFunction->SetParticleSystem(m_ParticleSystem);
  
  VectorType gradient;
  PointType newpoint;
  
  while (m_StopOptimization == false)
    {
    m_CostFunction->BeforeIteration();
    double maxdt;
    double movement = 0.0f;

    // Iterate over each domain
    for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
      {
      // skip any flagged domains
      if (m_ParticleSystem->GetDomainFlag(dom) == false)
        {
        // Tell function which domain we are working on.
        m_CostFunction->SetDomainNumber(dom);
        
        // Iterate over each particle position
        unsigned int k = 0;
        typename ParticleSystemType::PointContainerType::ConstIterator endit =
          m_ParticleSystem->GetPositions(dom)->GetEnd();
        for (typename ParticleSystemType::PointContainerType::ConstIterator it
               = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
          {
          // Compute gradient update.
          m_CostFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
          gradient = m_CostFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem,
                                                  maxdt);

          // May modify gradient and maxmove
          dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))
            ->ApplyVectorConstraints(gradient,
                                     m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt);

          // Hack to avoid blowing up under certain conditions.
          if (gradient.magnitude() > maxdt)
            { gradient = (gradient / gradient.magnitude()) * maxdt; }
          
          // Compute particle move based on update.
           for (unsigned int i = 0; i < VDimension; i++)
             {
             newpoint[i] = (*it)[i] - gradient[i] * m_TimeStep;
             }
           
           // Keep the biggest particle movement.
           if ( gradient.magnitude() > movement) movement = gradient.magnitude();

           // Apply update
           m_ParticleSystem->SetPosition(newpoint, it.GetIndex(), dom);
           } // for each particle
         } // if not flagged
       }// for each domain

     m_NumberOfIterations++;
     m_CostFunction->AfterIteration();
     this->InvokeEvent(itk::IterationEvent());
  
     // Has a maximum number of iterations been specified?
     if ((m_MaximumNumberOfIterations != 0) && (m_NumberOfIterations >= m_MaximumNumberOfIterations))
       {
       m_StopOptimization = true;
       }
    
     if (movement <= m_Tolerance) m_StopOptimization = true;         
  
     } // end while

 }

template <class TGradientNumericType, unsigned int VDimension>
void
PSMGradientDescentOptimizer<TGradientNumericType, VDimension>
::StartJacobiOptimization()
{
  // NOTE: THIS METHOD WILL NOT WORK AS WRITTEN IF PARTICLES ARE
  // ADDED TO THE SYSTEM DURING OPTIMIZATION.
  m_StopOptimization = false;
  m_NumberOfIterations = 0;
  m_CostFunction->SetParticleSystem(m_ParticleSystem);
  
  // Vector for storing updates.
  std::vector<std::vector<PointType> > updates(m_ParticleSystem->GetNumberOfDomains());

  for (unsigned int d = 0; d < m_ParticleSystem->GetNumberOfDomains(); d++)
    {
    updates[d].resize(m_ParticleSystem->GetPositions(d)->GetSize());
    }
  VectorType gradient;
  PointType  newpoint;

  while (m_StopOptimization == false)
    {
    m_CostFunction->BeforeIteration();
    double maxdt;
    double movement = 0.0f;
    
    // Iterate over each domain
    for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
      {
      // skip any flagged domains
      if (m_ParticleSystem->GetDomainFlag(dom) == false)
        {
        // Tell function which domain we are working on.
        m_CostFunction->SetDomainNumber(dom);
        
        // Iterate over each particle position
        unsigned int k = 0;
        typename ParticleSystemType::PointContainerType::ConstIterator endit =
          m_ParticleSystem->GetPositions(dom)->GetEnd();
        for (typename ParticleSystemType::PointContainerType::ConstIterator it
               = m_ParticleSystem->GetPositions(dom)->GetBegin(); it != endit; it++, k++)
          {
          // Compute gradient update.
          m_CostFunction->BeforeEvaluate(it.GetIndex(), dom, m_ParticleSystem);
          gradient = m_CostFunction->Evaluate(it.GetIndex(), dom, m_ParticleSystem,
                                                  maxdt);
          // May modify gradient and maxmove
          dynamic_cast<DomainType *>(m_ParticleSystem->GetDomain(dom))
            ->ApplyVectorConstraints(gradient, m_ParticleSystem->GetPosition(it.GetIndex(), dom), maxdt);
          
          // Hack to avoid blowing up under certain conditions.
          if (gradient.magnitude() > maxdt)
            {
            gradient = (gradient / gradient.magnitude()) * maxdt;
            }
          
          // Compute particle move based on update.
          for (unsigned int i = 0; i < VDimension; i++)
            {
            newpoint[i] = (*it)[i] - gradient[i] * m_TimeStep;
            }


          // Keep the biggest particle movement.
          if ( gradient.magnitude() > movement) movement = gradient.magnitude();
          
          // Store update
          updates[dom][k] = newpoint;
          } // for each particle
        } // if not flagged
      }// for each domain
    
    for (unsigned int dom = 0; dom < m_ParticleSystem->GetNumberOfDomains(); dom++)
      {
      // skip any flagged domains
      if (m_ParticleSystem->GetDomainFlag(dom) == false)
        {
        unsigned int k = 0;
        typename ParticleSystemType::PointContainerType::ConstIterator endit =
          m_ParticleSystem->GetPositions(dom)->GetEnd();
        for (typename ParticleSystemType::PointContainerType::ConstIterator it
               = m_ParticleSystem->GetPositions(dom)->GetBegin();  it != endit; it++, k++)
          {
          m_ParticleSystem->SetPosition(updates[dom][k], it.GetIndex(), dom);
          } // for each particle
        } // if not flagged
      } // for each domain
    
    m_NumberOfIterations++;
    m_CostFunction->AfterIteration();
    this->InvokeEvent(itk::IterationEvent());
    
    // Has a maximum number of iterations been specified?
    if ((m_MaximumNumberOfIterations != 0) && (m_NumberOfIterations >= m_MaximumNumberOfIterations))
      {
        m_StopOptimization = true;
      }

    // Otherwise, converge based on a tolerance.
    if (movement <= m_Tolerance) m_StopOptimization = true;         
 
    } // end while
  
}

} // end namespace

#endif
