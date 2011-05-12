/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImplicitSurfaceDomain.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImplicitSurfaceDomain_h
#define __itkParticleImplicitSurfaceDomain_h

#include "itkParticleImageDomainWithCurvature.h"

#ifdef SW_USE_MESH
#include "TriMesh.h"
#include "meshFIM.h"
#endif

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
template <class T, unsigned int VDimension=3>
class ITK_EXPORT ParticleImplicitSurfaceDomain
  : public ParticleImageDomainWithCurvature<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef ParticleImplicitSurfaceDomain Self;
  typedef ParticleImageDomainWithCurvature<T, VDimension> Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::PointType PointType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleImplicitSurfaceDomain, ParticleClipByRegionDomain);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** Set/Get the precision of the projection operation.  The resulting projection
      will be within the specified tolerance. */
  itkSetMacro(Tolerance, T);
  itkGetMacro(Tolerance, T);
  
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
  virtual bool ApplyConstraints(PointType &p) const;

  /** Optionally add a repulsion from a planar boundar specified in
      m_CuttingPlane */
  virtual bool ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                                      const PointType &pos,
                                      double maxtimestep) const;

  /** Define a distance measure on the surface.  Note that this distance
      measure is NOT the geodesic distance, as one might expect, but is only a
      Euclidean distance which ignores points whose normals are not
      sufficiently aligned (method returns a negative number).  The assumption
      here is that points are sufficiently close to one another on the surface
      that they may be considered to lie in a tangent plane. */
  virtual double Distance(const PointType &, const PointType &) const;

  void SetCuttingPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                       const vnl_vector<double> &c);

#ifdef SW_USE_MESH
  void SetMesh(TriMesh *mesh, const char *gfile);
#endif
  
  void RemoveCuttingPlane()  { m_UseCuttingPlane = false; }

  bool IsCuttingPlaneDefined() const {return m_UseCuttingPlane;}
  
  const vnl_vector_fixed<double, VDimension> &GetCuttingPlanePoint() const
  { return m_CuttingPlanePoint; }
  const vnl_vector_fixed<double, VDimension> &GetCuttingPlaneNormal() const
  { return m_CuttingPlaneNormal; }


  /** Maintain a list of spheres within the domain.  These are used as 
      soft constraints by some particle forcing functions. */
  void AddSphere(const vnl_vector_fixed<double,VDimension> &v, double r)
  {
    if (r > 0)
    {
      m_SphereCenterList.push_back(v);
      m_SphereRadiusList.push_back(r);
    }
  }
  
  /** Returns the radius of sphere i, or 0.0 if sphere i does not exist */
  double GetSphereRadius(unsigned int i) const
  {
    if (m_SphereRadiusList.size() > i) return m_SphereRadiusList[i];
    else return 0.0;
  }

  /** Returns the center point of sphere i.  If sphere i does not exist,
      returns 0 vector. */
  vnl_vector_fixed<double, VDimension> GetSphereCenter(unsigned int i) const
  {
    if (m_SphereRadiusList.size() > i) return m_SphereCenterList[i];
    else
      {  return vnl_vector_fixed<double, VDimension>(0.0,0.0,0.0);  }
  }

  unsigned int GetNumberOfSpheres() const
  {
    return m_SphereCenterList.size();
  }
    
  
protected:
  ParticleImplicitSurfaceDomain() : m_Tolerance(1.0e-4), m_UseCuttingPlane(false)
    {
#ifdef SW_USE_MESH
    m_fim = new meshFIM;
    m_mesh = NULL;
#endif
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
  vnl_vector_fixed<double, VDimension> m_CuttingPlanePoint;
  vnl_vector_fixed<double, VDimension> m_CuttingPlaneNormal;
  
#ifdef SW_USE_MESH
  TriMesh *m_mesh;
  meshFIM *m_fim;
#endif
  
  std::vector< vnl_vector_fixed<double, VDimension> > m_SphereCenterList;
  std::vector< double > m_SphereRadiusList;
  
  ParticleImplicitSurfaceDomain(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticleImplicitSurfaceDomain+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticleImplicitSurfaceDomain.txx"
#endif

#endif
