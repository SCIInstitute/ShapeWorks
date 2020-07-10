/*=========================================================================
  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#pragma once

#include "itkParticleImageDomainWithCurvature.h"

#include "vnl/vnl_inverse.h"

#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "meshFIM.h"

namespace itk
{
/** \class ParticleImplicitSurfaceDomain
 *
 *  A 3D cartesian domain that constrains points so that they always lie an
 *  implicit surface. The implicit surface is defined as the zero isosurface of
 *  the given image.   Constraints are applied using a Newton-Raphson
 *  iteration, and this class assumes it has a distance transform
 *  as an image.
 */
template <class T>
class ParticleImplicitSurfaceDomain : public ParticleImageDomainWithCurvature<T>
{
public:
  /** Standard class typedefs */
  typedef ParticleImageDomainWithCurvature<T> Superclass;
  typedef SmartPointer<ParticleImplicitSurfaceDomain>  Pointer;

  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  /** Method for creation through the object factory. */
  itkNewMacro(ParticleImplicitSurfaceDomain);

  /** Set/Get the precision of the projection operation.  The resulting projection
      will be within the specified tolerance. */
  virtual void SetTolerance(const T _Tolerance) {
    if (this->m_Tolerance != _Tolerance) 
    {
      this->m_Tolerance = _Tolerance;
      this->Modified();
    }
  }
  virtual T GetTolerance() {
    return this->m_Tolerance;
  }

  shapeworks::DomainType GetDomainType() const override {
    return shapeworks::DomainType::Image;
  }
  
  /** Apply any constraints to the given point location.  This method
      constrains points to lie within the given domain and on a given implicit
      surface.  If the point is not already on the surface, it is projected
      back to the surface using a Newton-Raphson iteration.  IMPORTANT: This
      method returns the true/false value of its superclass, and does not
      indicate changes only due to projection.  This is done for speed: we
      typically will only want to know if a point tried to move outside of the
      bounding box domain, since movement off the surface will be very
      common.  Consider subclassing this method to add a check for significant
      differences in the input and output points. */
  virtual bool ApplyConstraints(PointType &p) const override;

  /** Optionally add a repulsion from a planar boundar specified in
      m_CuttingPlane */
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, DIMENSION> &gradE, const PointType &pos) const override;


  inline PointType UpdateParticlePosition(PointType &point, vnl_vector_fixed<double, DIMENSION> &update) const override {
    PointType newpoint;
    for (unsigned int i = 0; i < DIMENSION; i++) { newpoint[i] = point[i] - update[i]; }
    ApplyConstraints(newpoint);
    return newpoint;
  }

  /** Define a distance measure on the surface.  Note that this distance
      measure is NOT the geodesic distance, as one might expect, but is only a
      Euclidean distance which ignores points whose normals are not
      sufficiently aligned (method returns a negative number).  The assumption
      here is that points are sufficiently close to one another on the surface
      that they may be considered to lie in a tangent plane. */
  virtual double Distance(const PointType &, const PointType &) const override;

  void SetCuttingPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                       const vnl_vector<double> &c);

  /** Transform cutting planes based on base index. Base plane coordinates passed as argument. */
  void TransformCuttingPlane(const vnl_matrix_fixed<double, DIMENSION + 1, DIMENSION + 1> &Trans) override;


  void SetMesh(TriMesh *mesh);
  void SetFeaMesh(const char *feaFile);
  void SetFeaGrad(const char *feaGradFile);
  void SetFids(const char *fidsFile);
  TriMesh* GetMesh()
  {
      return m_mesh;
  }
  TriMesh* GetMesh() const
  {
      return m_mesh;
  }

  void RemoveCuttingPlane()  { m_UseCuttingPlane = false; }

  void RemoveCuttingSphere()  { m_UseCuttingSphere = false; }

  bool IsCuttingPlaneDefined() const {return m_UseCuttingPlane;}

  bool IsCuttingSphereDefined() const {return m_UseCuttingSphere;}
  
  const vnl_vector_fixed<double, DIMENSION> &GetCuttingPlanePoint() const
  { return m_CuttingPlanePoint[0]; }
  const vnl_vector_fixed<double, DIMENSION> &GetCuttingPlaneNormal() const
  { return m_CuttingPlaneNormal[0]; }

  //Praful
  const vnl_vector_fixed<double, DIMENSION> &GetCuttingPlanePoint(int i) const
  { return m_CuttingPlanePoint[i]; }
  const vnl_vector_fixed<double, DIMENSION> &GetCuttingPlaneNormal(int i) const
  { return m_CuttingPlaneNormal[i]; }


  // Prateep
  const vnl_vector_fixed<double, DIMENSION> &GetA() const
  { return m_a[0]; }
  const vnl_vector_fixed<double, DIMENSION> &GetB() const
  { return m_b[0]; }
  const vnl_vector_fixed<double, DIMENSION> &GetC() const
  { return m_c[0]; }

  /** Maintain a list of spheres within the domain.  These are used as 
      soft constraints by some particle forcing functions. */
  void AddSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r) override
  {
//    if (r > 0) -- Praful, sign will be used to determine inwards or outwards
//    {
      m_SphereCenterList.push_back(v);
      m_SphereRadiusList.push_back(r);
      m_UseCuttingSphere = true;
//    }
  }
  
  /** Returns the radius of sphere i, or 0.0 if sphere i does not exist */
  double GetSphereRadius(unsigned int i) const
  {
    if (m_SphereRadiusList.size() > i) return m_SphereRadiusList[i];
    else return 0.0;
  }

  /** Returns the center point of sphere i.  If sphere i does not exist,
      returns 0 vector. */
  vnl_vector_fixed<double, DIMENSION> GetSphereCenter(unsigned int i) const
  {
    if (m_SphereRadiusList.size() > i) return m_SphereCenterList[i];
    else
      {  return vnl_vector_fixed<double, DIMENSION>(0.0,0.0,0.0);  }
  }

  unsigned int GetNumberOfSpheres() const
  {
    return m_SphereCenterList.size();
  }
    
  unsigned int GetNumberOfPlanes() const
  {
      return m_CuttingPlanePoint.size();
  }

  void PrintCuttingPlaneConstraints(std::ofstream &out) const override {
    for (unsigned int j = 0; j < GetNumberOfPlanes(); j++) {
      vnl_vector_fixed<double, DIMENSION> a = m_a[j];
      vnl_vector_fixed<double, DIMENSION> b = m_b[j];
      vnl_vector_fixed<double, DIMENSION> c = m_c[j];
      for (int d = 0; d < 3; d++) {
        out << a[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << b[d] << " ";
      }
      for (int d = 0; d < 3; d++) {
        out << c[d] << " ";
      }
      out << std::endl;
    }
  }

protected:
  ParticleImplicitSurfaceDomain() : m_Tolerance(1.0e-4), m_UseCuttingPlane(false), m_UseCuttingSphere(false)
  {
    m_mesh = NULL;
  }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os, indent);
    os << indent << "m_Tolerance = " << m_Tolerance << std::endl;
  }
  virtual ~ParticleImplicitSurfaceDomain() {};

private:
  T m_Tolerance;
  bool m_UseCuttingPlane;
  bool m_UseCuttingSphere;
  //Praful - adding ability to use more than one cutting planes
  std::vector < vnl_vector_fixed<double, DIMENSION> > m_CuttingPlanePoint;
  std::vector < vnl_vector_fixed<double, DIMENSION> > m_CuttingPlaneNormal;

  // Prateep -- Praful
  std::vector < vnl_vector_fixed<double, DIMENSION> > m_a;
  std::vector < vnl_vector_fixed<double, DIMENSION> > m_b;
  std::vector < vnl_vector_fixed<double, DIMENSION> > m_c;
  
  TriMesh *m_mesh;

  std::vector< vnl_vector_fixed<double, DIMENSION> > m_SphereCenterList;
  std::vector< double > m_SphereRadiusList;



  // Praful
  bool SphereVectorConstraintMayOrMayNotWork(vnl_vector_fixed<double, DIMENSION>& gradE,
    const PointType& pos) const;
};

} // end namespace itk

#include "itkParticleImplicitSurfaceDomain.txx"
