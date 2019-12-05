#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>

namespace Shapeworks {

int Image::Antialias(const std::string &inFilename, const std::string &outFilename, float maxRMSErr, int numIter)
{
  typedef   float InputPixelType;
  typedef   float InternalPixelType;
  typedef   float OutputPixelType;

  const     unsigned int    Dimension = 3;
  typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
  typedef itk::Image< OutputPixelType,    Dimension >   OutputImageType;
  typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
  //<ctc> TODO: in order to make more efficient use of multiple commands, make a function to read an image in this Image class and call it here (and in all the other executables being consolidating into this class)
  typedef itk::ImageFileReader< InputImageType  >  ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inFilename );
  InputImageType::ConstPointer inputImage = reader->GetOutput();

  typedef itk::AntiAliasBinaryImageFilter< InternalImageType, InternalImageType > FilterType;
  FilterType::Pointer antialiasFilter = FilterType::New();
  antialiasFilter->SetInput( reader->GetOutput() );
  antialiasFilter->SetMaximumRMSError(maxRMSErr);
  antialiasFilter->SetNumberOfIterations( numIter );
  antialiasFilter->Update();

  // Write the final output
  //<ctc> TODO: in order to make more efficient use of multiple commands, save image to Image class and only write it to disk when complete ([] add function to class to write its image(s))
  typedef itk::ImageFileWriter< OutputImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( antialiasFilter->GetOutput() );
  writer->SetFileName(outFilename.c_str());
  writer->Update();
    
  return EXIT_SUCCESS;
}

} // Shapeworks
