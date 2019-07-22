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
#include "itkPSMParticleSystem.h"
#include "itkPSMRegionNeighborhood.h"
#include "itkPSMRegionDomain.h"
#include "itkMacro.h"

/** This test exercises functionality of the base itkPSMParticleSystem class */
int itkPSMParticleSystemTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";

  try
    {
      typedef itk::Point<double, 3> PointType;
      itk::PSMParticleSystem<3>::Pointer P = itk::PSMParticleSystem<3>::New();
      itk::PSMRegionNeighborhood<3>::Pointer nb1 = itk::PSMRegionNeighborhood<3>::New();
      itk::PSMRegionNeighborhood<3>::Pointer nb2 = itk::PSMRegionNeighborhood<3>::New();
      itk::PSMRegionDomain<3>::Pointer d1 = itk::PSMRegionDomain<3>::New();
      itk::PSMRegionDomain<3>::Pointer d2 = itk::PSMRegionDomain<3>::New();
      
      // Define bounding box
      const unsigned int SZ = 100;
      PointType ptl, ptu;
      ptl[0] = 0.0f; ptl[1] = 0.0f; ptl[2] = 0.0f;
      ptu[0] = static_cast<double>(SZ); ptu[1] = static_cast<double>(SZ); ptu[2] = static_cast<double>(SZ);
      
      // Add domains and neighborhoods
      d1->SetRegion(ptl, ptu);
      d2->SetRegion(ptl, ptu);
      P->AddDomain(d1);
      P->AddDomain(d2);
      P->SetNeighborhood(0, nb1);
      P->SetNeighborhood(1, nb2);
      if (P->GetNumberOfDomains() != 2)
      {
        passed = false;
        errstring += std::string("AddDomain method failed. ");
      }
      //      if (P->GetNeighborhood(0) != nb1 || P->GetNeighborhood(1) != nb2)
      // {
      //  passed = false;
      //  errstring += std::string("SetNeighborhood method failed. ");
      // }

      // Add position
      PointType pt;
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i) + 0.1f;
        pt[1] = static_cast<double>(i) + 0.2f;
        pt[2] = static_cast<double>(i) + 0.3f;
        P->AddPosition(pt, 0);
        P->AddPosition(pt, 1);
      }
      if (P->GetNumberOfParticles(0) != SZ || P->GetNumberOfParticles(1) != SZ)
      {
        passed = false;
        errstring += std::string("AddPosition method failed. ");
      }

      
      // Split particles
      P->SplitAllParticles(0.1);
      if (P->GetNumberOfParticles(0) != SZ*2 || P->GetNumberOfParticles(1) != SZ*2)
      {
        passed = false;
        errstring += std::string("SplitAllParticles method failed. ");
      }

      for (unsigned int i = 0; i < 2; i++)
      {
        P->SplitParticle(0.1, 0, i);
        if (P->GetNumberOfParticles(i) != SZ*2+1)
        {
          passed = false;
          errstring += std::string("SplitParticle method failed. ");
        }
      }

      // Get/Set position
      for (unsigned int i = 0; i < 2; i++)
      {
        PointType originalPt = P->GetPosition(0, i);
        PointType modPt;
        modPt[0] = originalPt[0] + 0.1;
        modPt[1] = originalPt[1] + 0.2;
        modPt[2] = originalPt[2] + 0.3;
        P->SetPosition(modPt, 0, i);
        if (P->GetPosition(0, i) != modPt)
        {
          passed = false;
          errstring += std::string("Get/SetPosition methods failed. ");
        }
      }

      // remove position
      for (unsigned int i = 0; i < 2; i++)
      {
        P->RemovePosition(0, i);
        if (P->GetNumberOfParticles(i) != SZ*2)
        {
          passed = false;
          errstring += std::string("RemovePosition method failed. ");
        }
      }

      // Find neighborhood
      itk::PSMRegionNeighborhood<3>::PointVectorType vec;
      for (unsigned int i = 0; i < 2; i++)
      {
        // get permuted position
        PointType modPt = P->GetPosition(SZ/2, i);
        modPt[0] += 0.0011;
        vec = P->FindNeighborhoodPoints(modPt, 0.5, i);
        if (vec.size() != 2)
        {
          passed = false;
          errstring += std::string("FindNeighborhoodPoints method failed. ");
        }
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
