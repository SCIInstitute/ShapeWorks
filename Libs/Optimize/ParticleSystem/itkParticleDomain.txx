/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleDomain.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleDomain_txx
#define __itkParticleDomain_txx

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"
#define PARTICLE_DEBUG_FLAG 1

namespace itk
{

template<class T>
void
ParticleDomain<T>::
SetMesh(TriMesh *mesh)
{
  m_mesh = mesh;
}

template<class T>
void
ParticleDomain<T>::
SetFeaMesh(const char *feaFile)
{
    m_mesh->ReadFeatureFromFile(feaFile);
}

template<class T>
void
ParticleDomain<T>::
SetFeaGrad(const char *feaGradFile)
{
    m_mesh->ReadFeatureGradientFromFile(feaGradFile);
}

template<class T>
void
ParticleDomain<T>::
SetFids(const char *fidsFile)
{
    m_mesh->ReadFaceIndexMap(fidsFile);
    const typename ImageType::PointType orgn = this->GetOrigin();
    m_mesh->imageOrigin[0] = orgn[0];
    m_mesh->imageOrigin[1] = orgn[1];
    m_mesh->imageOrigin[2] = orgn[2];
    typename ImageType::RegionType::SizeType sz = this->GetSize();
    m_mesh->imageSize[0]   = sz[0];
    m_mesh->imageSize[1]   = sz[1];
    m_mesh->imageSize[2]   = sz[2];
    typename ImageType::SpacingType sp = this->GetSpacing();
    m_mesh->imageSpacing[0] = sp[0];
    m_mesh->imageSpacing[1] = sp[1];
    m_mesh->imageSpacing[2] = sp[2];
    typename ImageType::RegionType::IndexType idx = this->GetIndex();
    m_mesh->imageIndex[0]   = idx[0];
    m_mesh->imageIndex[1]   = idx[1];
    m_mesh->imageIndex[2]   = idx[2];
}


template<class T>
bool
ParticleDomain<T>::ApplyConstraints(PointType &p) const
{
  // First apply and constraints imposed by superclasses.  This will
  // guarantee the point starts in the correct image domain.
  bool flag = Superclass::ApplyConstraints(p);

  unsigned int k = 0;
  double mult = 1.0;

  const T epsilon = m_Tolerance * 0.001;
  T f = this->Sample(p);

  T gradmag = 1.0;
  while ( fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
    //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
    {
    vnl_vector_fixed<T, DIMENSION> grad = this->SampleGradientVnl(p);

    gradmag = grad.magnitude();
    vnl_vector_fixed<T, DIMENSION> vec = grad * (f / (gradmag + epsilon));
    for (unsigned int i = 0; i < DIMENSION; i++)
      {
      p[i] -= vec[i];
      }

    f = this->Sample(p);

    // Raise the tolerance if we have done too many iterations.
    k++;
    if (k > 10000)
      {
      mult *= 2.0;
      k = 0;
      }
    } // end while
    return flag;
}

template <class T>
double
ParticleDomain<T>::Distance(const PointType &a, const PointType &b) const
{
  if (m_mesh != NULL)
  {
    point p1;
    p1[0] = a[0];
    p1[1] = a[1];
    p1[2] = a[2];

    point p2;
    p2[0] = b[0];
    p2[1] = b[1];
    p2[2] = b[2];

    return ( m_mesh->GetGeodesicDistance(p1,p2) );
  }
}

} // end namespace
#endif
