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
#include "itkPSMRegionNeighborhood.h"
#include "itkPSMRegionDomain.h"
#include "itkPoint.h"
#include "itkMacro.h"

/** This test exercises functionality of PSMRegionNeighborhood class */
int itkPSMRegionNeighborhoodTest(int, char* [])
{
  bool passed = true;
  std::string errstring = "";
  // First try 3D
  try
    {
      typedef itk::Point<double, 3> PointType;
      // Test instantiation of the object
      itk::PSMRegionNeighborhood<3>::Pointer P = itk::PSMRegionNeighborhood<3>::New();
      itk::PSMRegionDomain<3>::Pointer domain = itk::PSMRegionDomain<3>::New();
      itk::PSMRegionNeighborhood<3>::PointVectorType vec;
      const unsigned int SZ = 100;
      PointType ptl;
      ptl[0] = 0.0f;
      ptl[1] = 0.0f;
      ptl[2] = 0.0f;
      PointType ptu;
      ptu[0] = static_cast<double>(SZ);
      ptu[1] = static_cast<double>(SZ);
      ptu[2] = static_cast<double>(SZ);
      domain->SetRegion(ptl, ptu);
      P->SetDomain(domain);

      // Test AddPosition
      PointType pt;
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i) + 0.1f;
        pt[1] = static_cast<double>(i) + 0.2f;
        pt[2] = static_cast<double>(i) + 0.3f;
        P->AddPosition(pt, i);
      }
      pt[0] = 0.0;
      pt[1] = 0.0;
      pt[2] = 0.0;
      vec = P->FindNeighborhoodPoints(pt, static_cast<double>(SZ*SZ*SZ));
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("AddPosition 3D method failed. ");
      }

      // Test SetPosition
      pt[0] = 1.1f;
      pt[1] = 1.2f;
      pt[2] = 1.3f;
      P->SetPosition(pt, SZ/2);
      pt[0] += 0.1;
      vec = P->FindNeighborhoodPoints(pt, static_cast<double>(SZ*SZ*SZ));
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("SetPosition 3D method failed. ");
      }

      // Test FindNeighborhoodPoints
      vec = P->FindNeighborhoodPoints(pt, 0.5);
      if (vec.size() != 2)
      {
        passed = false;
        errstring += std::string("FindNeighborhoodPoints 3D size failed . ");
      }
      else if ((vec[0].Index != 1 && vec[1].Index != 1) ||
               (vec[0].Index != SZ/2 && vec[1].Index != SZ/2))
      {
        passed = false;
        errstring += std::string("FindNeightborhoodPoints 3D index failed. ");

      }

      // Test RemovePosition
      P->RemovePosition(SZ/2);
      vec = P->FindNeighborhoodPoints(pt, 0.5);
      if (vec.size() != 1)
      {
        passed = false;
        errstring += std::string("RemovePoints 3D method failed. ");
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
  
  // Next try 2D
  try
    {
      typedef itk::Point<double, 2> PointType;
      // Test instantiation of the object
      itk::PSMRegionNeighborhood<2>::Pointer P = itk::PSMRegionNeighborhood<2>::New();
      itk::PSMRegionDomain<2>::Pointer domain = itk::PSMRegionDomain<2>::New();
      itk::PSMRegionNeighborhood<2>::PointVectorType vec;
      const unsigned int SZ = 100;
      PointType ptl;
      ptl[0] = 0.0f;
      ptl[1] = 0.0f;
      PointType ptu;
      ptu[0] = static_cast<double>(SZ);
      ptu[1] = static_cast<double>(SZ);
      domain->SetRegion(ptl, ptu);
      P->SetDomain(domain);

      // Test AddPosition
      PointType pt;
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i) + 0.1f;
        pt[1] = static_cast<double>(i) + 0.2f;
        P->AddPosition(pt, i);
      }
      pt[0] = 0.0;
      pt[1] = 0.0;
      vec = P->FindNeighborhoodPoints(pt, static_cast<double>(SZ*SZ));
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("AddPosition 2D method failed. ");
      }

      // Test SetPosition
      pt[0] = 1.1f;
      pt[1] = 1.2f;
      P->SetPosition(pt, SZ/2);
      pt[0] += 0.1;
      vec = P->FindNeighborhoodPoints(pt, static_cast<double>(SZ*SZ));
      if (vec.size() != SZ)
      {
        passed = false;
        errstring += std::string("SetPosition 2D method failed. ");
      }

      // Test FindNeighborhoodPoints
      vec = P->FindNeighborhoodPoints(pt, 0.5);
      if (vec.size() != 2)
      {
        passed = false;
        errstring += std::string("FindNeighborhoodPoints 2D size failed . ");
      }
      else if ((vec[0].Index != 1 && vec[1].Index != 1) ||
               (vec[0].Index != SZ/2 && vec[1].Index != SZ/2))
      {
        passed = false;
        errstring += std::string("FindNeightborhoodPoints 2D index failed. ");

      }

      // Test RemovePosition
      P->RemovePosition(SZ/2);
      vec = P->FindNeighborhoodPoints(pt, 0.5);
      if (vec.size() != 1)
      {
        passed = false;
        errstring += std::string("RemovePoints 2D method failed. ");
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
