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

#include "vnl/vnl_vector_fixed.h"
#include "vnl/vnl_matrix_fixed.h"

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
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, VDimension>& gradE,
    const PointType& pos) const = 0;
  virtual vnl_vector_fixed<double, VDimension> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, VDimension>& gradE,
    const PointType& pos) const = 0;
  
  /** A Domain may optionally return a bounding box.  The lower bound method
      gives the upper-left-hand corner of the domain.  The upper bound method
      gives the lower-right-hand-corner of the domain.  If a domain does not
      define boundaries, these methods will throw an exceptions  */
  virtual const PointType& GetLowerBound() const = 0;
  virtual const PointType& GetUpperBound() const = 0;

  virtual PointType GetZeroCrossingPoint() const = 0;
  virtual double GetSurfaceArea() const = 0;
  virtual double GetMaxDimRadius() const = 0;

  // Cutting Plane constraint functionality
  virtual void PrintCuttingPlaneConstraints(std::ofstream &out) const = 0;
  virtual void SetCuttingPlane(const vnl_vector<double>& a, const vnl_vector<double>& b, const vnl_vector<double> &c) = 0;
  virtual void TransformCuttingPlane(const vnl_matrix_fixed<double, VDimension + 1, VDimension + 1> &Trans) = 0;

  virtual void AddSphere(const vnl_vector_fixed<double, VDimension> &v, double r) = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;

  bool IsDomainFixed() const {
    return m_FixedDomain;
  }

protected:

  // is this a fixed domain or not?  We start as fixed and if an image is set, we set this to true
  bool m_FixedDomain{true};


  ParticleDomain() {}
  virtual ~ParticleDomain() {}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  
private:
  ParticleDomain(const ParticleDomain&); //purposely not implemented
  void operator=(const ParticleDomain&); //purposely not implemented

};

} // end namespace itk
