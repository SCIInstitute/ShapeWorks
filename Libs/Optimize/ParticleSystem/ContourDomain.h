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
class ContourDomain : public ParticleDomain
{
public:
  /** Standard class typedefs */
  typedef SmartPointer<ContourDomain>  Pointer;

  /** Point type used to store particle locations. */
  typedef typename ParticleDomain::PointType PointType;

  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Contour;
  }

  /** Apply any constraints to the given point location.
      This should force the point to a position on the surface that satisfies all constraints. */
  inline bool ApplyConstraints(PointType& p, bool dbg = false) const override {
    // TODO snap the point to the closest position on the contour.
    return true;
  }

  /** Reduce magnitude of the vector so that applying point = point + gradE does not violate any constraints.
      This should have no effect if there are no constraints. ImageDomain may restrict vector magnitude based on the narrow band. */
  inline bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> & gradE, const PointType &pos) const override {
    return true;
  }

  /** Applies the update to the point and returns the new point position. */
  inline PointType UpdateParticlePosition(PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const override {
    PointType newpoint;
    // TODO implement geodesic walk for contours
    for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i]; }
    return newpoint;
  }

  /** Projects the vector to the surface tangent at the point. */
  inline vnl_vector_fixed<double, DIMENSION> ProjectVectorToSurfaceTangent(vnl_vector_fixed<double, DIMENSION> & gradE, const PointType &pos) const override {
    // TODO project vector to surface tangent at pos
    return gradE;
  }

  /** Get the surface normal at a point. */
  inline vnl_vector_fixed<float, DIMENSION> SampleNormalAtPoint(const PointType &point) const override {
    vnl_vector_fixed<float, DIMENSION> normal(1, 0, 0);
    // TODO return normal at point
    return normal;
  }

  /** Used in ParticleMeanCurvatureAttribute */
  double GetCurvature(const PointType &p) const override {
    return GetSurfaceMeanCurvature();
  }

  /** Used in ParticleMeanCurvatureAttribute */
  inline double GetSurfaceMeanCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.15;
  }

  /** Used in ParticleMeanCurvatureAttribute */
  inline double GetSurfaceStdDevCurvature() const override {
    // This function is used by MeanCurvatureAttribute which is used for good/bad assessment
    // These arbitrary values should eventually be replaced with actual computation
    return 0.02;
  }

  /** Each domain can compute the distance between two points */
  inline double Distance(const PointType &a, const PointType &b) const override {
    // TODO compute geodesic distance on the contour
    return 1.0;
  }

  /** Gets the minimum x, y, z values of the bounding box for the domain. This is used for setting up the PowerOfTwoPointTree. */
  const PointType& GetLowerBound() const override {
    PointType p;
    // TODO get the lower bounds of the domain.
    return p;
  }

  /** Gets the maximum x, y, z values of the bounding box for the domain. This is used for setting up the PowerOfTwoPointTree. */
  const PointType& GetUpperBound() const override {
    PointType p;
    // TODO get the upper bounds of the domain.
    return p;
  }

  /** Get any valid point on the domain. This is used to place the first particle. */
  PointType GetZeroCrossingPoint() const override {
    PointType p;
    // TODO return any point on the contour.
    return p;
  }

  /** Use for neighborhood radius. */
  double GetSurfaceArea() const override {
    // TODO return contour equivalent of surface area
    return 0;
  }

  /** Used to compute sigma for sampling and neighborhood radius. */
  double GetMaxDimRadius() const override {
    // TODO return average edge length
    return 1;
  }


  void PrintCuttingPlaneConstraints(std::ofstream& out) const override {
    // TODO for Hong: figure out constraint thing
  }
  void SetCuttingPlane(const vnl_vector<double>& a, const vnl_vector<double>& b, const vnl_vector<double>& c) {
    // TODO for Hong: figure out constraint thing
  }
  void TransformCuttingPlane(const vnl_matrix_fixed<double, DIMENSION + 1, DIMENSION + 1> & Trans) {
    // TODO for Hong: figure out constraint thing
  }
  void AddSphere(const vnl_vector_fixed<double, DIMENSION>& v, double r) {
    // TODO for Hong: figure out constraint thing
  }


  void DeleteImages() override {
    // TODO delete the contours
  }
  void DeletePartialDerivativeImages() override {
    // TODO delete gradients computed from contours (if they exist)
  }


  ContourDomain() { }
  virtual ~ContourDomain() {}

protected:
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    DataObject::Superclass::PrintSelf(os, indent);
    os << indent << "ContourDomain\n";
  }

private:

};

} // end namespace itk
