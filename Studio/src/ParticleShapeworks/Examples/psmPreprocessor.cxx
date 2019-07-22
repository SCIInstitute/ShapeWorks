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
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
//#include <itkPSMCleanTrimLabelMapImageFilter.h>
#include <itkPSMImplicitSurfaceImageFilter.h>

int main_ (int argc, char *argv[])
{
  if (argc < 5)
    {
      std::cout << "Usage:" << argv[0] 
		<< "output_prefix smoothing_sigma isosurface_value input_file_1 input_file_2 ..." << std::endl;
      return 1;
    }

  double sigma = 0.0;
  double isosurface_value = 0.5;
  if (argc > 3)
    { sigma = atof(argv[3]);  }
  if (argc >4)
    {  isosurface_value = atof(argv[4]); }


  typedef itk::Image<float, 3> image_type;

  try {


  // First read all of the files one by one and compute the bounding box.


  
    std::cout << "Reading " << argv[4] << std::endl;
    itk::ImageFileReader<image_type>::Pointer reader 
      = itk::ImageFileReader<image_type>::New();
    reader->SetFileName(argv[4]);
    reader->Update();
    
    itk::ImageFileWriter<image_type>::Pointer fwriter 
      = itk::ImageFileWriter<image_type>::New();
    // fwriter->SetInput(filt->GetOutput());
    // fwriter->SetFileName("tmp.nrrd");
    // fwriter->Update();
    
  }
  catch (itk::ExceptionObject &e)
    {
      std::cerr << e << std::endl;
      return 1;
    }
  catch( ...)
    {
      std::cerr << "Unknown error" << std::endl;
      return 2;
    }

  return 0;
}

