---
title: Libs/Optimize/ParticleSystem.h

---

# Libs/Optimize/ParticleSystem.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md)** <br>A facade class managing interactions with a particle system.  |




## Source code

```cpp
#pragma once

#include <map>
#include <random>
#include <vector>

#include "Libs/Optimize/Container/GenericContainer.h"
#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "Libs/Optimize/Neighborhood/ParticleNeighborhood.h"
#include "Observer.h"
#include "ParticleEvents.h"
#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkEventObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"

namespace shapeworks {
class ParticleSystem : public itk::DataObject {
 public:
  static constexpr int VDimension = 3;

  typedef ParticleSystem Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  itkNewMacro(Self);

  itkTypeMacro(ParticleSystem, itk::DataObject);

  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  using DomainType = shapeworks::ParticleDomain;

  typedef itk::Point<double, VDimension> PointType;

  typedef GenericContainer<PointType> PointContainerType;

  //  typedef Transform<double, VDimension, VDimension> TransformType;
  typedef vnl_matrix_fixed<double, VDimension + 1, VDimension + 1> TransformType;
  typedef vnl_vector_fixed<double, VDimension> VectorType;
  typedef vnl_matrix<double> VnlMatrixType;

  void RegisterObserver(Observer *attr);

  void SynchronizePositions() {
    for (unsigned int d = 0; d < this->GetNumberOfDomains(); d++) {
      for (unsigned int p = 0; p < this->GetNumberOfParticles(d); p++) {
        this->SetPosition(this->GetPosition(p, d), p, d);
      }
    }
  }

  unsigned long int GetNumberOfParticles(unsigned int d = 0) const { return m_Positions[d]->GetSize(); }

  const PointType &AddPosition(const PointType &, unsigned int d = 0);
  const PointType &SetPosition(const PointType &, unsigned long int k, unsigned int d = 0);

  void RemovePosition(unsigned long int k, unsigned int d = 0);

  PointType &GetPosition(unsigned long int k, unsigned int d = 0) { return m_Positions[d]->operator[](k); }
  const PointType &GetPosition(unsigned long int k, unsigned int d = 0) const { return m_Positions[d]->operator[](k); }
  PointType GetTransformedPosition(unsigned long int k, unsigned int d = 0) const {
    return this->TransformPoint(m_Positions[d]->operator[](k), m_Transforms[d] * m_PrefixTransforms[d]);
  }
  PointType GetPrefixTransformedPosition(unsigned long int k, unsigned int d = 0) const {
    return this->TransformPoint(m_Positions[d]->operator[](k), m_PrefixTransforms[d]);
  }

  void SplitAllParticles(double epsilon);
  void SplitParticle(double epsilon, unsigned int idx, unsigned int d = 0);
  void AdvancedAllParticleSplitting(double epsilon, unsigned int domains_per_shape, unsigned int dom_to_process);
  // Debug function
  void PrintParticleSystem();

  std::shared_ptr<ParticleNeighborhood> GetNeighborhood(unsigned int k) const { return m_Neighborhoods[k]; }

  using PointVectorType = std::vector<ParticlePointIndexPair>;

  PointVectorType FindNeighborhoodPoints(const PointType &p, int idx, double r, unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(p, idx, r);
  }
  inline PointVectorType FindNeighborhoodPoints(const PointType &p, int idx, std::vector<double> &w,
                                                std::vector<double> &distances, double r, unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(p, idx, w, distances, r);
  }
  inline PointVectorType FindNeighborhoodPoints(const PointType &p, int idx, std::vector<double> &w, double r,
                                                unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(p, idx, w, r);
  }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx, double r, unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(GetPosition(idx, d), idx, r);
  }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx, std::vector<double> &w,
                                                std::vector<double> &distances, double r, unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(GetPosition(idx, d), idx, w, distances, r);
  }
  inline PointVectorType FindNeighborhoodPoints(unsigned int idx, std::vector<double> &w, double r,
                                                unsigned int d = 0) const {
    return m_Neighborhoods[d]->find_neighborhood_points(GetPosition(idx, d), idx, w, r);
  }

  //  inline int FindNeighborhoodPoints(const PointType &p,  double r, PointVectorType &vec, unsigned int d = 0) const
  //  {  return m_Neighborhoods[d]->FindNeighborhoodPoints(p, r, vec); }

  //   PointVectorType FindTransformedNeighborhoodPoints(const PointType &p, double r, unsigned int d = 0) const
  //   {
  //     PointVectorType ans = m_Neighborhoods[d]
  //       ->FindNeighborhoodPoints(this->TransformPoint(p, InverseTransform[d]), r);
  //     for (unsigned int i = 0; i < ans.size(); i++)
  //       {
  //       ans.Point[i] = this->TransformPoint(ans.Point[i], m_Transform[d]);
  //       }
  //     return ans;
  //   }

  void AddDomain(DomainType::Pointer input);

  std::vector<DomainType::Pointer>::const_iterator GetDomainsBegin() const { return m_Domains.begin(); }

  std::vector<DomainType::Pointer>::const_iterator GetDomainsEnd() const { return m_Domains.end(); }

  DomainType *GetDomain(unsigned int i) { return m_Domains[i].get(); }

  DomainType *GetDomain() { return m_Domains[0].get(); }

  const DomainType *GetDomain(unsigned int i) const { return m_Domains[i].get(); }

  const DomainType *GetDomain() const { return m_Domains[0].get(); }

  unsigned int GetNumberOfDomains() const { return m_Domains.size(); }

  void SetTransform(unsigned int i, const TransformType &);
  void SetTransform(const TransformType &p) { this->SetTransform(0, p); }
  void SetPrefixTransform(unsigned int i, const TransformType &);
  void SetPrefixTransform(const TransformType &p) { this->SetPrefixTransform(0, p); }

  std::vector<TransformType>::const_iterator GetTransformsBegin() const { return m_Transforms.begin(); }

  std::vector<TransformType>::const_iterator GetTransformsEnd() const { return m_Transforms.end(); }

  const TransformType &GetTransform(unsigned int i) const { return m_Transforms[i]; }

  const TransformType &GetTransform() const { return m_Transforms[0]; }

  TransformType GetTransform(unsigned int i) { return m_Transforms[i]; }

  TransformType GetTransform() { return m_Transforms[0]; }

  const TransformType &GetPrefixTransform(unsigned int i) const { return m_PrefixTransforms[i]; }

  const TransformType &GetPrefixTransform() const { return m_PrefixTransforms[0]; }

  TransformType GetPrefixTransform(unsigned int i) { return m_PrefixTransforms[i]; }

  TransformType GetPrefixTransform() { return m_PrefixTransforms[0]; }

  std::vector<TransformType>::const_iterator GetInverseTransformsBegin() const { return m_InverseTransforms.begin(); }

  std::vector<TransformType>::const_iterator GetInverseTransformsEnd() const { return m_InverseTransforms.end(); }

  const TransformType &GetInverseTransform(unsigned int i) const { return m_InverseTransforms[i]; }

  const TransformType &GetInverseTransform() const { return m_InverseTransforms[0]; }

  const TransformType &GetInversePrefixTransform(unsigned int i) const { return m_InversePrefixTransforms[i]; }

  const TransformType &GetInversePrefixTransform() const { return m_InversePrefixTransforms[0]; }

  const std::vector<PointContainerType::Pointer> &GetPositions() const { return m_Positions; }
  const PointContainerType::Pointer &GetPositions(unsigned int d) const { return m_Positions[d]; }

  void AddPositionList(const std::vector<PointType> &, unsigned int d = 0);

  PointType TransformPoint(const PointType &, const TransformType &) const;

  VectorType TransformVector(const VectorType &, const TransformType &) const;

  VnlMatrixType TransformNormalDerivative(const VnlMatrixType &, const TransformType &) const;

  inline TransformType InvertTransform(const TransformType &T) const {
    // Note, vnl_inverse is optimized for small matrices 1x1 - 4x4
    return vnl_inverse(T);
  }

  void FlagDomain(unsigned int i) {
    // ensure large enough
    while (i >= this->m_DomainFlags.size()) {
      m_DomainFlags.push_back(false);
    }

    // set the flag
    m_DomainFlags[i] = true;
  }
  void UnflagDomain(unsigned int i) { m_DomainFlags[i] = false; }
  bool GetDomainFlag(unsigned int i) const {
    if (i >= m_DomainFlags.size()) {
      // not set
      return false;
    }
    return m_DomainFlags[i];
  }
  const std::vector<bool> &GetDomainFlags() const { return m_DomainFlags; }
  void SetDomainFlags() {
    for (unsigned int i = 0; i < m_DomainFlags.size(); i++) {
      m_DomainFlags[i] = true;
    }
  }
  void ResetDomainFlags() {
    for (unsigned int i = 0; i < m_DomainFlags.size(); i++) {
      m_DomainFlags[i] = false;
    }
  }

  void SetFixedParticleFlag(unsigned int d, unsigned int i) { m_FixedParticleFlags[d][i] = true; }
  void ResetFixedParticleFlag(unsigned int d, unsigned int i) { m_FixedParticleFlags[d][i] = false; }
  bool GetFixedParticleFlag(unsigned int d, unsigned int i) const { return m_FixedParticleFlags[d][i]; }
  void ResetFixedParticleFlags() {
    for (unsigned d = 0; d < m_FixedParticleFlags.size(); d++) {
      for (unsigned int i = 0; i < m_FixedParticleFlags[d].size(); i++) m_FixedParticleFlags[d][i] = false;
    }
  }

  void SetDomainsPerShape(unsigned int num) {
    m_DomainsPerShape = num;
    m_FixedParticleFlags.resize(m_DomainsPerShape);
  }
  unsigned int GetDomainsPerShape() const { return m_DomainsPerShape; }

  void SetNumberOfDomains(unsigned int);

  // Returns the maximum distance between nearest neighbors in domain dom
  double ComputeMaxDistNearestNeighbors(size_t dom);

  void SetFieldAttributes(const std::vector<std::string> &field_attributes) { m_FieldAttributes = field_attributes; }

  const std::vector<std::string> &GetFieldAttributes() const { return m_FieldAttributes; }

 protected:
  ParticleSystem();
  void PrintSelf(std::ostream &os, itk::Indent indent) const;
  virtual ~ParticleSystem(){};

  TransformType &GetInverseTransform(unsigned int i) { return m_InverseTransforms[i]; }

  TransformType &GetInverseTransform() { return m_InverseTransforms[0]; }

  TransformType &GetInversePrefixTransform(unsigned int i) { return m_InversePrefixTransforms[i]; }

  TransformType &GetInversePrefixTransform() { return m_InversePrefixTransforms[0]; }

 private:
  ParticleSystem(const Self &);  // purposely not implemented
  void operator=(const Self &);  // purposely not implemented

  std::vector<PointContainerType::Pointer> m_Positions;

  std::vector<DomainType::Pointer> m_Domains;

  unsigned int m_DomainsPerShape;

  std::vector<std::shared_ptr<ParticleNeighborhood>> m_Neighborhoods;

  std::vector<TransformType> m_Transforms;

  std::vector<TransformType> m_InverseTransforms;

  std::vector<TransformType> m_PrefixTransforms;

  std::vector<TransformType> m_InversePrefixTransforms;

  std::vector<unsigned long int> m_IndexCounters;

  std::vector<bool> m_DomainFlags;

  std::vector<std::vector<bool>> m_FixedParticleFlags;

  std::vector<std::string> m_FieldAttributes;

  std::mt19937 m_rand{42};
};

}  // end namespace shapeworks
```


-------------------------------

Updated on 2024-09-19 at 07:07:12 +0000
