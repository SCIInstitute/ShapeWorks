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
#include "itkPSMPointTree.h"
#include "itkPoint.h"
#include "itkMacro.h"

/** This test exercises functionality of the base itkPSMPointTree class */
int itkPSMPointTreeTest(int, char* [])
{
  typedef itk::Point<double, 3> PointType;
  const unsigned int SZ = 10;

  bool passed = true;
  std::string errstring = "";

  try
    {
      // Test instantiation of the object
      itk::PSMPointTree<3>::Pointer P = itk::PSMPointTree<3>::New();

      // Construct a tree with the bounds (0,0,0) - (SZ,SZ,SZ) and 3 levels.
      PointType lb, ub, pt;
      lb[0] = 0.0f; lb[1] = 0.0f; lb[2] = 0.0f;
      ub[0] = ub[1] = ub[2] = static_cast<double>(SZ); 

      P->ConstructTree(lb,ub,3);
      
      // Add some points to the tree.  Spread them out uniformly.
      for (unsigned int x = 1; x < SZ; x++)
        {
          for (unsigned int y = 1; y < SZ; y++)
            {
              for (unsigned int z = 1; z < SZ; z++)
                {
                  pt[0] = static_cast<double>(x);
                  pt[1] = static_cast<double>(y);
                  pt[2] = static_cast<double>(z);
                  
                  // Add position with Domain = 0
                  P->AddPoint(pt,0);
                }
            }
        }

      // Test getting the points in a given region
      ub[0] = static_cast<double>(SZ/2);
      ub[1] = static_cast<double>(SZ/2);
      ub[2] = static_cast<double>(SZ/2);

      itk::PSMPointTree<3>::PointIteratorListType l = P->FindPointsInRegion(lb,ub);
      for (itk::PSMPointTree<3>::PointIteratorListType::const_iterator it = l.begin();
           it != l.end(); it++)
        {
          // All points should be <= SZ/2,SZ/2,SZ/2;  We must be in the 1st quadrant.
          if ( (*it)->Point[0] > ub[0] || (*it)->Point[1] > ub[1] || (*it)->Point[2] > ub[2] )
            {
              passed = false;
              errstring = "Points were sorted into the wrong tree bin.";
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
