/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleSystem.txx,v $
  Date:      $Date: 2011/03/24 01:17:34 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleSystem_txx
#define __itkParticleSystem_txx

#include "itkCommand.h"
#include "itkParticleImageDomainWithGradients.h"
namespace itk
{

template <unsigned int VDimension>
ParticleSystem<VDimension>
::ParticleSystem()
{
}

template <unsigned int VDimension>
void
ParticleSystem<VDimension>
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
  while (num >= m_PrefixTransforms.size()) {
    TransformType transform;
    transform.set_identity();
    m_PrefixTransforms.push_back(transform);
    m_InversePrefixTransforms.push_back(transform);
  }
  m_Positions.resize(num);
  m_IndexCounters.resize(num);
  m_Neighborhoods.resize(num);
  while(num >= this->m_DomainFlags.size()) {
    m_DomainFlags.push_back(false);
  }
  this->Modified();
}

template <unsigned int VDimension>
void ParticleSystem<VDimension>
::AddDomain( DomainType *input, int threadId)
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

  // Notify any observers.
  ParticleDomainAddEvent e;
  e.SetDomainIndex(m_Domains.size() - 1);
  e.SetPositionIndex(0);
  e.SetThreadID(threadId);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void ParticleSystem<VDimension>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "m_IndexCounters.size(): " << m_IndexCounters.size() << std::endl;
  os << indent << "m_Positions.size() : " << m_Positions.size() << std::endl;
  os << indent << "m_Domains.size() : " << m_Domains.size() << std::endl;
}

template <unsigned int VDimension>
void ParticleSystem<VDimension>
::SetTransform(unsigned int i, const TransformType& T, int threadId)
{
  if (i > static_cast<int>(m_Transforms.size())-1 || m_Transforms.empty()) {
    m_Transforms.resize(i+1);
    m_InverseTransforms.resize(i+1);
  }
  m_Transforms[i] = T;
  m_InverseTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticleTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void ParticleSystem<VDimension>
::SetPrefixTransform(unsigned int i, const TransformType& T, int threadId)
{
  if (i > static_cast<int>(m_PrefixTransforms.size())-1 || m_PrefixTransforms.empty()) {
    m_PrefixTransforms.resize(i+1);
    m_InversePrefixTransforms.resize(i+1);
  }

  m_PrefixTransforms[i] = T;
  m_InversePrefixTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticlePrefixTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

template <unsigned int VDimension>
void ParticleSystem<VDimension>
::SetNeighborhood(unsigned int i, NeighborhoodType* N, int threadId)
{
  if (m_DomainFlags[i] == true)
      return;
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
const typename ParticleSystem<VDimension>::PointType &
ParticleSystem<VDimension>
::AddPosition( const PointType &p, unsigned int d, int threadId)
{
  m_Positions[d]->operator[](m_IndexCounters[d]) = p;

  // Potentially modifies position!
  if (m_DomainFlags[d] == false) {
      // debugg
      //std::cout << "d" << d << " before apply " << m_Positions[d]->operator[](m_IndexCounters[d]);
    const auto idx = m_IndexCounters[d];
    m_Domains[d]->ApplyConstraints( m_Positions[d]->operator[](idx), idx);
      // debugg
      //std::cout << " after apply " << m_Positions[d]->operator[](m_IndexCounters[d]) << std::endl;
    m_Neighborhoods[d]->AddPosition( m_Positions[d]->operator[](idx), idx, threadId);
  }

  // Increase the FixedParticleFlag list size if necessary.
  if (m_IndexCounters[d] >= m_FixedParticleFlags[d % m_DomainsPerShape].size())
  {
    m_FixedParticleFlags[d % m_DomainsPerShape].push_back(false);
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
const typename ParticleSystem<VDimension>::PointType &
ParticleSystem<VDimension>
::SetPosition(const PointType &p,  unsigned long int k,
                                        unsigned int d,  int threadId)
{
  if (m_FixedParticleFlags[d % m_DomainsPerShape][k] == false)
  {
    // Potentially modifies position!
    if (m_DomainFlags[d] == false) {
      m_Positions[d]->operator[](k) = p;

      // Debuggg
      //std::cout << "SynchronizePositions Apply constraints " << m_Positions[d]->operator[](k);
      m_Domains[d]->ApplyConstraints( m_Positions[d]->operator[](k), k);
      // Debuggg
      //std::cout << " updated " << m_Positions[d]->operator[](k) << std::endl;

      m_Neighborhoods[d]->SetPosition( m_Positions[d]->operator[](k), k, threadId);
    }

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
ParticleSystem<VDimension>
::RemovePosition(unsigned long int k,
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
ParticleSystem<VDimension>
::AddPositionList(const std::vector<PointType> &p,
                                            unsigned int d, int threadId )
{
  // Traverse the list and add each point to the domain.
  for (typename std::vector<PointType>::const_iterator it= p.begin(); it != p.end(); it++) {
    this->AddPosition(*it, d, threadId);
  }
}

template <unsigned int VDimension>
void
ParticleSystem<VDimension>
::PrintParticleSystem(){

    for (unsigned int d = 0; d < this->GetNumberOfDomains(); d++)
      {
        std::vector<PointType> list;
      for (unsigned int p = 0; p < this->GetNumberOfParticles(d); p++)
        {
        list.push_back(this->GetPosition(p,d));
        }
          std::cout << "D " << d << " Curr Pos ";
          for(size_t i = 0; i < list.size(); i++)
              std::cout << list[i] << " ";
          std::cout << " List size " << list.size() << std::endl;
      }

    /*
    size_t num_doms = this->GetNumberOfDomains();

    for(size_t domain = 0; domain < num_doms; domain++){
        std::vector<PointType> list;
        typename PointContainerType::ConstIterator endIt = GetPositions(domain)->GetEnd();
        for (typename PointContainerType::ConstIterator it = GetPositions(domain)->GetBegin();
             it != endIt; it++)
          {    list.push_back(*it);    }
        std::cout << "D " << domain << " Curr Pos ";
        for(size_t i = 0; i < list.size(); i++)
            std::cout << list[i] << " ";
        std::cout << " List size " << list.size() << std::endl;
    }
    */
}

template<unsigned int VDimension>
void ParticleSystem<VDimension>::AdvancedAllParticleSplitting(double epsilon,
                                                              unsigned int domains_per_shape,
                                                              unsigned int dom_to_process)
{
  size_t num_doms = this->GetNumberOfDomains();

  for (size_t domain = 0; domain < num_doms; domain++) {
      this->GetDomain(domain)->GetConstraints()->InitializeLagrangianParameters(1,1,1);
  }

  std::vector<std::vector<PointType> > lists;

  for (size_t domain = dom_to_process; domain < num_doms; domain += domains_per_shape) {
    std::vector<PointType> list;

    for (auto k = 0; k < GetPositions(domain)->GetSize(); k++) {
      list.push_back(GetPositions(domain)->Get(k));
    }
    lists.push_back(list);

  }

  if (lists.size() > 0) {
    for (size_t i = 0; i < lists[0].size(); i++) {
      // While the random vector updated violates plane constraints
      // Breaks when it doesn't violate for any domain
      std::vector<PointType> newposs_good;

      while (true) {
        // Generate random unit vector
        std::uniform_real_distribution<double> distribution(-1000., 1000.);

        vnl_vector_fixed<double, 3> random;

        for (int i = 0; i < 3; i++) {
          random[i] = distribution(this->m_rand);
        }
        double norm = random.magnitude();
        random /= norm;

        // Check where the update will take us after applying it to the point and th constraints.
        newposs_good.clear();
        bool good = true; // flag to check if the new update violates in any domain
        for (size_t j = 0; j < lists.size(); j++) {
          // Add epsilon times random direction to existing point and apply domain
          // constraints to generate a new particle position.
          PointType newpos = this->GetDomain(dom_to_process+j*domains_per_shape)->GetPositionAfterSplit(lists[j][i], random, epsilon);
          // Go to surface
          if (!this->m_DomainFlags[dom_to_process+j*domains_per_shape] &&
              !this->GetDomain(dom_to_process+j*domains_per_shape)->GetConstraints()->IsAnyViolated(newpos)) {
            this->GetDomain(dom_to_process+j*domains_per_shape)->ApplyConstraints(newpos, -1);
          }
          newposs_good.push_back(newpos);
          // Check for plane constraint violations

          /*if (this->GetDomain(dom_to_process+j*domains_per_shape)->GetConstraints()->IsAnyViolated(newpos)) {
            good = false;
            //std::cout << "violation " << lists[j][i] << " new point " << std::endl;
            break;
          }*/
        }

        if (good) {
          for (size_t j = 0; j < lists.size(); j++) {
            this->AddPosition(newposs_good[j], dom_to_process+j*domains_per_shape, 0);
            // Debuggg
            //std::cout << "Domain " << j << " Curr Pos " << lists[j][i] << " random "
            // << random  << " epsilon " << epsilon << " picked " << newposs_good[j] << std::endl;
          }
          break;
        }

      }// while end
    } // for end
  } // if end
}

template <unsigned int VDimension>
void
ParticleSystem<VDimension>
::SplitAllParticlesInDomain(const vnl_vector_fixed<double, VDimension> &random, unsigned int domain, int threadId)
{
  // Loop through all particle positions in the domain and add a new position
  // at an epsilon distance and random direction. Since we are going to add
  // positions to the list, we need to first copy the list.
  std::vector<PointType> list;
  typename PointContainerType::ConstIterator endIt = GetPositions(domain)->GetEnd();
  for (typename PointContainerType::ConstIterator it = GetPositions(domain)->GetBegin();
       it != endIt; it++)
    {    list.push_back(*it);    }

  int k = 0;
  for (typename std::vector<PointType>::const_iterator it = list.begin(); it != list.end(); it++, k++) {
    // Add epsilon times random direction to existing point and apply domain
    // constraints to generate a new particle position.  Add the new position.
    PointType startingPos = *it;

    vnl_vector_fixed<double, VDimension> updateVector = random * 0.5;
    vnl_vector_fixed<double, VDimension> projected = this->GetDomain(domain)->ProjectVectorToSurfaceTangent(updateVector, startingPos);

    projected = projected * updateVector.magnitude() / projected.magnitude();


    PointType newpos = this->GetDomain(domain)->UpdateParticlePosition(startingPos, projected);
    this->AddPosition(newpos, domain, threadId);

    // Apply opposite update to each original point in the split.
    auto neg_projected = -projected;
    newpos = this->GetDomain(domain)->UpdateParticlePosition(startingPos, neg_projected);
    this->SetPosition(newpos, k, domain, threadId);
    const ParticleDomain *particle_domain = this->GetDomain(domain);
    particle_domain->InvalidateParticlePosition(k);

  }
}

template <unsigned int VDimension>
void
ParticleSystem<VDimension>
::RegisterAttribute( ParticleAttribute<VDimension> *attr)
{
  // Register any methods defined by the attribute as observers of this
  // ParticleSystem with appropriate events.
  if (attr->m_DefinedCallbacks.Event == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::EventCallback);
    this->AddObserver(ParticleEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.EventWithIndex == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::EventWithIndexCallback);
    this->AddObserver(ParticleEventWithIndex(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.DomainAddEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::DomainAddEventCallback);
    this->AddObserver(ParticleDomainAddEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.TransformSetEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::TransformSetEventCallback);
    this->AddObserver(ParticleTransformSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PrefixTransformSetEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::PrefixTransformSetEventCallback);
    this->AddObserver(ParticlePrefixTransformSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.NeighborhoodSetEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::NeighborhoodSetEventCallback);
    this->AddObserver(ParticleNeighborhoodSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionSetEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::PositionSetEventCallback);
    this->AddObserver(ParticlePositionSetEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionAddEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::PositionAddEventCallback);
    this->AddObserver(ParticlePositionAddEvent(), tmpcmd);
    }
  if (attr->m_DefinedCallbacks.PositionRemoveEvent == true)
    {
    typename MemberCommand< ParticleAttribute<VDimension> >::Pointer tmpcmd
      = MemberCommand< ParticleAttribute<VDimension> >::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<VDimension>::PositionRemoveEventCallback);
    this->AddObserver(ParticlePositionRemoveEvent(), tmpcmd);
    }
}

} // end namespace

#endif //__itkParticleSystem_txx
