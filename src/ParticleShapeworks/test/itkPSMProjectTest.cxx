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
#include "itkPSMProject.h"
#include "itkPSMProjectReader.h"
#include "itkPSMProjectWriter.h"

/** This test exercises functionality of the base itkPSMEntropyModelFilter class */
int itkPSMProjectTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";

  // Check for proper arguments
  if (argc < 2)
    {
      std::cout << "Wrong number of arguments. \nUse: " 
	<< "itkPSMProjectTest PSMProjectFile.xml <outputfile.xml (optional>\n"
        << "See itk::PSMProject and itk::PSMDOMNode for documentation on the parameter file format."
	<< std::endl;
      return EXIT_FAILURE;
    }

  try
    {
      // Try to read a valid PSM Project XML file
      itk::PSMProjectReader::Pointer reader = itk::PSMProjectReader::New();
      reader->SetFileName(argv[1]);
      reader->Update();
      itk::PSMProject::Pointer project = reader->GetOutput();

      // Print out the project
      std::cout << project << std::endl;

      // Stream the project to std::out
      project->Stream(std::cout);

      // Can we retrieve all of the appropriate fields?
      // Distance transforms ...
      const std::vector<std::string> &dt = project->GetDistanceTransforms();
      std::cout << itk::PSMProject::distance_transforms_tag << std::endl;
      for (unsigned int i = 0; i < dt.size(); i++)
        {
          std::cout << " " << dt[i] << std::endl;
        }

      // Models
      const std::vector<std::string> &model = project->GetModel(std::string("my_model"));
      std::cout << itk::PSMProject::model_tag << std::endl;
      for (unsigned int i = 0; i < model.size(); i++)
        {
          std::cout << " " << model[i] << std::endl;
        }

      // project->GetSegmentations();
      // project->GetCorrespondences();

      if (argc > 2) // also write an output file
        {
          itk::PSMProjectWriter::Pointer writer = itk::PSMProjectWriter::New();
          writer->SetFileName(argv[2]);
          writer->SetInput(project);
          writer->Update();
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

