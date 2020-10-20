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
#define PARTICLE_DEBUG_FLAG 1

namespace itk
{

template<class T>
void
ParticleImplicitSurfaceDomain<T>::
SetMesh(TriMesh *mesh)
{
  m_mesh = new meshFIM();
  m_mesh->SetMesh(mesh);
}

template<class T>
void
ParticleImplicitSurfaceDomain<T>::
SetFeaMesh(const char *feaFile)
{
    m_mesh->ReadFeatureFromFile(feaFile);
}

template<class T>
void
ParticleImplicitSurfaceDomain<T>::
SetFeaGrad(const char *feaGradFile)
{
    m_mesh->ReadFeatureGradientFromFile(feaGradFile);
}

template<class T>
void
ParticleImplicitSurfaceDomain<T>::
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
ParticleImplicitSurfaceDomain<T>::ApplyConstraints(PointType &p, bool dbg) const
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
    PointType p_old = p;
    //vnl_vector_fixed<T, DIMENSION> grad = -this->SampleGradientAtPoint(p);
    vnl_vector_fixed<T, DIMENSION> gradf = this->SampleGradientAtPoint(p);
    vnl_vector_fixed<double, DIMENSION> grad;
    grad[0] = double(gradf[0]); grad[1] = double(gradf[1]); grad[2] = double(gradf[2]);


    gradmag = grad.magnitude();
    //vnl_vector_fixed<T, DIMENSION> vec = grad * (f / (gradmag + epsilon));
    vnl_vector_fixed<double, DIMENSION> vec = grad * (double(f) / (gradmag + double(epsilon)));

    vnl_vector_fixed<double, DIMENSION> vec_old = vec;
    std::stringstream msg = this->GetConstraints()->applyBoundaryConstraints(vec, p);
    if(dbg){
        msg << std::endl;
        std::cout << msg.str();
    }

    for (unsigned int i = 0; i < DIMENSION; i++)
      {
      p[i] -= vec[i];
      }

    f = this->Sample(p);

    if(!this->GetConstraints()->IsAnyViolated(p_old) && this->GetConstraints()->IsAnyViolated(p)){
        msg << std::endl << "####### Violation within apply constraints #######" << p_old << p  << std::endl;
        msg << "f " << f << " epsilon " << epsilon << std::endl;
        msg << "vec_old " << vec_old << " vec " << vec << std::endl;
        msg << std::endl << std::endl;
        std::cerr << msg.str();
    }
    if(vec[0] == 0 && vec[1] == 0 && vec[2] == 0){
        std::cerr << "Stuck ";
    }

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


} // end namespace
#endif
