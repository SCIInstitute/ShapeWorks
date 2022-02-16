#include "itkParticleSystem.h"

namespace itk {

ParticleSystem::PointType ParticleSystem::TransformPoint(const PointType &p, const TransformType &T) const {
  PointType ans;

  ans[0] = T[0][0] * p[0] + T[0][1] * p[1] + T[0][2] * p[2] + T[0][3];
  ans[1] = T[1][0] * p[0] + T[1][1] * p[1] + T[1][2] * p[2] + T[1][3];
  ans[2] = T[2][0] * p[0] + T[2][1] * p[1] + T[2][2] * p[2] + T[2][3];

  return ans;
}

ParticleSystem::VectorType ParticleSystem::TransformVector(const VectorType &p, const TransformType &T) const {
  VectorType ans;

  ans[0] = T[0][0] * p[0] + T[0][1] * p[1] + T[0][2] * p[2];
  ans[1] = T[1][0] * p[0] + T[1][1] * p[1] + T[1][2] * p[2];
  ans[2] = T[2][0] * p[0] + T[2][1] * p[1] + T[2][2] * p[2];

  return ans;
}

ParticleSystem::VnlMatrixType ParticleSystem::TransformNormalDerivative(const VnlMatrixType &p,
                                                                        const TransformType &T) const {
  VnlMatrixType ans;

  ans = T.extract(3, 3) * p;  // rotation and scaling part

  return ans;
}

ParticleSystem::ParticleSystem() {}

void ParticleSystem::SetNumberOfDomains(unsigned int num) {
  // in case nothing has changed.
  if (num == m_Domains.size()) {
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
  while (num >= this->m_DomainFlags.size()) {
    m_DomainFlags.push_back(false);
  }
  this->Modified();
}

void ParticleSystem::AddDomain(DomainType *input, int threadId) {
  this->Modified();

  for (unsigned int idx = 0; idx < m_Domains.size(); ++idx) {
    if (!m_Domains[idx]) {
      m_Domains[idx] = input;
      m_Positions[idx] = PointContainerType::New();
      m_IndexCounters[idx] = 0;
      return;
    }
  }

  this->SetNumberOfDomains(static_cast<int>(m_Domains.size() + 1));
  m_Domains[static_cast<int>(m_Domains.size()) - 1] = input;
  m_Positions[static_cast<int>(m_Domains.size()) - 1] = PointContainerType::New();
  m_IndexCounters[static_cast<int>(m_Domains.size() - 1)] = 0;
  m_Neighborhoods[static_cast<int>(m_Domains.size() - 1)] = NeighborhoodType::New();
  m_Transforms[static_cast<int>(m_Domains.size() - 1)].set_identity();
  m_InverseTransforms[static_cast<int>(m_Domains.size() - 1)].set_identity();

  // Notify any observers.
  ParticleDomainAddEvent e;
  e.SetDomainIndex(m_Domains.size() - 1);
  e.SetPositionIndex(0);
  e.SetThreadID(threadId);
  this->InvokeEvent(e);
}

void ParticleSystem::PrintSelf(std::ostream &os, Indent indent) const {
  Superclass::PrintSelf(os, indent);

  os << indent << "m_IndexCounters.size(): " << m_IndexCounters.size() << std::endl;
  os << indent << "m_Positions.size() : " << m_Positions.size() << std::endl;
  os << indent << "m_Domains.size() : " << m_Domains.size() << std::endl;
}

void ParticleSystem::SetTransform(unsigned int i, const TransformType &T, int threadId) {
  if (i > static_cast<int>(m_Transforms.size()) - 1 || m_Transforms.empty()) {
    m_Transforms.resize(i + 1);
    m_InverseTransforms.resize(i + 1);
  }
  m_Transforms[i] = T;
  m_InverseTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticleTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

void ParticleSystem::SetPrefixTransform(unsigned int i, const TransformType &T, int threadId) {
  if (i > static_cast<int>(m_PrefixTransforms.size()) - 1 || m_PrefixTransforms.empty()) {
    m_PrefixTransforms.resize(i + 1);
    m_InversePrefixTransforms.resize(i + 1);
  }

  m_PrefixTransforms[i] = T;
  m_InversePrefixTransforms[i] = this->InvertTransform(T);

  // Notify any observers.
  ParticlePrefixTransformSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

void ParticleSystem::SetNeighborhood(unsigned int i, NeighborhoodType *N, int threadId) {
  if (m_DomainFlags[i] == true) return;
  m_Neighborhoods[i] = N;
  m_Neighborhoods[i]->SetDomain(m_Domains[i]);
  m_Neighborhoods[i]->SetPointContainer(m_Positions[i]);

  // Notify any observers.
  ParticleNeighborhoodSetEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(i);
  this->InvokeEvent(e);
}

const typename ParticleSystem::PointType &ParticleSystem::AddPosition(const PointType &p, unsigned int d,
                                                                      int threadId) {
  m_Positions[d]->operator[](m_IndexCounters[d]) = p;

  // Potentially modifies position!
  if (m_DomainFlags[d] == false) {
    // debugg
    // std::cout << "d" << d << " before apply " << m_Positions[d]->operator[](m_IndexCounters[d]);
    const auto idx = m_IndexCounters[d];
    m_Domains[d]->ApplyConstraints(m_Positions[d]->operator[](idx), idx);
    // debugg
    // std::cout << " after apply " << m_Positions[d]->operator[](m_IndexCounters[d]) << std::endl;
    m_Neighborhoods[d]->AddPosition(m_Positions[d]->operator[](idx), idx, threadId);
  }

  // Increase the FixedParticleFlag list size if necessary.
  if (m_IndexCounters[d] >= m_FixedParticleFlags[d % m_DomainsPerShape].size()) {
    m_FixedParticleFlags[d % m_DomainsPerShape].push_back(false);
  }

  // Notify any observers.
  ParticlePositionAddEvent e;
  e.SetThreadID(threadId);
  e.SetDomainIndex(d);
  e.SetPositionIndex(m_IndexCounters[d]);
  this->InvokeEvent(e);
  m_IndexCounters[d]++;

  return m_Positions[d]->operator[](m_IndexCounters[d] - 1);
}

const typename ParticleSystem::PointType &ParticleSystem::SetPosition(const PointType &p, unsigned long int k,
                                                                      unsigned int d, int threadId) {
  if (m_FixedParticleFlags[d % m_DomainsPerShape][k] == false) {
    // Potentially modifies position!
    if (m_DomainFlags[d] == false) {
      m_Positions[d]->operator[](k) = p;

      // Debuggg
      // std::cout << "SynchronizePositions Apply constraints " << m_Positions[d]->operator[](k);
      m_Domains[d]->ApplyConstraints(m_Positions[d]->operator[](k), k);
      // Debuggg
      // std::cout << " updated " << m_Positions[d]->operator[](k) << std::endl;

      m_Neighborhoods[d]->SetPosition(m_Positions[d]->operator[](k), k, threadId);
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

void ParticleSystem::AddPositionList(const std::vector<PointType> &p, unsigned int d, int threadId) {
  // Traverse the list and add each point to the domain.
  for (typename std::vector<PointType>::const_iterator it = p.begin(); it != p.end(); it++) {
    this->AddPosition(*it, d, threadId);
  }
}

void ParticleSystem::PrintParticleSystem() {
  for (unsigned int d = 0; d < this->GetNumberOfDomains(); d++) {
    std::vector<PointType> list;
    for (unsigned int p = 0; p < this->GetNumberOfParticles(d); p++) {
      list.push_back(this->GetPosition(p, d));
    }
    std::cout << "D " << d << " Curr Pos ";
    for (size_t i = 0; i < list.size(); i++) std::cout << list[i] << " ";
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

void ParticleSystem::AdvancedAllParticleSplitting(double epsilon, unsigned int domains_per_shape,
                                                  unsigned int dom_to_process) {
  size_t num_doms = this->GetNumberOfDomains();

  for (size_t domain = 0; domain < num_doms; domain++) {
    this->GetDomain(domain)->GetConstraints()->InitializeLagrangianParameters(1, 1, 1);
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
        bool good = true;  // flag to check if the new update violates in any domain
        for (size_t j = 0; j < lists.size(); j++) {
          // Add epsilon times random direction to existing point and apply domain
          // constraints to generate a new particle position.
          PointType newpos = this->GetDomain(dom_to_process + j * domains_per_shape)
                                 ->GetPositionAfterSplit(lists[j][i], random, epsilon);
          // Go to surface
          if (!this->m_DomainFlags[dom_to_process + j * domains_per_shape] &&
              !this->GetDomain(dom_to_process + j * domains_per_shape)->GetConstraints()->isAnyViolated(newpos)) {
            this->GetDomain(dom_to_process + j * domains_per_shape)->ApplyConstraints(newpos, -1);
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
            this->AddPosition(newposs_good[j], dom_to_process + j * domains_per_shape, 0);
            // Debuggg
            // std::cout << "Domain " << j << " Curr Pos " << lists[j][i] << " random "
            // << random  << " epsilon " << epsilon << " picked " << newposs_good[j] << std::endl;
          }
          break;
        }

      }  // while end
    }    // for end
  }      // if end
}

void ParticleSystem::RegisterAttribute(ParticleAttribute<3> *attr) {
  // Register any methods defined by the attribute as observers of this
  // ParticleSystem with appropriate events.
  if (attr->m_DefinedCallbacks.Event == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::EventCallback);
    this->AddObserver(ParticleEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.EventWithIndex == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::EventWithIndexCallback);
    this->AddObserver(ParticleEventWithIndex(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.DomainAddEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::DomainAddEventCallback);
    this->AddObserver(ParticleDomainAddEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.TransformSetEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::TransformSetEventCallback);
    this->AddObserver(ParticleTransformSetEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.PrefixTransformSetEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::PrefixTransformSetEventCallback);
    this->AddObserver(ParticlePrefixTransformSetEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.NeighborhoodSetEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::NeighborhoodSetEventCallback);
    this->AddObserver(ParticleNeighborhoodSetEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.PositionSetEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::PositionSetEventCallback);
    this->AddObserver(ParticlePositionSetEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.PositionAddEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::PositionAddEventCallback);
    this->AddObserver(ParticlePositionAddEvent(), tmpcmd);
  }
  if (attr->m_DefinedCallbacks.PositionRemoveEvent == true) {
    MemberCommand<ParticleAttribute<3>>::Pointer tmpcmd = MemberCommand<ParticleAttribute<3>>::New();
    tmpcmd->SetCallbackFunction(attr, &ParticleAttribute<3>::PositionRemoveEventCallback);
    this->AddObserver(ParticlePositionRemoveEvent(), tmpcmd);
  }
}

}  // namespace itk
