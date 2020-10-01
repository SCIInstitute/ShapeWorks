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

#include "DomainType.h"
#include "Constraints.h"

namespace itk
{
class ParticleDomain : public DataObject
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ParticleDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef Point<double, DIMENSION> PointType;

  /** Apply any constraints to the given point location.
      This should force the point to a position on the surface that satisfies all constraints. */
  virtual bool ApplyConstraints(PointType& p, bool dbg = false) const = 0;

  /** Applies the update to the point and returns the new point position. */
  virtual PointType UpdateParticlePosition(const PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const = 0;

  /** Projects the vector to the surface tangent at the point. */
  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos) const = 0;

  virtual vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point) const = 0;
  virtual vnl_matrix_fixed<float, DIMENSION, DIMENSION> SampleHessianAtPoint(const PointType &p) const = 0;
  /** Distance between locations is used for computing energy and neighborhoods. */
  virtual double Distance(const PointType &a, const PointType &b) const {
    return a.EuclideanDistanceTo(b);
  }
  /** Squared Distance between locations is used for computing sigma. */
  virtual double SquaredDistance(const PointType &a, const PointType &b) const {
    return a.SquaredEuclideanDistanceTo(b);
  }


  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetCurvature(const PointType &p) const = 0;
  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetSurfaceMeanCurvature() const = 0;
  /** Used in ParticleMeanCurvatureAttribute */
  virtual double GetSurfaceStdDevCurvature() const = 0;

  /** Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the PowerOfTwoPointTree. */
  virtual const PointType& GetLowerBound() const = 0;
  /** Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the PowerOfTwoPointTree. */
  virtual const PointType& GetUpperBound() const = 0;

  /** Get any valid point on the domain. This is used to place the first particle. */
  virtual PointType GetZeroCrossingPoint() const = 0;
  /** Use for neighborhood radius. */
  virtual double GetSurfaceArea() const = 0;

  /** GetValidLocation returns a PointType location on the surface. Used for placing the first particle. */
  virtual PointType GetValidLocationNear(PointType p) const = 0;
  /** GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma */
  virtual double GetMaxDiameter() const = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;
  virtual void UpdateZeroCrossingPoint() = 0;

  bool IsDomainFixed() const {
    return m_FixedDomain;
  }
  virtual shapeworks::DomainType GetDomainType() const = 0;

  Constraints *GetConstraints() const {return constraints;}

protected:

  // is this a fixed domain or not?  We start as fixed and if an image or mesh is set, we set this to false
  bool m_FixedDomain{true};

  ParticleDomain() {constraints = new Constraints();}
  virtual ~ParticleDomain() {delete constraints;}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
  }
  Constraints *constraints;

private:
  ParticleDomain(const ParticleDomain&); //purposely not implemented
  void operator=(const ParticleDomain&); //purposely not implemented


};

} // end namespace itk
