/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkPSMProcrustesRegistration.cxx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.5 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkPSMProcrustesRegistration.h"

namespace itk {

template<unsigned int VDimension>
void
PSMProcrustesRegistration<VDimension>
::RunRegistration(int d)
{
  // Assume all domains have the same number of particles.
  const int totalDomains =  m_PSMParticleSystem->GetNumberOfDomains();
  const int numPoints =     m_PSMParticleSystem->GetNumberOfParticles(0);
  const int numShapes =     totalDomains / m_DomainsPerShape;
  //PSMProcrustesFunctionType typedefs
  ShapeListType shapelist;
  ShapeType     shapevector;
  PointType     point;
  
  // Read input shapes from file list
  for (int i = d % m_DomainsPerShape; i < totalDomains; i+=m_DomainsPerShape)
    {
    int j = 0;
    shapevector.clear();
    for (j = 0; j < numPoints; j++)
      {
        for (unsigned int k = 0; k < VDimension; k++)
          {
          point(k) = m_PSMParticleSystem->GetPosition(j,i)[k];
          }
      shapevector.push_back(point);
      }
    shapelist.push_back(shapevector);
    }
    
  // Run alignment
  SimilarityTransformListType transforms;
  
  typename PSMProcrustesFunctionType::Pointer procrustes = PSMProcrustesFunctionType::New();
  procrustes->RunGeneralizedProcrustes(transforms, shapelist);
  // Construct transform matrices for each particle system.
  int k = d % m_DomainsPerShape;
  for (int i = 0; i < numShapes; i++, k += m_DomainsPerShape)
    {
      // Transform from Configuration space to Procrustes space. Translation
      // followed by rotation and scaling.    
      TransformType R;  //PSMParticleSystemType typedef
      if( VDimension == 3 )
        {
          if (m_Scaling == true)
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
          else // do not do scaling
          {
              R(0,0) =  transforms[i].rotation(0,0);
              R(1,0) =  transforms[i].rotation(1,0);
              R(2,0) =  transforms[i].rotation(2,0);
              R(3,0) =  0.0;
              
              R(0,1) =  transforms[i].rotation(0,1);
              R(1,1) =  transforms[i].rotation(1,1);
              R(2,1) =  transforms[i].rotation(2,1);
              R(3,1) =  0.0;
              
              R(0,2) =  transforms[i].rotation(0,2);
              R(1,2) =  transforms[i].rotation(1,2);
              R(2,2) =  transforms[i].rotation(2,2);
              R(3,2) =  0.0;
              
              R(0,3) =  transforms[i].translation(0) * R(0,0) + transforms[i].translation(1) * R(0,1) + transforms[i].translation(2) * R(0,2);
              R(1,3) =  transforms[i].translation(0) * R(1,0) + transforms[i].translation(1) * R(1,1) + transforms[i].translation(2) * R(1,2);
              R(2,3) =  transforms[i].translation(0) * R(2,0) + transforms[i].translation(1) * R(2,1) + transforms[i].translation(2) * R(2,2);
              R(3,3) =  1.0;
          }

          m_PSMParticleSystem->SetTransform(k, R);
          //          std::cout << "R" << std::endl;
          //          std::cout << R << std::endl;
          //          std::cout << std::endl;
        }
      else if( VDimension == 2 )
        {
        if (m_Scaling == true)
          {
              R(0,0) =  transforms[i].rotation(0,0) * transforms[i].scale;
              R(1,0) =  transforms[i].rotation(1,0) * transforms[i].scale;
              R(2,0) =  0.0;
              
              R(0,1) =  transforms[i].rotation(0,1) * transforms[i].scale;
              R(1,1) =  transforms[i].rotation(1,1) * transforms[i].scale;
              R(2,1) =  0.0;
              
              R(0,2) =  transforms[i].translation(0) * R(0,0) + transforms[i].translation(1) * R(0,1);
              R(1,2) =  transforms[i].translation(0) * R(1,0) + transforms[i].translation(1) * R(1,1);
              R(2,2) =  1.0;
          }
        else // do not do scaling
          {
              R(0,0) =  transforms[i].rotation(0,0);
              R(1,0) =  transforms[i].rotation(1,0);
              R(2,0) =  0.0;
              
              R(0,1) =  transforms[i].rotation(0,1);
              R(1,1) =  transforms[i].rotation(1,1);
              R(2,1) =  0.0;
              
              R(0,2) =  transforms[i].translation(0) * R(0,0) + transforms[i].translation(1) * R(0,1);
              R(1,2) =  transforms[i].translation(0) * R(1,0) + transforms[i].translation(1) * R(1,1);
              R(2,2) =  1.0;
          }
        
        m_PSMParticleSystem->SetTransform(k, R);
        //        std::cout << "R" << std::endl;
        //        std::cout << R << std::endl;
        //        std::cout << std::endl;
        }
    }  // end for loop
} // end RunRegistration
  // Explicitly instantiate the function for 3D and 2D
  template void PSMProcrustesRegistration<3>::RunRegistration(int d);
  template void PSMProcrustesRegistration<2>::RunRegistration(int d);
} // end namespace itk
