/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#include <iostream>
#include "itkMacro.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkPSMImplicitSurfaceImageFilter.h"

/** This test exercises functionality of the base itkPSMTrimLabelMapImageFilter class */
int itkPSMImplicitSurfaceImage2DFilterTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";

  // Check for proper arguments
  if (argc < 3)
    {
      std::cout << "Wrong number of arguments.  You need at least an input file name and output file name. \nUse: " 
                << "itkPSMImplicitSurfaceImageFilterTest InputImage OutputImage [blurring_sigma] [isosurface_value]" << std::endl;
      return EXIT_FAILURE;
    }

  typedef itk::Image<unsigned char, 2> InputImageType;
  typedef itk::Image<float, 2>        OutputImageType;

  double sigma = 0.0;
  double isosurface_value = 0.5;
  if (argc >3)
   {  sigma = atof(argv[3]);  }
  if (argc >4)
    { isosurface_value = atof(argv[4]);  }

  try
    {
     // Read image file
     itk::ImageFileReader<InputImageType>::Pointer reader 
       = itk::ImageFileReader<InputImageType>::New(); 
     reader->SetFileName(argv[1]);
     
      // The Filter
     itk::PSMImplicitSurfaceImageFilter<InputImageType, OutputImageType>::Pointer P 
       = itk::PSMImplicitSurfaceImageFilter<InputImageType, OutputImageType>::New();
     P->SetSmoothingSigma(sigma);
     P->SetIsosurfaceValue(isosurface_value);
     P->SetInput(reader->GetOutput());
     
      // Write the output file to trigger the pipeline
     itk::ImageFileWriter<OutputImageType>::Pointer writer
       = itk::ImageFileWriter<OutputImageType>::New();
     writer->SetInput(P->GetOutput());
     writer->SetFileName(argv[2]);
     writer->Update();
    }
  catch(itk::ExceptionObject &e)
    {
      errstring = "ITK exception with description: " + std::string(e.GetDescription())
        + std::string("\n at location:") + std::string(e.GetLocation())
        + std::string("\n in file:") + std::string(e.GetFile());
      passed = false;
    }
  catch(...)
    {
      errstring = "Unknown exception thrown";
      passed = false;
    }

  if (passed)
    {
    std::cout << "All tests passed" << std::endl;
    return EXIT_SUCCESS;
    }
  else
    {
    std::cout << "Test failed with the following error:" << std::endl;
    std::cout << errstring << std::endl;
    return EXIT_FAILURE;
    }
}

