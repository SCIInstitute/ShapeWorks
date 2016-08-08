/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 * or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <iostream>
#include "itkPSMCommandLineClass.h"
#include "itkPSMProjectReader.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkExceptionObject.h"
#include "itkGDCMImageIOFactory.h"
#include "itkMetaImageIOFactory.h"
#include "itkObjectFactoryBase.h"
#include "itkNrrdImageIOFactory.h"
#include "itkExceptionObject.h"


int main(int argc, char *argv[])
{
  std::string output_path = "";
  std::string input_path_prefix = "";
  std::string errstring = "";
  // Check for proper arguments
  if (argc < 2)
  {
    std::cout << "Wrong number of arguments. \nUse: "
      << "psmCommandLineTool parameter_file [output_path] [input_path]\n"
      << "See itk::PSMParameterFileReader for documentation on the parameter file format.\n"
      << "Note that input_path will be prefixed to any file names and paths in the xml parameter file.\n"
      << std::endl;
    return EXIT_FAILURE;
  }

  if (argc > 2)
  {
    output_path = std::string(argv[2]);
  }

  if (argc > 3)
  {
    input_path_prefix = std::string(argv[3]);
  }

  try
  {
    // The following are called to fix an ITK runtime error where image format is not recognized.
    itk::ObjectFactoryBase::RegisterFactory(itk::MetaImageIOFactory::New());
    itk::ObjectFactoryBase::RegisterFactory(itk::GDCMImageIOFactory::New());
    itk::ObjectFactoryBase::RegisterFactory(itk::NrrdImageIOFactory::New());

    // Read and parse the project file
    // itk::PSMProjectReader::Pointer xmlReader = itk::PSMProjectReader::New();
    // xmlReader->SetFileName(argv[1]);
    // xmlReader->Update();
    // itk::PSMProject::Pointer project = xmlReader->GetOutput();

    // The dimensions of the input images need to be checked in order to
    // correctly initialize PSMCommandLineClass.
    //    const std::vector<std::string> &dt_files = project->GetDistanceTransforms();
    //  std::string fname = input_path_prefix + dt_files[0];

    // 1. First try to read the file. This is just a convenient way to
    // check whether the file is valid or not. An exception will be
    // thrown by ITK otherwise.  This is slow, but an easy solution.
    //  itk::ImageFileReader<itk::Image<unsigned int, 3>>::Pointer
    //   testreader = itk::ImageFileReader<itk::Image<unsigned int, 3>>::New();
    //  testreader->SetFileName(fname);
    //  testreader->Update();

    // 2. If the file was read successfully, we now just probe the image
    // information. Note that ITK does not provide a method under
    // ImageIOBase to actually check the validity of a file, so the
    // following is dangerous unless you know that the file actually
    // exists.
    //    std::cout << "Checking input image dimensions of " << fname << std::endl;
    //   typedef itk::ImageIOBase::IOComponentType ScalarPixelType;
    //  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(
    //                                     fname.c_str(), itk::ImageIOFactory::ReadMode);
    //  imageIO->SetFileName(fname);

    //    imageIO->ReadImageInformation();
    //    const size_t numOfDimensions =  imageIO->GetNumberOfDimensions();
    //   std::cout << "Number of dimensions: " << numOfDimensions << std::endl;
    const size_t numOfDimensions = 3;
    if (numOfDimensions == 2)
    {
      std::cout << "Running 2D optimization code" << std::endl;
      itk::PSMCommandLineClass<2>::Pointer psmClass
        = itk::PSMCommandLineClass<2>::New();
      psmClass->Run(argv[1], input_path_prefix, output_path);
    } else if (numOfDimensions == 3)
    {
      std::cout << "Running 3D optimization code" << std::endl;
      itk::PSMCommandLineClass<3>::Pointer psmClass
        = itk::PSMCommandLineClass<3>::New();
      psmClass->Run(argv[1], input_path_prefix, output_path);
    }
  }

  catch (itk::ExceptionObject &e)
  {
    std::cerr << "ITK exception with description: " << e.GetDescription()
      << "\n at location:" << e.GetLocation()
      << "\n in file:" << e.GetFile() << std::endl;
    return EXIT_FAILURE;
  }

  catch (...)
  {
    errstring = "Unknown exception thrown";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
