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
#include <fstream>
#include "itkImage.h"
#include "itkPSMRBFCorrespondenceInterpolator.h"

/** This test exercises functionality of the base
    itkPSMRBFCorrespondenceInterpolator class */
int itkPSMRBFCorrespondenceInterpolatorTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";
  std::string output_path = "";
  std::string input_path_prefix = "";

  // Check for proper arguments
  if (argc < 4)
    {
      std::cout << "Wrong number of arguments. \nUse: " 
	<< "itkPSMRBFCorrespondenceInterpolatorTest PointFileA PointFileB PointListToInterpolate [input_path]\n"
	<< std::endl;
      return EXIT_FAILURE;
    }
  
  if (argc > 4)
    {
      input_path_prefix = std::string(argv[4]);
    }


  try
    {
     // Create the modeling filter and set up the optimization.
     itk::PSMRBFCorrespondenceInterpolator<3>::Pointer P 
       = itk::PSMRBFCorrespondenceInterpolator<3>::New();

     std::vector<itk::PSMRBFCorrespondenceInterpolator<3>::PointType> landmarks;

     // Read points from disk
     for (unsigned int i = 1; i < 4; i++)
       {

         std::vector<itk::PSMRBFCorrespondenceInterpolator<3>::PointType> c;
         int counter = 0;

         // Open the ascii file.
         //         std::cout << "Reading " << argv[i] << std::endl;
         std::string fn = input_path_prefix + std::string(argv[i]);
         std::ifstream in( fn.c_str() );
         if ( !in )
           {
             errstring += "Could not open point file for input: ";
             errstring += fn;
             passed = false;
             break;
           }
         
         // Read all of the points, one point per line.
         while (in)
           {
             itk::PSMRBFCorrespondenceInterpolator<3>::PointType pt;

              for (unsigned int d = 0; d < 3; d++)
                {
                  in >> pt[d];
                }
              c.push_back(pt);
              counter++;
            }
          // this algorithm pushes the last point twice
          c.pop_back();
          //  std::cout << "Read " << counter-1 << " points. " << std::endl;
          in.close();

          if (i == 1)
            {
              P->SetPointSetA(c);
            }
          else if (i == 2)
            {
              P->SetPointSetB(c);
            }
          else if (i == 3)
            {
              landmarks = c;
            }

        }
     
     
     if (passed == true)
       {
         // Compute the mapping from PointSetA to PointSetB
         P->Initialize();

         std::cout << argv[3] << std::endl;
         
         // Map each point
         for (unsigned int i = 0; i < landmarks.size(); i++)
           {
           itk::PSMRBFCorrespondenceInterpolator<3>::PointType p
             =  P->Evaluate(landmarks[i]);
           std::cout << p[0] << " " << p[1] << " " << p[2] << std::endl;
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
    //    std::cout << "All tests passed" << std::endl;
    return EXIT_SUCCESS;
    }
  else
    {
    std::cout << "Test failed with the following error:" << std::endl;
    std::cout << errstring << std::endl;
    return EXIT_FAILURE;
    }
}
