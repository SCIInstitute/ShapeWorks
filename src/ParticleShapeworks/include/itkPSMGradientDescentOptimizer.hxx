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
          
          // May modify gradient
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
          // May modify gradient
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
