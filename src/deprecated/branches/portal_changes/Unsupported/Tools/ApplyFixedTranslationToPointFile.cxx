/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ApplyFixedTranslationToPointFile.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// A simple utility that translates points in a point file by a specific
// 3d translation x y z

#include <iostream>
#include <vector>
#include <string>
#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"

int main(int argc, char *argv[])
{
  if (argc != 6)
    {
    std::cerr << "Use: " << argv[0] << "input output x y z" << std::endl;
    return 1;
    }

  float x = atof(argv[3]);
  float y = atof(argv[4]);
  float z = atof(argv[5]);
  
  std::cout << "x = " << x << std::endl;
  std::cout << "y = " << y << std::endl;
  std::cout << "z = " << z << std::endl;  
  
  std::vector< itk::ParticlePositionReader<3>::PointType > poslist;
  itk::ParticlePositionReader<3>::PointType pos;
  
  itk::ParticlePositionReader<3>::Pointer reader
    = itk::ParticlePositionReader<3>::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  for (unsigned int q = 0; q < reader->GetOutput().size(); q++)
    {
    pos[0] = reader->GetOutput()[q][0] + x;
    pos[1] = reader->GetOutput()[q][1] + y;
    pos[2] = reader->GetOutput()[q][2] + z;
    poslist.push_back( pos );
    }
  
  itk::ParticlePositionWriter<3>::Pointer writer 
    = itk::ParticlePositionWriter<3>::New();
  writer->SetFileName(argv[2]);
  writer->SetInput(poslist);
  writer->Update();
  

  return 0;
}
