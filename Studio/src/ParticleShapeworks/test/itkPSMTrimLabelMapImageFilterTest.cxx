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
#include "itkPSMTrimLabelMapImageFilter.h"

/** This test exercises functionality of the base itkPSMTrimLabelMapImageFilter class */
int itkPSMTrimLabelMapImageFilterTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";

  // Check for proper arguments
  if (argc < 3)
    {
      std::cout << "Wrong number of arguments.  You need at least two image segmentation files. \nUse: " 
                << "itkPSMTrimLabelMapImageFilterTest InputImage OutputImage " << std::endl;
      return EXIT_FAILURE;
    }

  typedef itk::Image<unsigned char, 3> ImageType;

  try
    {
      // Create the filter
     itk::PSMTrimLabelMapImageFilter<ImageType>::Pointer P 
       = itk::PSMTrimLabelMapImageFilter<ImageType>::New();
     P->SetBackgroundValue(0);
     P->SetForegroundValue(1);
     
     // Read image file
     itk::ImageFileReader<ImageType>::Pointer reader 
       = itk::ImageFileReader<ImageType>::New();
     std::cout << "Reading input file " << argv[1] << std::endl;
     
     reader->SetFileName(argv[1]);
     reader->Update();
     
     P->SetInput(reader->GetOutput());
     P->Update();
     
     std::cout << "Bounding Box Region is:" << std::endl;
     std::cout << P->GetBoundingBox() << std::endl;

     std::cout << "Origin is: " << std::endl;
     std::cout << P->GetOutput()->GetOrigin() << std::endl;

     // Write the output file
     itk::ImageFileWriter<ImageType>::Pointer writer
       = itk::ImageFileWriter<ImageType>::New();

     std::cout << "Writing output file " << argv[2] << std::endl;
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

