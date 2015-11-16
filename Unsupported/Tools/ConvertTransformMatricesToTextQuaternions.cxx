/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ConvertTransformMatricesToTextQuaternions.cxx,v $
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
  unsigned int domains_per_shape;

  // READ PARAMETERS
  TiXmlDocument doc(argv[1]);
  bool loadOkay = doc.LoadFile();
  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;

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

   // Read the translations.
   object_reader< itk::ParticleSystem<3>::TransformType > transreader;
   transreader.SetFileName(trans_file.c_str());
   transreader.Update();
   
  // Read the rotation matrices
   object_reader< itk::ParticleSystem<3>::TransformType > rotreader;
   rotreader.SetFileName(rot_file.c_str());
   rotreader.Update();

   unsigned int num_shapes = rotreader.GetOutput().size() / domains_per_shape;
   std::cout << "num_shapes = " << num_shapes << std::endl;
   //   unsigned int 
   
   // Separate translation and rotation from matices, compute mean translation.
   // Translation comes from both files.
   std::vector< std::vector<QuaternionType> > poseLists(num_shapes);
   std::vector< VectorType > transList(num_shapes);
   std::vector< VectorType > rotTransList(num_shapes);
   std::vector< QuaternionType > rotList;
   for (unsigned int i = 0; i < transList.size(); i++) { transList[i].fill(0.0); }
   
   for (unsigned int i = 0; i < num_shapes * domains_per_shape; i++)
     {
     unsigned int idx = i % num_shapes;
     //     std::cout << "Translation " << i << std::endl;
     //     std::cout << transreader.GetOutput()[i] << std::endl;
//      transList[idx][0] += (transreader.GetOutput()[i])(0,3) -(rotreader.GetOutput()[i])(0,3);
//      transList[idx][1] += (transreader.GetOutput()[i])(1,3) -(rotreader.GetOutput()[i])(1,3);
//      transList[idx][2] += (transreader.GetOutput()[i])(2,3) -(rotreader.GetOutput()[i])(2,3);
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
     //     std::cout << tmpQ << std::endl;
     }

   for (unsigned int i = 0; i < domains_per_shape; i++)
     {
     std::cout << "STRUCTURE " << i << std::endl;
     for (unsigned int j = 0; j < num_shapes; j++)
       {
       std::cout << j << "\t" << poseLists[j][i] << std::endl;
       }
     }

   
   
   // Compute the means transl. and rotation
//    for (unsigned int i = 0; i < num_shapes; i++)
//      {
//      transList[i] /= (double)(num_samples);
//      rotTransList[i] /= (double)(num_samples);

//      QuaternionType mean;
//      stats.computeMean(poseLists[i], mean);
//      rotList.push_back(mean);
     
//      }



//    // Apply the translations and rotations
//   for (unsigned int i = 0; i < inputfiles.size(); i++)
//     {
//     std::vector< itk::ParticlePositionReader<3>::PointType > poslist;
//     itk::ParticlePositionReader<3>::PointType pos;
    
//     itk::ParticlePositionReader<3>::Pointer reader = itk::ParticlePositionReader<3>::New();
//     reader->SetFileName(inputfiles[i].c_str());
//     reader->Update();

//     //    std::cout << inputfiles[i] << std::endl;
//     //    std::cout << "Translation: " << transList[i] << std::endl;
//     //    std::cout << "Rotation (axis, angle):  " << rotList[i].axis()
//     //              << "\t" << rotList[i].angle() << std::endl;
    
//     for (unsigned int q = 0; q < reader->GetOutput().size(); q++)
//       {
//       VectorType pos;
//       pos[0] = reader->GetOutput()[q][0];
//       pos[1] = reader->GetOutput()[q][1];
//       pos[2] = reader->GetOutput()[q][2];

//       // Rotate 
//       pos = (rotList[i].inverse()).rotate(pos);
//       pos -= rotTransList[i];


//       // Scale
//       pos /= scales[i];
      
//       // Translate
//       pos[0] += transList[i][0];
//       pos[1] += transList[i][1];
//       pos[2] += transList[i][2];
      
//       PointType res;
//       res[0] = pos[0];
//       res[1] = pos[1];
//       res[2] = pos[2];
//       poslist.push_back( res );
//       }

//     itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
//     writer->SetFileName(outputfiles[i].c_str());
//     writer->SetInput(poslist);
//     writer->Update();
//     }
 
  return 0;
}
