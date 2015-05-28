/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: FunctionOfPositionGenerator.cxx,v $
  Date:      $Date: 2009/05/06 21:49:16 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


int main(int argc, char *argv[])
{
  typedef itk::Image<float, 3> ImageType;
  
  if (argc != 4)
    {
    std::cerr << "Use: attribute rotated_attribute surface" << std::endl;
    return 1;
    }

  //  itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
  itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
  //  reader->SetFileName(argv[1]);
  //  reader->Update();

  //  const int xmax = reader->GetRequestedRegion()->GetSize(0);
  //  const int ymax = reader->GetRequestedRegion()->GetSize(1);
  //  const int zmax = reader->GetRequestedRegion()->GetSize(2);

  itk::Image<float,3>::Pointer img = itk::Image<float,3>::New();
  ImageType::RegionType reg;
  ImageType::RegionType::SizeType sz;
  ImageType::RegionType::IndexType idx;
  sz[0] = 128;   sz[1] = 128;   sz[2] = 128;
  idx[0] = 0; idx[1] = 0; idx[2] = 0;
  reg.SetSize(sz);
  reg.SetIndex(idx);
  img->SetRegions(reg);
  img->Allocate();

  itk::Image<float,3>::Pointer img2 = itk::Image<float,3>::New();
  img2->SetRegions(reg);
  img2->Allocate();

  itk::Image<float,3>::Pointer img3 = itk::Image<float,3>::New();
  img3->SetRegions(reg);
  img3->Allocate();
  

  float cx= 64.0;
  
  float x,y,z, px,py;
  
  float rotx;
  float theta = 30.0/180.0 * 3.1415926535;
  float costheta = cos(theta);
  float sintheta = sin(theta);

  
  float r = 55.0;
  
  for (idx[2] = 0; idx[2] < sz[2]; idx[2]++)
    {
    z = idx[2]-cx;
    for (idx[1] = 0; idx[1] < sz[1]; idx[1]++)
      {
      y = idx[1]-cx;
      for (idx[0] = 0; idx[0] < sz[0]; idx[0]++)
        {
        x = idx[0]-cx;

        float R = sqrt(x*x + y*y + z*z);
        
        // rotate
        px = x * r/((R+1.0e-10) + cx);
        py = y * r/((R+1.0e-10) + cx);

        rotx = px * costheta - py * sintheta;
        

        img->SetPixel(idx, px);
        img2->SetPixel(idx, rotx);
        img3->SetPixel(idx, R-r);
        
        }
      }
    }

  
  writer->SetInput(img);
  writer->SetFileName(argv[1]);
  writer->Update();

    writer->SetInput(img2);
  writer->SetFileName(argv[2]);
  writer->Update();

  writer->SetInput(img3);
  writer->SetFileName(argv[3]);
  writer->Update();

  return 0;
}
