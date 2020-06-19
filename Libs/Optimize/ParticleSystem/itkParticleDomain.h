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
  virtual bool ApplyConstraints(PointType& p) const = 0;
  /** Reduce magnitude of the vector so that applying point = point + gradE does not violate any constraints.
      This should have no effect if there are no constraints. ImageDomain may restrict vector magnitude based on the narrow band. */
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos) const = 0;

  /** Applies the update to the point and returns the new point position. */
  virtual PointType UpdateParticlePosition(PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const = 0;

  /** Projects the vector to the surface tangent at the point. */
  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos) const = 0;
  /** Get the surface normal at a point. */
  virtual vnl_vector_fixed<float, 3> SampleNormalAtPoint(const PointType & point) const = 0;

  /** A Domain may define a distance calculation.  This is useful in cases
      such as geodesic distance, where distance depends on some information
      contained in the Domain.  The default implementation is Euclidean
      distance. */
  virtual double Distance(const PointType &a, const PointType &b) const
  {
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
  /** Used to compute sigma for sampling and neighborhood radius. */
  virtual double GetMaxDimRadius() const = 0;

  // Cutting Plane constraint functionality
  virtual void PrintCuttingPlaneConstraints(std::ofstream &out) const = 0;
  virtual void SetCuttingPlane(const vnl_vector<double>& a, const vnl_vector<double>& b, const vnl_vector<double> &c) = 0;
  virtual void TransformCuttingPlane(const vnl_matrix_fixed<double, DIMENSION + 1, DIMENSION + 1> &Trans) = 0;

  virtual void AddSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r) = 0;

  virtual void DeleteImages() = 0;
  virtual void DeletePartialDerivativeImages() = 0;

  bool IsDomainFixed() const {
    return m_FixedDomain;
  }
  virtual shapeworks::DomainType GetDomainType() const = 0;

protected:

  // is this a fixed domain or not?  We start as fixed and if an image or mesh is set, we set this to false
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
