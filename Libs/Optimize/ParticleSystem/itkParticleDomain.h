/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include <atomic>
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

  static std::atomic<long> euclideanDistanceCount;
  static std::atomic<long> sqEuclideanDistanceCount;

  /** Standard class typedefs */
  typedef SmartPointer<ParticleDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef Point<double, DIMENSION> PointType;

  /** Apply any constraints to the given point location.  This method may, for
      example, implement boundary conditions or restrict points to lie on a
      surface.  Default behavior does nothing.  Returns true if the value of
      the point was modified and false otherwise. */
  virtual bool ApplyConstraints(PointType& p) const = 0;
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos) const = 0;

  virtual PointType UpdateParticlePosition(const PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const = 0;

  virtual vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION>& gradE, const PointType& pos) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleGradientAtPoint(const PointType &point) const = 0;
  virtual vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType & point) const = 0;
  virtual vnl_matrix_fixed<float, DIMENSION, DIMENSION> SampleHessianAtPoint(const PointType &p) const = 0;

  /** Distance between locations is used for computing energy and neighborhoods. */
  virtual double Distance(const PointType &a, const PointType &b) const {
    throw std::runtime_error("boo");
    euclideanDistanceCount++;
    return a.EuclideanDistanceTo(b);
  }
  /** Squared Distance between locations is used for computing sigma. */
  virtual double SquaredDistance(const PointType &a, const PointType &b) const {
    throw std::runtime_error("boo");
    sqEuclideanDistanceCount++;
    return a.SquaredEuclideanDistanceTo(b);
  }

  virtual double GetCurvature(const PointType &p) const = 0;
  virtual double GetSurfaceMeanCurvature() const = 0;
  virtual double GetSurfaceStdDevCurvature() const = 0;
  
  /** A Domain may optionally return a bounding box.  The lower bound method
      gives the upper-left-hand corner of the domain.  The upper bound method
      gives the lower-right-hand-corner of the domain.  If a domain does not
      define boundaries, these methods will throw an exceptions  */
  virtual const PointType& GetLowerBound() const = 0;
  virtual const PointType& GetUpperBound() const = 0;

  /** GetValidLocation returns a PointType location on the surface. Used for placing the first particle. */
  virtual PointType GetValidLocationNear(PointType p) const = 0;
  virtual double GetSurfaceArea() const = 0;
  /** GetMaxDiameter returns the maximum diameter of the domain and is used for computing sigma */
  virtual double GetMaxDiameter() const = 0;

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
