/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleImplicitSurfaceDomain.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleImplicitSurfaceDomain_txx
#define __itkParticleImplicitSurfaceDomain_txx

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"
//#define PARTICLE_DEBUG_FLAG 0

namespace itk
{

template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetCuttingPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                const vnl_vector<double> &c)
{
  // See http://mathworld.wolfram.com/Plane.html, for example
  vnl_vector<double> q;
  if (VDimension == 3)  q = vnl_cross_3d((b-a),(c-a));
  else if (VDimension == 2)  q = vnl_cross_2d((b-a),(c-a));
  
  if (q.magnitude() > 0.0)
  {
    m_CuttingPlaneNormal = q / q.magnitude();
    m_CuttingPlanePoint = a;
    m_UseCuttingPlane = true;
  }
}

#ifdef SW_USE_MESH
template<class T, unsigned int VDimension>
void
ParticleImplicitSurfaceDomain<T, VDimension>::
SetMesh(TriMesh *mesh, const char *gfile)
{
  m_mesh = mesh;

  m_mesh->need_faces();	
	m_mesh->need_neighbors();
	
  orient(m_mesh);

  // Recompute values that are no longer correct
  if (!m_mesh->normals.empty()) m_mesh->normals.clear();
  m_mesh->need_normals();

	if (!m_mesh->adjacentfaces.empty()) m_mesh->adjacentfaces.clear();
	m_mesh->need_adjacentfaces();

	if (!m_mesh->across_edge.empty()) m_mesh->across_edge.clear();
	  m_mesh->need_across_edge();

  if (!m_mesh->tstrips.empty()) m_mesh->tstrips.clear();
  m_mesh->need_tstrips();

  m_mesh->need_faceedges();
  m_mesh->need_oneringfaces();
  m_mesh->need_abs_curvatures();
  m_mesh->need_speed();
  m_mesh->setSpeedType(1);

  double d = m_mesh->bsphere.r * PI * 2.0f * 0.05f;
  m_fim->SetStopDistance(d);

  m_fim->loadGeodesicFile(m_mesh, gfile);
}
#endif

template<class T, unsigned int VDimension>
bool
ParticleImplicitSurfaceDomain<T, VDimension>::
ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                       const PointType &pos,
                       double maxtimestep) const
{

  // NOTE --- DISABLED
  return Superclass::ApplyVectorConstraints(gradE,pos,maxtimestep);
  // END --- DISABLED
  
  if (this->m_UseCuttingPlane == true)
    {    
    // See http://mathworld.wolfram.com/Point-PlaneDistance.html, for example
    vnl_vector_fixed<double, 3> x;
    vnl_vector_fixed<T, VDimension> grad = this->SampleGradientVnl(pos);
    for (unsigned int i = 0; i < VDimension; i++)
      { x[i] = pos[i]; }
    const double D = dot_product(m_CuttingPlaneNormal, x- m_CuttingPlanePoint);
    
    //    x = m_CuttingPlaneNormal * fabs(1.0 / (D + 1.0e-3));

    // x = m_CuttingPlaneNormal * lambda * exp(-lambda * fabs(D));
    
    // Gradient of simple force 1/D^2 to push away from cutting plane
    vnl_vector_fixed<double, VDimension> df;
    const double k = (-2.0 / (D * D * D));
    df[0] = k * grad[0] * m_CuttingPlaneNormal[0];
    df[1] = k * grad[1] * m_CuttingPlaneNormal[1];
    df[2] = k * grad[2] * m_CuttingPlaneNormal[2];

    gradE = gradE + df;

    // Make sure force is not huge relative to other forces.
    if (gradE.magnitude() > maxtimestep)
      {
      gradE.normalize();
      gradE = gradE * maxtimestep;
      }    
    }
  
  return Superclass::ApplyVectorConstraints(gradE,pos,maxtimestep);
}



template<class T, unsigned int VDimension>
bool
ParticleImplicitSurfaceDomain<T, VDimension>::ApplyConstraints(PointType &p) const
{
  // First apply and constraints imposed by superclasses.  This will
  // guarantee the point starts in the correct image domain.
  bool flag = Superclass::ApplyConstraints(p);

  if (this->m_ConstraintsEnabled == true)
    {
  
    unsigned int k = 0;
    double mult = 1.0;
    
    const T epsilon = m_Tolerance * 0.001;
    T f = this->Sample(p);
    
    T gradmag = 1.0;
    while ( fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
      //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
      {
      vnl_vector_fixed<T, VDimension> grad = this->SampleGradientVnl(p);
      
      gradmag = grad.magnitude();
      vnl_vector_fixed<T, VDimension> vec   =  grad  * ( f / (gradmag + epsilon) );
      for (unsigned int i = 0; i < VDimension; i++)
        {
        p[i] -= vec[i];
        }
      
#ifdef  PARTICLE_DEBUG_FLAG
      if ( ! this->IsInsideBuffer(p) )
        {
      itkExceptionMacro("A Point, " << p << ", was projected outside the given image domain." );
        }
#endif  
      
      f = this->Sample(p);
      
#ifdef  PARTICLE_DEBUG_FLAG
      if ( gradmag < epsilon && fabs(f) > m_Tolerance)
        {
        itkExceptionMacro("Newton-Raphson iteration failed to find the zero level-set.  Gradient is zero, but f = "  <<  f );
        }
#endif
      
      // Raise the tolerance if we have done too many iterations.
      k++;
      if (k > 10000)
        {
        mult *= 2.0;
        k = 0;
        }
      } // end while
    } // end if m_ConstraintsEnabled == true

  return flag; 
}

template <class T, unsigned int VDimension>
double
ParticleImplicitSurfaceDomain<T, VDimension>::Distance(const PointType &a, const PointType &b) const
{
  // Check to see if the normals are >= 90 degrees apart.
  //if ( dot_product(this->SampleGradientVnl(a), this->SampleGradientVnl(b) ) <= 0.0  )
  //{
  //  return -1.0;
  //}
  //else  // Return distance
  //  {
#ifdef SW_USE_MESH
    if (m_mesh != NULL)
      {
      point pt;
      pt[0] = a[0]; // + (this->GetImage()->GetOrigin())[0];
      pt[1] = a[1]; // + (this->GetImage()->GetOrigin())[1];
      pt[2] = a[2]; // + (this->GetImage()->GetOrigin())[2];
      int v1 = m_mesh->FindNearestVertex(pt);
      
      pt[0] = b[0]; // + (this->GetImage()->GetOrigin())[0];
      pt[1] = b[1]; // + (this->GetImage()->GetOrigin())[1];
      pt[2] = b[2]; // + (this->GetImage()->GetOrigin())[2];
      int v2 = m_mesh->FindNearestVertex(pt);
      
      return (m_mesh->GetGeodesicDistance(v1,v2));
      }
    else
      {
#endif
      double sum = 0.0;
      for (unsigned int i = 0; i < VDimension; i++)
        {      sum += (b[i]-a[i]) * (b[i]-a[i]);      }
      return sqrt(sum);
#ifdef SW_USE_MESH
      }
#endif
    //}
}

} // end namespace
#endif
