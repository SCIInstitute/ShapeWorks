/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkDataObject.h"
#include "itkPoint.h"

namespace itk
{
template <unsigned int VDimension=3>
class ParticleDomain : public DataObject
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef Point<double, VDimension> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  virtual bool ApplyConstraints(PointType& p) const = 0;

  /** A Domain may define a distance calculation.  This is useful in cases
      such as geodesic distance, where distance depends on some information
      contained in the Domain.  The default implementation is Euclidean
      distance. */
  virtual double Distance(const PointType &a, const PointType &b) const
  {
    double sum = 0.0;
    for (unsigned int i = 0; i < VDimension; i++)
      {      sum += (b[i]-a[i]) * (b[i]-a[i]);      }
    return sqrt(sum);
  }

  /** Return a position that is on the surface */
  virtual PointType GetPositionOnSurface() const = 0;
  
  /** A Domain may optionally return a bounding box.  The lower bound method
      gives the upper-left-hand corner of the domain.  The upper bound method
      gives the lower-right-hand-corner of the domain.  If a domain does not
      define boundaries, these methods will throw an exceptions  */
  virtual const PointType& GetLowerBound() const = 0;
  virtual const PointType& GetUpperBound() const = 0;

 /** Enable/Disable constraints on particle positions imposed by a domain. */
  void DisableConstraints()  { m_ConstraintsEnabled = false; }
  void EnableConstraints()   { m_ConstraintsEnabled = true; }    
  bool GetConstraintsEnabled() const  { return m_ConstraintsEnabled; }
  void SetConstraintsEnabled( bool g )  { m_ConstraintsEnabled = g; }

protected:
  ParticleDomain() : m_ConstraintsEnabled(true) {}
  virtual ~ParticleDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }

  bool m_ConstraintsEnabled;
  
private:
  ParticleDomain(const ParticleDomain&); //purposely not implemented
  void operator=(const ParticleDomain&); //purposely not implemented

};

} // end namespace itk
