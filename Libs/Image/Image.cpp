#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>

namespace Shapeworks {

bool Image::read(const std::string &inFilename)
{
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

  this->image = reader->GetOutput();
  return true;
}

bool Image::write(const std::string &outFilename)
{
  if (!this->image) return false;

  typedef itk::ImageFileWriter<ImageType> WriterType;

  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->image);
  writer->SetFileName(outFilename);

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

  return true;
}

bool Image::antialias(float maxRMSErr, int numIter)
{
  if (!this->image) return false;

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

  this->image = antialiasFilter->GetOutput();
  return true;
}

} // Shapeworks
