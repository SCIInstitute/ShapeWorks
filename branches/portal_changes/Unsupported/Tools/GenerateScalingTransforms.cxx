/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: GenerateScalingTransforms.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include <iostream>
#include <vector>
#include <string>
#include "param.h"
#include "itkParticleSystem.h"
#include "object_reader.h"
#include "object_writer.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  bool ok = true;
  std::string output;
  std::vector<  itk::ParticleSystem<3>::TransformType > rots;

  // READ PARAMETERS
  param::parameterFile pf(argv[1]);

  //  PARAMSET(pf, input, "input", 0, ok, "");
  //  PARAMSET(pf, replacefile, "replacefile", 0, ok, "");
  PARAMSET(pf, output, "output", 0, ok, "");
  
   if (ok != true)
    {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
    }

   // Read the scales
   ok = true;
   int i = 0;
   float s;
   std::vector<float> scales;
   while (ok == true)
     {     
     PARAMSET(pf, s, "scales", i, ok, 1.0);

     itk::ParticleSystem<3>::TransformType rot;
     rot(0,0) = s;   rot(0,1) = 0.0; rot(0,2) = 0.0; rot(0,3) = 0.0;
     rot(1,0) = 0.0; rot(1,1) = s;   rot(1,2) = 0.0; rot(1,3) = 0.0;
     rot(2,0) = 0.0; rot(2,1) = 0.0; rot(2,2) = s;   rot(2,3) = 0.0;
     rot(3,0) = 0.0; rot(3,1) = 0.0; rot(3,2) = 0.0; rot(3,3) = 1.0;
     
     if (ok == true)
       {
       rots.push_back(rot);
       std::cout << i << std::endl << rot << std::endl << std::endl;
       }
     i++;
     }

   // Write the output
   object_writer< itk::ParticleSystem<3>::TransformType > rotwriter;
   rotwriter.SetFileName(output.c_str());
   rotwriter.SetInput(rots);
   rotwriter.Update();
   
  return 0;
}
