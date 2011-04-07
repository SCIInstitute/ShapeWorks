/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: AffineTransformer.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
// A simple utility that performs an image transformation and point transformations.

#include <iostream>
#include <vector>
#include <string>
#include "itkParticleSystem.h"
#include "param.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main(int argc, char *argv[])
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef itk::Image<double, 3> image_type;
  
  if (argc != 2)
    {
    std::cerr << "Use: " << argv[0] << "paramfile" << std::endl;
    return 1;
    }

  param::parameterFile pf(argv[1]);
  std::string outputfile;

  // Collect a list of input file names
  std::vector< std::string > inputfiles;
  std::vector< std::string > outputfiles;
  std::string tmpa;
  std::string tmpb;

  int ii = 0;
  bool ok = true;  
  while (ok == true)
    {
    // Record the point file names.
    PARAMSET(pf, tmpa, "inputs", ii, ok, "");
    PARAMSET(pf, tmpb, "outputs", ii, ok, "");
    if (ii==0 && ok != true)
      {
      std::cerr << "No input/output files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      inputfiles.push_back(tmpa);
      outputfiles.push_back(tmpb);
      } // if ok == true
    ii++;
    } // while ok == true

  // Assume all domains have the same number of particles.
  const int numShapes = inputfiles.size();
  
  // Read the transforms
  ii = 0;
  std::vector<ParticleSystemType::TransformType> transform_params;
  for (int i = 0; i < numShapes; i++)
    { 
    // DO NOT DO SCALING
    //    transforms[i].scale = 1.0;
    //
    double tmp;

    ParticleSystemType::TransformType R;
    for (int k = 0; k < 4; k++)
      for (int j = 0; j < 4; j++)
        {
        PARAMSET(pf, tmp, "transforms", ii, ok, 0.0);
        R(k,j) = tmp;
        ii++;
        }
    transform_params.push_back(R);
    std::cout << R << std::endl;
    
    }

  // Apply transforms to each image

  for (int i =0; i < numShapes; i++)
    {

    itk::ImageFileReader<image_type>::Pointer reader = itk::ImageFileReader<image_type>::New();
    itk::ImageFileWriter<image_type>::Pointer writer = itk::ImageFileWriter<image_type>::New();
    reader->SetFileName(inputfiles[i].c_str());
    writer->SetFileName(outputfiles[i].c_str());

    reader->Update();
    
    itk::AffineTransform<double>::Pointer trans = itk::AffineTransform<double>::New();
    itk::AffineTransform<double>::MatrixType mat;
    
    for (int j = 0; j < 3; j++)
      for (int k = 0; k < 3; k++)
        {
        mat(j,k) = transform_params[i](j,k);
        }
    trans->SetMatrix(mat);
    
    //    itk::AffineTransform<double>::OutputVectorType axis;
    //    itk::AffineTransform<double>::OutputVectorType off;
    //    double angle;

    // Perform the transformation.
    itk::LinearInterpolateImageFunction<image_type,double>::Pointer
      interp1 = itk::LinearInterpolateImageFunction<image_type,double>::New();
    
    itk::NearestNeighborInterpolateImageFunction<image_type,double>::Pointer
      interp2 = itk::NearestNeighborInterpolateImageFunction<image_type,double>::New();
    
    itk::ResampleImageFilter<image_type, image_type>::Pointer resampler
      = itk::ResampleImageFilter<image_type, image_type>::New();
    resampler->SetOutputParametersFromImage( reader->GetOutput() );
    resampler->SetTransform(trans);
    resampler->SetInterpolator(interp2);
    resampler->SetInput( reader->GetOutput() );
    resampler->SetDefaultPixelValue( 0.0 );
    resampler->Update();
    
    writer->SetInput( resampler->GetOutput() );
    writer->Update();    
    }
  

//   // Now write all of the transformed points.
//   ParticleSystemType::PointType pos;
//   for (unsigned int i = 0; i < numShapes; i++)
//     {
//     std::vector< ParticleSystemType::PointType > pointlist;
//     pointlist.clear();

//     for (unsigned int k = 0; k < shapelist[i].size(); k++)
//       {
//       pos[0] = (shapelist[i][k])[0];
//       pos[1] = (shapelist[i][k])[1];
//       pos[2] = (shapelist[i][k])[2];

//       pointlist.push_back(pos);      
//       }

//     itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
//     writer->SetFileName(outputfiles[i].c_str());    
//     writer->SetInput(pointlist);
//     writer->Update();
//     }
  
  return 0;
}
