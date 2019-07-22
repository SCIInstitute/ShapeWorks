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
#include "itkPSMGradientDescentOptimizer.h"
#include "itkPSMShapeEntropyFunction.h"
#include "itkPSMRegionNeighborhood.h"
#include "itkPSMRegionDomain.h"

/** This test exercises functionality of the base itkPSMGradientDescentOptimizerFilter class */
int itkPSMGradientDescentOptimizerTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";

  try
    {
      itk::PSMGradientDescentOptimizer<float, 3>::Pointer P 
        = itk::PSMGradientDescentOptimizer<float, 3>::New();

      typedef itk::Point<double, 3> PointType;
      
      itk::PSMShapeEntropyFunction<3>::Pointer cf 
        = itk::PSMShapeEntropyFunction<3>::New();
      itk::PSMShapeMatrixAttribute<double, 3>::Pointer sm = 
        itk::PSMShapeMatrixAttribute<double, 3>::New();
      sm->SetDomainsPerShape(1);
      cf->SetShapeMatrix(sm);

      // setup particle system
      itk::PSMParticleSystem<3>::Pointer ps = itk::PSMParticleSystem<3>::New();
      ps->RegisterAttribute(sm);

      // fill in particle system with points
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
      ps->AddDomain(d1);
      ps->AddDomain(d2);
      ps->SetNeighborhood(0, nb1);
      ps->SetNeighborhood(1, nb2);

      // Add position
      PointType pt;
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i) + 0.1f;
        pt[1] = static_cast<double>(i) + 0.2f;
        pt[2] = static_cast<double>(i) + 0.3f;
        ps->AddPosition(pt, 0);
        ps->AddPosition(pt, 1);
      }

      // Setup optimizer
      P->SetCostFunction(cf);
      P->SetParticleSystem(ps);

      // FIXME: We should probably run optimization here!
      //P->StartOptimization();
      
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
