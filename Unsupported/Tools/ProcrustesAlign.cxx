/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ProcrustesAlign.cxx,v $
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
#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"
#include "Procrustes3D.h"
#include "itkParticleSystem.h"
#include "object_writer.h"
#include "tinyxml.h"
#include <sstream>

// Computes procrustes alignment for a set of point files and also reports the
// scaling


int main(int argc, char *argv[])
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;
  
  std::string outputfile;

  // Collect a list of input file names
  std::vector< std::string > inputfiles;
  std::vector< std::string > outputfiles;

  std::string tmpa;
  std::string tmpb;

  std::string transform_filename, scale_filename, quat_filename;

  //PARAMSET(pf, transform_filename, "transformfile", 0, ok, "transforms");
  transform_filename = "transforms";
  elem = docHandle.FirstChild( "transformfile" ).Element();
  if (elem) transform_filename = elem->GetText();

  elem = docHandle.FirstChild( "inputs" ).Element();
  if (!elem)
  {
    std::cerr << "No input files have been specified" << std::endl;
    throw 1;
  }
  else
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> tmpa)
    {
      inputfiles.push_back(tmpa);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  elem = docHandle.FirstChild( "outputs" ).Element();
  if (!elem)
  {
    std::cerr << "No output files have been specified" << std::endl;
    throw 1;
  }
  else
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> tmpb)
    {
      outputfiles.push_back(tmpb);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

 // Assume all domains have the same number of particles.
  const unsigned int numShapes = inputfiles.size();
  //  const int numPoints = 

  Procrustes3D::ShapeListType shapelist;
  Procrustes3D::ShapeListType shapelist_copy; // Because procrustes modifies its input
  Procrustes3D::ShapeType     shapevector;
  Procrustes3D::PointType     point;

  // Read input shapes from file list
  for(int d = 0; d < numShapes; d++)
    {
    shapevector.clear();
    itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
    reader->SetFileName(inputfiles[d].c_str());
    reader->Update();

    for(unsigned int j = 0; j < reader->GetOutput().size(); j++)
      {
      point(0) = (reader->GetOutput()[j])[0];
      point(1) = (reader->GetOutput()[j])[1];
      point(2) = (reader->GetOutput()[j])[2];
      
      shapevector.push_back(point);
      }
    shapelist.push_back(shapevector);
    shapelist_copy.push_back(shapevector);
    }

  // Run alignment
  Procrustes3D::SimilarityTransformListType transforms;
  Procrustes3D procrustes;
  procrustes.AlignShapes(transforms, shapelist);

  // Transform shapes
  Procrustes3D::TransformShapes(shapelist_copy, transforms);
  
  // Construct transform matrices for each particle system
  std::vector<ParticleSystemType::TransformType> transform_matrices;
  std::cout << "NUMSHAPES = " << numShapes << std::endl;
  for (int i = 0; i < numShapes; i++)
    { 
    // DO NOT DO SCALING
    //    transforms[i].scale = 1.0;
    //
    //    std::cout << "scale " << i << " = " << transforms[i].scale <<
    //    std::endl;
    
      std::cout << "--------------" << std::endl;
    std::cout << "Shape " << i << std::endl;
    std::cout << "Scale = " << 1.0 / transforms[i].scale << std::endl;
    std::cout << "translation = " << transforms[i].translation << std::endl;
    std::cout << "rotation = \n" 
	      << transforms[i].rotation(0,0) << "\t" << transforms[i].rotation(0,1) << "\t" << transforms[i].rotation(0,2) << "\n" 	         << transforms[i].rotation(1,0) << "\t" << transforms[i].rotation(1,1) << "\t" << transforms[i].rotation(1,2) << "\n" 
	      << transforms[i].rotation(2,0) << "\t" << transforms[i].rotation(2,1) << "\t" << transforms[i].rotation(2,2) << std::endl;

    ParticleSystemType::TransformType R;
    
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
        
    // std::cout << R(0,0) << "\t" << R(0,1) << "\t" << R(0,2) << "\t" << R(0,3) << std::endl;
    // std::cout << R(1,0) << "\t" << R(1,1) << "\t" << R(1,2) << "\t" << R(1,3) << std::endl;
    // std::cout << R(2,0) << "\t" << R(2,1) << "\t" << R(2,2) << "\t" << R(2,3) << std::endl;
    // std::cout << R(3,0) << "\t" << R(3,1) << "\t" << R(3,2) << "\t" << R(3,3) << std::endl;
    
    transform_matrices.push_back(R);
    }

  // Write transforms
  object_writer< ParticleSystemType::TransformType > transwriter;
  transwriter.SetFileName( transform_filename.c_str() );
  transwriter.SetInput(transform_matrices);
  transwriter.Update();

  // Now write all of the transformed points.
  ParticleSystemType::PointType pos;
  for (unsigned int i = 0; i < numShapes; i++)
    {
    std::vector< ParticleSystemType::PointType > pointlist;
    std::vector< ParticleSystemType::PointType > pointlist2;
    pointlist.clear();
    pointlist2.clear();

    for (unsigned int k = 0; k < shapelist_copy[i].size(); k++)
      {
      pos[0] = (shapelist_copy[i][k])[0];
      pos[1] = (shapelist_copy[i][k])[1];
      pos[2] = (shapelist_copy[i][k])[2];

      pointlist.push_back(pos);

      // DEBUG
      // Rotation by x degrees...
      // const double x = static_cast<double>(i) * 15.0 * (3.141592654 / 180.0);
      // ParticleSystemType::PointType tmp;
      // tmp[0] = cos(x) * pos[0] - sin(x) * pos[1];
      // tmp[1] = sin(x) * pos[0] + cos(x) * pos[1];
      // tmp[2] = pos[2];
      // pointlist2.push_back(tmp);
      //

      }

    itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
    writer->SetFileName(outputfiles[i].c_str());    
    writer->SetInput(pointlist);
    writer->Update();

    // itk::ParticlePositionWriter<3>::Pointer writer2 = itk::ParticlePositionWriter<3>::New();
    // writer2->SetFileName(outputfiles2[i].c_str());    
    // writer2->SetInput(pointlist2);
    // writer2->Update();
    }
  
  return 0;
}

