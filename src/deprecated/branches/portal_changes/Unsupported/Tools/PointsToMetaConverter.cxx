/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: PointsToMetaConverter.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// A simple utility to convert a points file into a meta data object file.

#include <iostream>
#include <vector>
#include <string>
#include "itkParticlePositionReader.h"
#include "itkParticlePositionMetaWriter.h"

int main(int argc, char *argv[])
{
  if (argc != 3)
    {
    std::cerr << "Use: " << argv[0] << "input output" << std::endl;
    return 1;
    }

  itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
  reader->SetFileName(argv[1]);

  itk::ParticlePositionMetaWriter<3>::Pointer writer = itk::ParticlePositionMetaWriter<3>::New();
  writer->SetFileName(argv[2]);

  reader->Update();

  writer->SetInput(reader->GetOutput());
  writer->Update();


  return 0;
}
