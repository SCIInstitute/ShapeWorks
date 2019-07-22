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
#include "itkPSMSurfaceNeighborhood.h"
#include "itkPoint.h"
#include "itkImage.h"
#include "itkMacro.h"


/** This test exercises functionality of PSMSurfaceNeighborhood class */
int itkPSMSurfaceNeighborhoodTest(int argc, char* argv[])
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

      // Test instantiation of the object
      itk::PSMSurfaceNeighborhood<ImageType>::Pointer P 
        = itk::PSMSurfaceNeighborhood<ImageType>::New();

      // Read image file
      itk::ImageFileReader<ImageType>::Pointer reader
        = itk::ImageFileReader<ImageType>::New();
      reader->SetFileName(argv[1]);
      reader->UpdateLargestPossibleRegion();

      // Setup Domain
      itk::PSMImplicitSurfaceDomain<float, 3>::Pointer domain
        = itk::PSMImplicitSurfaceDomain<float, 3>::New();
      domain->SetImage(reader->GetOutput());
      P->SetDomain(domain);

      // Get Bounds
      PointType ptl = domain->GetLowerBound();
      PointType ptu = domain->GetUpperBound();

      // Test AddPosition
      unsigned int SZ = 100;
      PointType pt;
      double range[3];
      double maxDist = 0.0;
      for (unsigned int i = 0; i < 3; i++)
      {
        range[i] = ptu[i]-ptl[i];
        maxDist += range[i]*range[i];
      }

      for (unsigned int i = 0; i < SZ; i++)
      {
        for (unsigned int j = 0; j < 3; j++) 
        {
          pt[j] = static_cast<double>(i)/static_cast<double>(SZ)*range[j]+ptl[j]+0.1;
        }
        P->AddPosition(pt, i);
      }

      for (unsigned int i = 0; i < 3; i++)
      {
        pt[i] = range[i]/2 + ptl[i];
      }
      itk::PSMSurfaceNeighborhood<ImageType>::PointVectorType vec;
      std::vector<double> weights;

      vec = P->FindNeighborhoodPointsWithWeights(pt, weights, maxDist);
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("AddPosition method failed. ");
      }

      // Test SetPosition
      P->SetPosition(pt, SZ/4);
      pt[0] += 0.1;
      vec = P->FindNeighborhoodPointsWithWeights(pt, weights, maxDist);
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("SetPosition method failed. ");
      }
      
      // Test FindNeighborhoodPoints
      vec = P->FindNeighborhoodPointsWithWeights(pt, weights, 0.5);
      if (vec.size() != 2)
      {
        passed = false;
        errstring += std::string("FindNeighborhoodPoints size failed. ");
      }
      else if ((vec[0].Index != SZ/2 && vec[1].Index != SZ/2) ||
               (vec[0].Index != SZ/4 && vec[1].Index != SZ/4))
      {
        passed = false;
        errstring += std::string("FindNeightborhoodPoints index failed. ");
      }

       // Test RemovePosition
      P->RemovePosition(SZ/4);
      vec = P->FindNeighborhoodPointsWithWeights(pt, weights, 0.5);
      if (vec.size() != 1)
      {
        passed = false;
        errstring += std::string("RemovePoints method failed. ");
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
