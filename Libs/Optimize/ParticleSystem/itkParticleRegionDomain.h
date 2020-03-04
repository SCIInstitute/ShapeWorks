/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleDomain.h"

namespace itk
{
template <unsigned int VDimension=3>
class ParticleRegionDomain : public ParticleDomain<VDimension>
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleRegionDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain<VDimension>::PointType PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface. This function will clip the point to the boundaries. */
  virtual bool ApplyConstraints(PointType &p) const
  {
    bool changed = false;
    for (unsigned int i = 0; i < VDimension; i++)
    {
        if (p[i] < GetLowerBound()[i]) {
            changed = true;
            p[i] = GetLowerBound()[i];
        }
        else if (p[i] > GetUpperBound()[i]) {
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
  void SetRegion(const PointType &lowerBound, const PointType &upperBound)
  {
    SetLowerBound(lowerBound);
    SetUpperBound(upperBound);
  }
  
protected:
  ParticleRegionDomain() {}
  virtual ~ParticleRegionDomain() {};

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    ParticleDomain<VDimension>::PrintSelf(os, indent);
    os << "LowerBound = " << GetLowerBound() << std::endl;
    os << "UpperBound = " << GetUpperBound() << std::endl;
  }
  
private:
  PointType m_LowerBound;
  PointType m_UpperBound;
};

} // end namespace itk
