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
#include "itkPoint.h"
#include "itkMacro.h"
#include "itkPSMContainer.h"

/** This test exercises functionality of the base itkPSMContainer class */
int itkPSMContainerTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";

  const unsigned int SZ = 100;
  // First do 3D Point
  try
    {
    // Define the type of point (particle position) stored in this
    // container.
    typedef itk::Point<double, 3> PointType;
    itk::PSMContainer<PointType>::Pointer P = itk::PSMContainer<PointType>::New();
    PointType pt;

    // Test storing values
    for (unsigned int i = 0; i < SZ; i++)
      {
      pt[0] = static_cast<double>(i) + 0.1f;
      pt[1] = static_cast<double>(i) + 0.2f;
      pt[2] = static_cast<double>(i) + 0.3f;
          
      P->operator[](i) = pt;
      }

    // Test Size method
    if (P->GetSize() != SZ) 
      {
      passed = false;
      errstring += std::string("GetSize method failed");
      }

    // Test HasIndex method
    if (P->HasIndex(0) != true || P->HasIndex(SZ+1) != false)
      {
      passed = false;
      errstring += std::string("HasIndex method failed");
      }

    // Test accessor methods
    if (P->operator[](SZ-1)[0] != static_cast<double>(SZ-1) + 0.1f || 
        P->operator[](SZ-1)[1] != static_cast<double>(SZ-1) + 0.2f || 
        P->operator[](SZ-1)[2] != static_cast<double>(SZ-1) + 0.3f)
      {
      passed = false;
      errstring += std::string("Accessor method failed or values were stored incorrectly.");
      }

    // Test Erase method
    P->Erase(SZ /2);
    if (P->HasIndex(SZ/2) == true)
      {
      passed = false;
      errstring += std::string("Erase method failed");
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

  // Next do 2D Point
  try
    {
    typedef itk::Point<double, 2> PointType;
    itk::PSMContainer<PointType>::Pointer P = itk::PSMContainer<PointType>::New();
    PointType pt;

    // Test storing values
    for (unsigned int i = 0; i < SZ; i++)
      {
      pt[0] = static_cast<double>(i) + 0.1f;
      pt[1] = static_cast<double>(i) + 0.2f;
          
      P->operator[](i) = pt;
      }

    // Test Size method
    if (P->GetSize() != SZ) 
      {
      passed = false;
      errstring += std::string("GetSize method failed");
      }

    // Test HasIndex method
    if (P->HasIndex(0) != true || P->HasIndex(SZ+1) != false)
      {
      passed = false;
      errstring += std::string("HasIndex method failed");
      }

    // Test accessor methods
    if (P->operator[](SZ-1)[0] != static_cast<double>(SZ-1) + 0.1f || 
        P->operator[](SZ-1)[1] != static_cast<double>(SZ-1) + 0.2f) 
      {
      passed = false;
      errstring += std::string("Accessor method failed or values were stored incorrectly.");
      }

    // Test Erase method
    P->Erase(SZ /2);
    if (P->HasIndex(SZ/2) == true)
      {
      passed = false;
      errstring += std::string("Erase method failed");
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
