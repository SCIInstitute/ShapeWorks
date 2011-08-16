/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ScalePoints.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// A simple utility that scales points by a specified value

#include <iostream>
#include <vector>
#include <string>
#include "param.h"
#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

 param::parameterFile pf(argv[1]);
 // Collect a list of point file names and output file names
  int i=0;
  std::vector< std::string > inputfiles;
  std::vector< std::string > outputfiles;
  std::vector< float > scales;
  std::string tmp;
  bool ok = true;
  while (ok == true)
    {
    // Record the point file names.
    PARAMSET(pf, tmp, "inputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      //      std::cout << i << "\t" << tmp <<  std::endl;
      inputfiles.push_back(tmp);
      } // if ok == true
    i++;
    } // while ok == true


  ok = true;
  i = 0;
  while (ok == true)
    {
    // Record the outputfiles
    PARAMSET(pf, tmp, "outputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No output files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      outputfiles.push_back(tmp);
      } // if ok == true
    i++;
    } // while ok == true


  // Read the scales
  ok = true;
  i = 0;
  while (ok == true)
    {
    float s;
    // Record the outputfiles
    PARAMSET(pf, s, "scales", i, ok, 0.0);
    if (i==0 && ok != true)
      {
      std::cerr << "No scales have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      scales.push_back(s);
      } // if ok == true
    i++;
    } // while ok == true
  

  if (scales.size() != outputfiles.size() || outputfiles.size() != inputfiles.size())
    {
    std::cerr << "Mismatch in number of paramters" << std::endl;
    return 2;
    }

  for (unsigned int p = 0; p < inputfiles.size(); p++)
    {
    std::vector< itk::ParticlePositionReader<3>::PointType > poslist;
    itk::ParticlePositionReader<3>::PointType pos;
    
    itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
    reader->SetFileName(inputfiles[p].c_str());
    reader->Update();
    std::cout << inputfiles[p] << std::endl;
    for (unsigned int q = 0; q < reader->GetOutput().size(); q++)
      {
      pos[0] = reader->GetOutput()[q][0] * scales[p];
      pos[1] = reader->GetOutput()[q][1] * scales[p];
      pos[2] = reader->GetOutput()[q][2] * scales[p];
      poslist.push_back( pos );
      }

    itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
    writer->SetFileName(outputfiles[p].c_str());
    writer->SetInput(poslist);
    writer->Update();
    }
    

  return 0;
}
