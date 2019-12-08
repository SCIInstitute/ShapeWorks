#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>

namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
bool Image::read(const std::string &inFilename)
{
  if (inFilename.empty())
  {
    std::cerr << "Empty filename passed to read; returning false." << std::endl;
    return false;
  }

  typedef itk::ImageFileReader<ImageType> ReaderType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inFilename);
  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Failed to read image " << inFilename << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Successfully read image " << inFilename << std::endl;
  this->image = reader->GetOutput();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::write(const std::string &outFilename)
{
  if (!this->image)
  {
    std::cerr << "No image to write, so returning false." << std::endl;
    return false;
  }

  typedef itk::ImageFileWriter<ImageType> WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->image);
  writer->SetFileName(outFilename);

  //<ctc> todo: check for empty filename, and do something more sensible if an exception is thrown -- otherwise there's an infinite loop!
  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Failed to write image to " << outFilename << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Successfully wrote image " << outFilename << std::endl;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::antialias(float maxRMSErr, int numIter)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }
  
  typedef itk::AntiAliasBinaryImageFilter<ImageType, ImageType> FilterType;
  FilterType::Pointer antialiasFilter = FilterType::New();
  antialiasFilter->SetInput(this->image);
  antialiasFilter->SetMaximumRMSError(maxRMSErr);
  antialiasFilter->SetNumberOfIterations(numIter);
  //antialiasFilter->SetNumberOfLayers(numLayers);  // TODO: should we specify this parameters?
  try
  {
    antialiasFilter->Update();  
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Antialias filter failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Antialias filter succeeded!\n";
  this->image = antialiasFilter->GetOutput();
  return true;
}

} // Shapeworks
