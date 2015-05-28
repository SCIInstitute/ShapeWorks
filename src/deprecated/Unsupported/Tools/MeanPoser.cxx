/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: MeanPoser.cxx,v $
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
#include <string>
#include <iostream>
#include "itkParticleSystem.h"
#include "itkParticlePositionReader.h"
#include "itkParticlePositionWriter.h"
#include "RotationGeometry.hxx"
#include "RiemannianStatistics.hxx"
#include "object_reader.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  typedef itk::ParticleSystem<3>::PointType PointType;
  typedef RotationGeometry<double>::PointType QuaternionType;
  typedef RotationGeometry<double>::TangentType VectorType;
  RotationGeometry<double> geom;
  RiemannianStatistics<QuaternionType, VectorType, double> stats(&geom);
  
  std::string tmpa, tmpb, trans_file, rot_file;
  std::vector< std::string > inputfiles;
  std::vector< std::string > outputfiles;
  unsigned int domains_per_shape;

  // READ PARAMETERS
  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;

  unsigned int num_samples;
  //PARAMSET(pf, domains_per_shape, "domains_per_shape", 0, ok, 1);
  domains_per_shape = 1;
  elem = docHandle.FirstChild( "domains_per_shape" ).Element();
  if (elem)
    domains_per_shape = atoi(elem->GetText());
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }

  //PARAMSET(pf, num_samples, "number_samples", 0, ok, 1);
  num_samples = 1;
  elem = docHandle.FirstChild( "num_samples" ).Element();
  if (elem)
    num_samples = atoi(elem->GetText());
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }

  //PARAMSET(pf, trans_file, "local_translations", 0, ok, "");
  trans_file = "";
  elem = docHandle.FirstChild( "local_translations" ).Element();
  if (elem)
    trans_file = elem->GetText();
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }

  //PARAMSET(pf, rot_file, "transform_file", 0, ok, "");
  rot_file = "";
  elem = docHandle.FirstChild( "transform_file" ).Element();
  if (elem)
    rot_file = elem->GetText();
  else
  {
    std::cerr << "Missing parameters" << std::endl;
    return 1;
  }

  std::vector< double> scales;

  // Collect a list of point file names and output file names
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

  double s;
  elem = docHandle.FirstChild( "scales" ).Element();
  if (!elem)
  {
    std::cerr << "No scales have been specified" << std::endl;
    throw 1;
  }
  else
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> s)
    {
      scales.push_back(s);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  unsigned int num_shapes = inputfiles.size();
  unsigned int num_domains = num_samples * domains_per_shape;

  // Read the translations.
  object_reader< itk::ParticleSystem<3>::TransformType > transreader;
  transreader.SetFileName(trans_file.c_str());
  transreader.Update();

  // Read the rotation matrices
  object_reader< itk::ParticleSystem<3>::TransformType > rotreader;
  rotreader.SetFileName(rot_file.c_str());
  rotreader.Update();

  if (outputfiles.size() != num_shapes
     || transreader.GetOutput().size() != num_domains
     || rotreader.GetOutput().size() != num_domains)
   {
   std::cerr << "Mismatch in number of files and transforms" << std::endl;
   return 1;
   }
   
  // Separate translation and rotation from matices, compute mean translation.
  // Translation comes from both files.

  std::vector< std::vector<QuaternionType> > poseLists(num_shapes);
  std::vector< VectorType > transList(num_shapes);
  std::vector< VectorType > rotTransList(num_shapes);
  std::vector< QuaternionType > rotList;
  for (unsigned int i = 0; i < transList.size(); i++) { transList[i].fill(0.0); }
   
   for (unsigned int i = 0; i < num_domains; i++)
     {
     unsigned int idx = i % num_shapes;
     transList[idx][0] += (transreader.GetOutput()[i])(0,3);
     transList[idx][1] += (transreader.GetOutput()[i])(1,3);
     transList[idx][2] += (transreader.GetOutput()[i])(2,3);

     rotTransList[idx][0] += (rotreader.GetOutput()[i])(0,3);
     rotTransList[idx][1] += (rotreader.GetOutput()[i])(1,3);
     rotTransList[idx][2] += (rotreader.GetOutput()[i])(2,3);

     // Construct a quaternion from the rotation
     vnl_matrix_fixed<double, 3, 3> r;
     r(0,0) = (rotreader.GetOutput()[i])(0, 0);
     r(0,1) = (rotreader.GetOutput()[i])(0, 1);
     r(0,2) = (rotreader.GetOutput()[i])(0, 2);
     r(1,0) = (rotreader.GetOutput()[i])(1, 0);
     r(1,1) = (rotreader.GetOutput()[i])(1, 1);
     r(1,2) = (rotreader.GetOutput()[i])(1, 2);
     r(2,0) = (rotreader.GetOutput()[i])(2, 0);
     r(2,1) = (rotreader.GetOutput()[i])(2, 1);
     r(2,2) = (rotreader.GetOutput()[i])(2, 2);
     QuaternionType tmpQ(r);
     tmpQ.normalize();
     poseLists[idx].push_back(tmpQ);
     std::cout << tmpQ << std::endl;
     }

   // Compute the means transl. and rotation
   for (unsigned int i = 0; i < num_shapes; i++)
     {
     transList[i] /= (double)(num_samples);
     rotTransList[i] /= (double)(num_samples);

     QuaternionType mean;
     stats.computeMean(poseLists[i], mean);
     rotList.push_back(mean);
     }



   // Apply the translations and rotations
  for (unsigned int i = 0; i < inputfiles.size(); i++)
    {
    std::vector< itk::ParticlePositionReader<3>::PointType > poslist;
    itk::ParticlePositionReader<3>::PointType pos;
    
    itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
    reader->SetFileName(inputfiles[i].c_str());
    reader->Update();

    
    for (unsigned int q = 0; q < reader->GetOutput().size(); q++)
      {
      VectorType pos;
      pos[0] = reader->GetOutput()[q][0];
      pos[1] = reader->GetOutput()[q][1];
      pos[2] = reader->GetOutput()[q][2];

      // Rotate 
      pos = (rotList[i].inverse()).rotate(pos);
      pos -= rotTransList[i];


      // Scale
      pos /= scales[i];
      
      // Translate
      pos[0] += transList[i][0];
      pos[1] += transList[i][1];
      pos[2] += transList[i][2];
      
      PointType res;
      res[0] = pos[0];
      res[1] = pos[1];
      res[2] = pos[2];
      poslist.push_back( res );
      }

    itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
    writer->SetFileName(outputfiles[i].c_str());
    writer->SetInput(poslist);
    writer->Update();
    }
 
  return 0;
}

