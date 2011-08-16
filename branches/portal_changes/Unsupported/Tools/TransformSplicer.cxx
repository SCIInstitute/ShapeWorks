/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: TransformSplicer.cxx,v $
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
  //  std::string replacefile;
  std::vector< unsigned int> indices;

  // READ PARAMETERS
  param::parameterFile pf(argv[1]);

  PARAMSET(pf, input, "input", 0, ok, "");
  //  PARAMSET(pf, replacefile, "replacefile", 0, ok, "");
  PARAMSET(pf, output, "output", 0, ok, "");
  
   if (ok != true)
    {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
    }

   // Read the transforms
   object_reader< itk::ParticleSystem<3>::TransformType > rotreader;
   rotreader.SetFileName(input.c_str());
   rotreader.Update();
   
   // replace transforms
   int idx = 0;
   ok = true;

   for (unsigned int i = 0; ok == true; )
     {
     float tmp;
     ok = true;
     PARAMSET(pf, tmp, "replacements", i, ok, 0);
     if (ok != true) break;
     
     std::cout << "tmp = " << tmp << std::endl;
     idx = (int)tmp;
     itk::ParticleSystem<3>::TransformType T;
     i++;
     for (unsigned int j = 0; j < 4; j++)
       for (unsigned int k = 0; k < 4; k++, i++)
         {

         PARAMSET(pf, tmp, "replacements", i, ok, 0);
         T(j,k) = tmp;
         }
     
     std::cout << T << std::endl;
     rotreader.GetOutput()[idx] = T;
     
     }
   
   // Write the output
   object_writer< itk::ParticleSystem<3>::TransformType > rotwriter;
   rotwriter.SetFileName(output.c_str());
   rotwriter.SetInput(rotreader.GetOutput());
   rotwriter.Update();
   
  return 0;
}
