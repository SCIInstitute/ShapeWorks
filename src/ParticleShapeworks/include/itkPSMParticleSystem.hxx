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
#ifndef __itkPSMParticleSystem_hxx
#define __itkPSMParticleSystem_hxx
#include "itkPSMParticleSystem.h"

#include "itkCommand.h"

namespace itk
{

template <unsigned int VDimension>
PSMParticleSystem<VDimension>::PSMParticleSystem()
{
}

template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>
::SetNumberOfDomains(unsigned int num)
{
  // in case nothing has changed.
  if (num == m_Domains.size())
    {
    return;
    }
  m_Domains.resize(num);
  m_Transforms.resize(num);
  m_InverseTransforms.resize(num);
  m_PrefixTransforms.resize(num);
  m_InversePrefixTransforms.resize(num);
  m_Positions.resize(num);
  m_IndexCounters.resize(num);
  m_Neighborhoods.resize(num);
  m_DomainFlags.resize(num);
  this->Modified();
}

template <unsigned int VDimension>
void PSMParticleSystem<VDimension>::AddDomain( DomainType *input, int threadId)
{
  this->Modified();
  
  for (unsigned int idx = 0; idx < m_Domains.size(); ++idx)
    {
    if (!m_Domains[idx])
      {
      m_Domains[idx] = input;
      m_Positions[idx]  =  PointContainerType::New();
      m_IndexCounters[idx] = 0;
      return;
      }
    }

  this->SetNumberOfDomains(static_cast<int>(m_Domains.size() + 1));
  m_Domains[ static_cast<int>( m_Domains.size() ) - 1] = input;
  m_Positions[static_cast<int>( m_Domains.size() ) - 1] = PointContainerType::New();
  m_IndexCounters[static_cast<int>( m_Domains.size() -1)] = 0;
  m_Neighborhoods[static_cast<int>( m_Domains.size() -1)] = NeighborhoodType::New();
  m_Transforms[static_cast<int>( m_Domains.size() -1)].set_identity();
  m_InverseTransforms[static_cast<int>( m_Domains.size() -1)].set_identity();
  m_PrefixTransforms[static_cast<int>( m_Domains.size() -1)].set_identity();
  m_InversePrefixTransforms[static_cast<int>( m_Domains.size() -1)].set_identity();
  m_DomainFlags[static_cast<int>( m_Domains.size() -1)] = false;
  
  // Notify any observers.
  ParticleDomainAddEvent e;
  e.SetDomainIndex(m_Domains.size() - 1);
  e.SetPositionIndex(0);
  e.SetThreadID(threadId);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void PSMParticleSystem<VDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "m_IndexCounters.size(): " << m_IndexCounters.size() << std::endl;
  os << indent << "m_Positions.size() : " << m_Positions.size() << std::endl;
  os << indent << "m_Domains.size() : " << m_Domains.size() << std::endl;
}

template <unsigned int VDimension>
void PSMParticleSystem<VDimension>
::SetTransform(unsigned int i, const TransformType& T, int threadId)
{
  m_Transforms[i] = T;
  m_InverseTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticleTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void PSMParticleSystem<VDimension>
::SetPrefixTransform(unsigned int i, const TransformType& T, int threadId)
{
  m_PrefixTransforms[i] = T;
  m_InversePrefixTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticlePrefixTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void PSMParticleSystem<VDimension>
::SetNeighborhood(unsigned int i, NeighborhoodType* N, int threadId)
{
  m_Neighborhoods[i] = N;
  m_Neighborhoods[i]->SetDomain(m_Domains[i]);
  m_Neighborhoods[i]->SetPointContainer(m_Positions[i]);
  
  // Notify any observers.
  ParticleNeighborhoodSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
const typename PSMParticleSystem<VDimension>::PointType &
PSMParticleSystem<VDimension>::AddPosition( const PointType &p, unsigned int d, int threadId)
{
  m_Positions[d]->operator[](m_IndexCounters[d]) = p;

  // Apply constraints to the given position.  Note that the following
  // operation potentially modifes that position.
  m_Domains[d]->ApplyConstraints( m_Positions[d]->operator[](m_IndexCounters[d]));
  
  m_Neighborhoods[d]->AddPosition( m_Positions[d]->operator[](m_IndexCounters[d]),
                                   m_IndexCounters[d], threadId);
  
  // Increase the FixedParticleFlag list size if necessary.
  if (m_IndexCounters[0] >= m_FixedParticleFlags.size())
    {
      m_FixedParticleFlags.push_back(false);
    }
  
  // Notify any observers.
  ParticlePositionAddEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(d);
  e.SetPositionIndex(m_IndexCounters[d]);
  this->InvokeEvent(e);
  m_IndexCounters[d]++;
 
  return m_Positions[d]->operator[](m_IndexCounters[d]-1);
}

template <unsigned int VDimension>
const typename PSMParticleSystem<VDimension>::PointType &
PSMParticleSystem<VDimension>::SetPosition(const PointType &p,  unsigned long int k,
                                        unsigned int d,  int threadId)
{
  if (m_FixedParticleFlags[k] == false)
    {
      m_Positions[d]->operator[](k) = p;
      
      // Apply constraints to the given position.  Note that the following
      // operation potentially modifes that position.
      m_Domains[d]->ApplyConstraints( m_Positions[d]->operator[](k));
      
      m_Neighborhoods[d]->SetPosition( m_Positions[d]->operator[](k), k,
                                       threadId);
    }
  
  // Notify any observers.
  ParticlePositionSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(d);
  e.SetPositionIndex(k);
  
  this->InvokeEvent(e);
  
  return m_Positions[d]->operator[](k);
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::RemovePosition(unsigned long int k,
                                           unsigned int d,  int threadId)
{
  m_Positions[d]->Erase(k);
  
  m_Neighborhoods[d]->RemovePosition(k, threadId);
  
  // Notify any observers.
  ParticlePositionRemoveEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(d);
  e.SetPositionIndex(k);
  
  this->InvokeEvent(e);
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::AddPositionList(const std::vector<PointType> &p,
                                            unsigned int d, int threadId )
{
  // Traverse the list and add each point to the domain.
  for (typename std::vector<PointType>::const_iterator it= p.begin();
       it != p.end(); it++)
    {
      this->AddPosition(*it, d, threadId);    
    }
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::SplitParticle(double epsilon, unsigned int idx, unsigned int domain, int threadId)
{
  // Find a random direction.
  vnl_vector_fixed<double, VDimension> random;
  
  for (unsigned int i = 0; i < VDimension; i++)
    {  random[i] = static_cast<double>(rand());  }
  double norm = random.magnitude();
  
  // Normalize the random vector.
  random /= norm;
  
  // Add epsilon times random direction to existing point and apply domain
  // constraints to generate a new particle position.  Add the new position.
  PointType newpos;
  for (unsigned int i = 0; i < VDimension; i++)
    {
      newpos[i] = this->GetPosition(idx,domain)[i] + epsilon * random[i];
    }
  this->GetDomain(domain)->ApplyConstraints(newpos);
  this->AddPosition(newpos, domain, threadId);
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::SplitAllParticlesInDomain(const vnl_vector_fixed<double, VDimension> &random,
                                                      double epsilon, unsigned int domain, int threadId)
{
  // Loop through all particle positions in the domain and add a new position
  // at an epsilon distance and random direction. Since we are going to add
  // positions to the list, we need to first copy the list.
  std::vector<PointType> list;
  typename PointContainerType::ConstIterator endIt = GetPositions(domain)->GetEnd();     
  for (typename PointContainerType::ConstIterator it = GetPositions(domain)->GetBegin();
       it != endIt; it++)
    {    list.push_back(*it);    }
  
  for (typename std::vector<PointType>::const_iterator it = list.begin();
       it != list.end(); it++)
    {
      // Add epsilon times random direction to existing point and apply domain
      // constraints to generate a new particle position.  Add the new position.
      PointType newpos;
      for (unsigned int i = 0; i < VDimension; i++)
        {
          newpos[i] = (*it)[i] + epsilon * random[i];
        }
      this->GetDomain(domain)->ApplyConstraints(newpos);
      this->AddPosition(newpos, domain, threadId);
    } // end for std::vector::iterator
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::SplitAllParticles(double epsilon, int threadId)
{
  // Find a random direction.
  vnl_vector_fixed<double, VDimension> random;
  
  for (unsigned int i = 0; i < VDimension; i++)
    {        random[i] = static_cast<double>(rand());        }
  double norm = random.magnitude();
  
  // Normalize the random vector.
  random /= norm;
  
  // Loop through all particle positions in the domain and add a new position
  // at an epsilon distance and random direction.
  for (unsigned i = 0; i < this->GetNumberOfDomains(); i++)
    {
      this->SplitAllParticlesInDomain(random, epsilon, i, threadId);
    }
}
  
template <unsigned int VDimension>
void
PSMParticleSystem<VDimension>::RegisterAttribute( PSMAttribute<VDimension> *attr)
{
  // Register any methods defined by the attribute as observers of this
  // PSMParticleSystem with appropriate events.
  if (attr->m_DefinedCallbacks.Event == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::EventCallback);
    this->AddObserver(ParticleEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.EventWithIndex == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::EventWithIndexCallback);
    this->AddObserver(ParticleEventWithIndex(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.DomainAddEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::DomainAddEventCallback);
    this->AddObserver(ParticleDomainAddEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.TransformSetEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::TransformSetEventCallback);
    this->AddObserver(ParticleTransformSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PrefixTransformSetEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::PrefixTransformSetEventCallback);
    this->AddObserver(ParticlePrefixTransformSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.NeighborhoodSetEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::NeighborhoodSetEventCallback);
    this->AddObserver(ParticleNeighborhoodSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionSetEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::PositionSetEventCallback);
    this->AddObserver(ParticlePositionSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionAddEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::PositionAddEventCallback);
    this->AddObserver(ParticlePositionAddEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionRemoveEvent == true)
    {
    typename MemberCommand< PSMAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< PSMAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &PSMAttribute<VDimension>::PositionRemoveEventCallback);
    this->AddObserver(ParticlePositionRemoveEvent(), tmpcmd);
    }
}


} // end namespace

#endif //__itkPSMParticleSystem_hxx
