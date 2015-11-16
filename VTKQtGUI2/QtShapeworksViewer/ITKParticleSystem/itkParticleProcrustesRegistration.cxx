/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleProcrustesRegistration.cxx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.5 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkParticleProcrustesRegistration.h"
#include "Procrustes3D.h"

namespace itk {

template<>
void
ParticleProcrustesRegistration<3>::RunRegistration(int d)
{
  // Assume all domains have the same number of particles.
  const int totalDomains = m_ParticleSystem->GetNumberOfDomains();
  const int numPoints = m_ParticleSystem->GetNumberOfParticles(0);
  const int numShapes = totalDomains / m_DomainsPerShape;

  Procrustes3D::ShapeListType shapelist;
  Procrustes3D::ShapeType     shapevector;
  Procrustes3D::PointType     point;

  // Read input shapes from file list
  for(int i = d % m_DomainsPerShape; i < totalDomains; i+=m_DomainsPerShape)
    {
    shapevector.clear();
    for(int j = 0; j < numPoints; j++)
      {
      point(0) = m_ParticleSystem->GetPosition(j,i)[0];
      point(1) = m_ParticleSystem->GetPosition(j,i)[1];
      point(2) = m_ParticleSystem->GetPosition(j,i)[2];
      
      shapevector.push_back(point);
      }
    shapelist.push_back(shapevector);
    }

  // Run alignment
  Procrustes3D::SimilarityTransformListType transforms;
  Procrustes3D procrustes;
  procrustes.AlignShapes(transforms, shapelist);

  // Construct transform matrices for each particle system.
  int k = d % m_DomainsPerShape;
  //    double avgscaleA = 1.0;
  //    double avgscaleB = 1.0;
  for (int i = 0; i < numShapes; i++, k += m_DomainsPerShape)
    {
    if (m_Scaling == false)
      {
      // DO NOT DO PROCRUSTES SCALING.   
      // If the user supplied some scales
      if (m_FixedScales.size() != 0)
        {
        transforms[i].scale = m_FixedScales[i];
        std::cout << "Fixed scale " << i << " = " << m_FixedScales[i] << std::endl;
        }
      else // otherwise do not scale at all
        {
        transforms[i].scale = 1.0;
        }
      }
    // else
    //  {
    //   DEBUG
    //   std::cout << transforms[i].scale << std::endl;
    //  }
    //  if (i < 15) avgscaleA *= transforms[i].scale;
    //  if (i >= 15) avgscaleB *= transforms[i].scale;
    
    // Transform from Configuration space to Procrustes space.  Translation
    // followed by rotation and scaling.
    
    ParticleSystemType::TransformType R;

    if (m_RotationTranslation == true)
      {
      R(0,0) =  transforms[i].rotation(0,0) * transforms[i].scale;
      R(1,0) =  transforms[i].rotation(1,0) * transforms[i].scale;
      R(2,0) =  transforms[i].rotation(2,0) * transforms[i].scale;
      R(3,0) =  0.0;
      
      R(0,1) =  transforms[i].rotation(0,1) * transforms[i].scale;
      R(1,1) =  transforms[i].rotation(1,1) * transforms[i].scale;
      R(2,1) =  transforms[i].rotation(2,1) * transforms[i].scale;
      R(3,1) =  0.0;
      
      R(0,2) =  transforms[i].rotation(0,2) * transforms[i].scale;
      R(1,2) =  transforms[i].rotation(1,2) * transforms[i].scale;
      R(2,2) =  transforms[i].rotation(2,2) * transforms[i].scale;
      R(3,2) =  0.0;
      
      R(0,3) =  transforms[i].translation(0) * R(0,0) + transforms[i].translation(1) * R(0,1) + transforms[i].translation(2) * R(0,2);
      R(1,3) =  transforms[i].translation(0) * R(1,0) + transforms[i].translation(1) * R(1,1) + transforms[i].translation(2) * R(1,2);
      R(2,3) =  transforms[i].translation(0) * R(2,0) + transforms[i].translation(1) * R(2,1) + transforms[i].translation(2) * R(2,2);
      R(3,3) =  1.0;
      }
    else // only use the scaling (could just be 1.0 depending on m_Scaling value)
      {
      R(0,0) =  transforms[i].scale;
      R(1,0) =  0.0;
      R(2,0) =  0.0;
      R(3,0) =  0.0;
      
      R(0,1) =  0.0;
      R(1,1) =  transforms[i].scale;
      R(2,1) =  0.0;
      R(3,1) =  0.0;
      
      R(0,2) =  0.0;
      R(1,2) =  0.0;
      R(2,2) =  transforms[i].scale;
      R(3,2) =  0.0;
      
      R(0,3) =  0.0;
      R(1,3) =  0.0;
      R(2,3) =  0.0;
      R(3,3) =  1.0;
      
      }

    m_ParticleSystem->SetTransform(k, R);


    std::cout << R << std::endl;
    std::cout << std::endl;
    
    }  
}


} // end namespace
