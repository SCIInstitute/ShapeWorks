/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: FunctionValueComparison.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkParticlePositionWriter.h"
#include "itkParticlePositionReader.h"
#include <iostream>
#include <vector>

int main( int argc, char *argv[] )
{

  if (argc < 4)
    {
    std::cerr << "Use: " << argv[0] << " fileA fileB rotfileA" << std::endl;
    return 1;
    }

  std::vector<itk::ParticlePositionWriter<3>::PointType> outvec;
  itk::ParticlePositionWriter<3>::PointType pt;
  itk::ParticlePositionWriter<3>::Pointer writer =
    itk::ParticlePositionWriter<3>::New();
  writer->SetFileName(argv[3]);
  
  itk::ParticlePositionReader<3>::Pointer readerA
    = itk::ParticlePositionReader<3>::New();
  readerA->SetFileName(argv[1]);
  readerA->Update();

  itk::ParticlePositionReader<3>::Pointer readerB
    = itk::ParticlePositionReader<3>::New();
  readerB->SetFileName(argv[2]);
  readerB->Update();

  float xc = 64.0;
  float theta = -30.0/180.0 * 3.1415926535;
  float costheta = cos(theta);
  float sintheta = sin(theta);
  float x,y,z, xr, yr, zr, xb, yb, zb;
  float avg = 0.0;
  float dist = 0.0;
  for (unsigned int i = 0; i < readerA->GetOutput().size(); i++)
    {
    x = readerA->GetOutput()[i][0] - xc;
    y = readerA->GetOutput()[i][1] - xc;
    z = readerA->GetOutput()[i][2] - xc;

    xb = readerB->GetOutput()[i][0] - xc;
    yb = readerB->GetOutput()[i][1] - xc;
    zb = readerB->GetOutput()[i][2] - xc;
    
    xr = x * costheta - y * sintheta;
    yr = x * sintheta + y * costheta;
    zr = z;

    pt[0] = xr + xc;
    pt[1] = yr + xc;
    pt[2] = zr + xc;
    outvec.push_back(pt);
    
    dist = sqrt((xr - xb) * (xr - xb) + (yr - yb) * (yr - yb) + (zr - zb) * (zr - zb));

        
    std::cout << "(" << xr << ", " << yr << ", " << zr << ") ("
              << xb << ", " << yb << ", " << zb << ")"
              << " dist = " << dist << std::endl;

    
    avg += dist;
    }
  avg /= static_cast<float>(readerA->GetOutput().size());

  std::cout << "Average distance between the rotated x = " << avg << std::endl;

  writer->SetInput(outvec);
  writer->Update();
  
  return 0;
}
  


