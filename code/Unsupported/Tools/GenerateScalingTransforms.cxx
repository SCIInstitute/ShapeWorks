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
#include "tinyxml.h"
#include <sstream>
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

  std::string output;
  std::vector<  itk::ParticleSystem<3>::TransformType > rots;

  // READ PARAMETERS
  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;

  //PARAMSET(pf, output, "output", 0, ok, "");
  output = "";
  elem = docHandle.FirstChild( "output" ).Element();
  if (elem)
    output = elem->GetText();
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }
  
  // Read the scales
  int i = 0;
  float s;
  elem = docHandle.FirstChild( "scales" ).Element();
  if (elem)
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> s)
    {
     itk::ParticleSystem<3>::TransformType rot;
     rot(0,0) = s;   rot(0,1) = 0.0; rot(0,2) = 0.0; rot(0,3) = 0.0;
     rot(1,0) = 0.0; rot(1,1) = s;   rot(1,2) = 0.0; rot(1,3) = 0.0;
     rot(2,0) = 0.0; rot(2,1) = 0.0; rot(2,2) = s;   rot(2,3) = 0.0;
     rot(3,0) = 0.0; rot(3,1) = 0.0; rot(3,2) = 0.0; rot(3,3) = 1.0;

     rots.push_back(rot);
     std::cout << i << std::endl << rot << std::endl << std::endl;
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  // Write the output
  object_writer< itk::ParticleSystem<3>::TransformType > rotwriter;
  rotwriter.SetFileName(output.c_str());
  rotwriter.SetInput(rots);
  rotwriter.Update();
   
  return 0;
}
