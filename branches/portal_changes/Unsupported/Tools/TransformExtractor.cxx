/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: TransformExtractor.cxx,v $
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
  std::string input;
  std::string output;
  std::vector< unsigned int> indices;
  

  // READ PARAMETERS
  param::parameterFile pf(argv[1]);

  PARAMSET(pf, input, "input", 0, ok, "");
  PARAMSET(pf, output, "output", 0, ok, "");
  
   if (ok != true)
    {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
    }

   // Collect a list of indices
   int i = 0;
   ok = true;
   unsigned int a;
   while (ok == true)
     {
     // Record the mean point file input and output names.
     PARAMSET(pf, a, "indices", i, ok, 0);
     if (i==0 && ok != true)
       {
       std::cerr << "No indices" << std::endl;
       throw 1;
       }
     if (ok == true)
       {
       indices.push_back(a);
       } // if ok == true
     i++;
     } // while ok == true

   
  // Read the rotation matrices
   object_reader< itk::ParticleSystem<3>::TransformType > rotreader;
   rotreader.SetFileName(input.c_str());
   rotreader.Update();

   std::vector< itk::ParticleSystem<3>::TransformType > outtrans;
   // Select the necessary transforms and compile the output.
   for (unsigned int i = 0; i < indices.size(); i++)
     {
     outtrans.push_back(rotreader.GetOutput()[indices[i]]);
     std::cout << rotreader.GetOutput()[indices[i]] << std::endl;
     }

std::cout << "aouttrans size = " << outtrans.size() << std::endl;
   // Write the output
   object_writer< itk::ParticleSystem<3>::TransformType > rotwriter;
   rotwriter.SetFileName(output.c_str());
   rotwriter.SetInput(outtrans);
   rotwriter.Update();
   
  return 0;
}
