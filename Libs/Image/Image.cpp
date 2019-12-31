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
  if (outFilename.empty())
  {
    std::cerr << "Empty filename passed to write; returning false." << std::endl;
    return false;
  }

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

///////////////////////////////////////////////////////////////////////////////
bool Image::resamplevolume(bool isBinary, bool isCenterImage, float isoSpacing, int sizeX, int sizeY, int sizeZ)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
  ResampleFilterType::Pointer resampler = ResampleFilterType::New();

  typedef itk::IdentityTransform<double, Dimension> TransformType;
  TransformType::Pointer transform = TransformType::New();
  
  transform->SetIdentity();
  resampler->SetTransform(transform);

  if (isBinary)
  {
    typedef itk::BSplineInterpolateImageFunction<ImageType, double, double> InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    interpolator->SetSplineOrder(3);
    resampler->SetInterpolator(interpolator);
    resampler->SetDefaultPixel(-1);
  }
  else
  {
    typedef itk::LinearInterpolateImageFunction<ImageType, double> InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    resampler->SetInterpolator(interpolator);
    resampler->SetDefaultPixelValue(0);
  }

  ImageType::SpacingType spacing;
  spacing[0] = isoSpacing;
  spacing[1] = isoSpacing;
  spacing[2] = isoSpacing;

  resampler->SetOutputSpacing(spacing);
  resampler->SetOutputOrigin(inputImage->GetOrigin());
  resampler->SetOutputDirection(inputImage->GetDirection());

  ImageType::SizeType outputSize;
  ImageType::SizeType inputSize = inputImage->GetLargestPossibleRegion().GetSize();
  typedef ImageType::SizeType::SizeValueType SizeValueType;
  const ImageType::SpacingType& inputSpacing = inputImage->GetSpacing();

  if (sizeX == 0 || sizeY == 0 || sizeZ == 0)
  {
    sizeX = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
    sizeY = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
    sizeZ = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
  }

  outputSize[0] = static_cast<SizeValueType>(sizeX);
  outputSize[1] = static_cast<SizeValueType>(sizeY);
  outputSize[2] = static_cast<SizeValueType>(sizeZ);

  resampler->SetSize(outputSize);

  if (isBinary)
  {
    this->image.antialias();
  }

  resampler->SetInput(this->image);
  resampler->Update();

  ImageType::Pointer resampledImage = resampler->GetOutput();

  if (isBinary)
  {
    ImageType::Pointer outputImage = OutputImageType::New();
  }
  else
  {
    ImageType::Pointer outputImage = resampler->GetOutput();
  }

  outputImage->SetSpacing(spacing);
  outputImage->SetOrigin(inputImage->GetOrigin());

  if (isBinary)
  {
    itk::ImageRegionIterator<ImageType> imageIterator(resampledImage, resampledImage->GetLargestPossibleRegion());
    itk::ImageRegionIterator<ImageType> outIterator(outputImage, outputImage->GetLargestPossibleRegion());

    while (!imageIterator.IsAtEnd())
    {
      OutputPixelType val = imageIterator.Get();

      if (val>=0)
          outIterator.Set((OutputPixelType)1);
      else
          outIterator.Set((OutputPixelType)0);

      ++imageIterator;
      ++outIterator;
    }
  }

  typedef itk::ChangeInformationImageFilter<ImageType> ImageInfoFilterType;
  ImageInfoFilterType::Pointer infoFilter = ImageInfoFilterType::New();

  if (isCenterImage)
  {
    infoFilter->SetInput(outputImage);
    infoFilter->CenterImageOn();
    infoFilter->Update();
    // writer->SetInput(infoFilter->GetOutput());
    resampler->Update();
  }
  else
    // writer->SetInput(resampler->GetOutput());
    resampler->Update();
  
  try
  {
    resampler->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Resample volumes to be isotropic failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Resample volumes to be isotropic succeeded!\n";
  this->image = resampler->GetOutput();
  return true;
}

} // Shapeworks


