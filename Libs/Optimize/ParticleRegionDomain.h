#pragma once

#include "ParticleDomain.h"

namespace shapeworks {

class ParticleRegionDomain : public ParticleDomain {
 public:
  /** Standard class typedefs */
  using Pointer = std::shared_ptr<ParticleRegionDomain>;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface. This function will clip the point to the boundaries. */
  virtual bool ApplyConstraints(PointType &p) const {
    bool changed = false;
    for (unsigned int i = 0; i < DIMENSION; i++) {
      if (p[i] < GetLowerBound()[i]) {
        changed = true;
        p[i] = GetLowerBound()[i];
      } else if (p[i] > GetUpperBound()[i]) {
        changed = true;
        p[i] = GetUpperBound()[i];
      }
    }
    return changed;
  }

  virtual const PointType &GetUpperBound() const { return m_UpperBound; }
  virtual const PointType &GetLowerBound() const { return m_LowerBound; }
  void SetUpperBound(const PointType _UpperBound) { m_UpperBound = _UpperBound; }
  void SetLowerBound(const PointType _LowerBound) { m_LowerBound = _LowerBound; }
  void SetRegion(const PointType &lowerBound, const PointType &upperBound) {
    SetLowerBound(lowerBound);
    SetUpperBound(upperBound);
  }

 protected:
  ParticleRegionDomain() {}
  virtual ~ParticleRegionDomain(){};

  void PrintSelf(std::ostream &os, itk::Indent indent) const {
    os << "LowerBound = " << GetLowerBound() << std::endl;
    os << "UpperBound = " << GetUpperBound() << std::endl;
  }

  /** Check whether the point p may be sampled in this domain. */
  inline bool IsInsideBuffer(const PointType &p) const {
    for (int i = 0; i < DIMENSION; i++) {
      if (p[i] < m_LowerBound[i] || p[i] > m_UpperBound[i]) {
        return false;
      }
    }
    return true;
  }

 private:
  PointType m_LowerBound;
  PointType m_UpperBound;
};

}  // namespace shapeworks
