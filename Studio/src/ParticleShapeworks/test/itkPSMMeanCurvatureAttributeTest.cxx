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
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPSMMeanCurvatureAttribute.h"
#include "itkPSMSurfaceNeighborhood.h"
#include "itkMacro.h"

/** This test exercises functionality of the base itkPSMMeanCurvatureAttribute class */
int itkPSMMeanCurvatureAttributeTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";

  // Check for proper arguments
  if (argc < 2)
  {
    std::cout << "Wrong number of arguments.  You need one image file." << std::endl;
    return EXIT_FAILURE;
  }

  try
    {
      typedef itk::Point<double, 3> PointType;
      typedef itk::Image<float, 3> ImageType;

      itk::PSMMeanCurvatureAttribute<float, 3>::Pointer P 
        = itk::PSMMeanCurvatureAttribute<float,3>::New();

      // Read image file
      itk::ImageFileReader<ImageType>::Pointer reader
        = itk::ImageFileReader<ImageType>::New();
      reader->SetFileName(argv[1]);
      reader->UpdateLargestPossibleRegion();

      itk::PSMSurfaceNeighborhood<ImageType>::Pointer sn
        = itk::PSMSurfaceNeighborhood<ImageType>::New();

      // Setup Domain
      itk::PSMImageDomainWithCurvature<float, 3>::Pointer domain
        = itk::PSMImageDomainWithCurvature<float, 3>::New();
      domain->SetImage(reader->GetOutput());
      
      // Get Bounds
      PointType ptl = domain->GetLowerBound();
      PointType ptu = domain->GetUpperBound();

      itk::PSMParticleSystem<3>::Pointer ps =
        itk::PSMParticleSystem<3>::New();
      ps->RegisterAttribute(P);
      ps->AddDomain(domain);
      ps->SetNeighborhood(0, sn);

      // Add points
      unsigned int SZ = 100;
      PointType pt;
      double range[3] = { ptu[0]-ptl[0], ptu[1]-ptl[1], ptu[2]-ptl[2] };
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i)/static_cast<double>(SZ)*range[0]+ptl[0]+0.1;
        pt[1] = static_cast<double>(i)/static_cast<double>(SZ)*range[1]+ptl[1]+0.1;
        pt[2] = static_cast<double>(i)/static_cast<double>(SZ)*range[2]+ptl[2]+0.1;
        ps->AddPosition(pt, 0);
      }

      P->ComputeCurvatureStatistics(ps, 0);
      
      if (P->GetMeanCurvature(0) == 0.0f)
      {
        passed = false;
        errstring += std::string("GetMeanCurvature method failed. ");
      }
      if (P->GetCurvatureStandardDeviation(0) == 0.0f)
      {
        passed = false;
        errstring += std::string("GetCurvatureStandardDeviation method failed. ");
      }
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
