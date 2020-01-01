#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkChangeInformationImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>


namespace Shapeworks {

///////////////////////////////////////////////////////////////////////////////
bool Image::read(const std::string &inFilename)
{
  if (inFilename.empty())
  {
    std::cerr << "Empty filename passed to read; returning false." << std::endl;
    return false;
  }

  using ReaderType = itk::ImageFileReader<ImageType>;
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

  using WriterType itk::ImageFileWriter<ImageType>;
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
  
  using FilterType itk::AntiAliasBinaryImageFilter<ImageType, ImageType>;
  FilterType::Pointer antialiasFilter = FilterType::New();
  antialiasFilter->SetInput(this->image);
  antialiasFilter->SetMaximumRMSError(maxRMSErr);
  antialiasFilter->SetNumberOfIterations(numIter);
  //antialiasFilter->SetNumberOfLayers(numLayers);  // TODO: should we specify this parameters?
  this->image = antialiasFilter->GetOutput();

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
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::resamplevolume(bool isBinary, bool isCenterImage, float isoSpacing, Dims outputSize)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  ResampleFilterType::InterpolatorType::Pointer interpolator;

  if (isBinary)
  {
    this->antialias();

    using InterpolatorType = itk::BSplineInterpolateImageFunction<ImageType, double, double>;
    interpolator = InterpolatorType::New();
    interpolator->SetSplineOrder(3);
    resampler->SetDefaultPixelValue(-1);
  }
  else
  {
    using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
    interpolator = InterpolatorType::New();
    resampler->SetDefaultPixelValue(0);
  }
  resampler->SetInterpolator(interpolator);

  using TransformType = itk::IdentityTransform<double, Image::dims>;
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
  resampler->SetTransform(transform);

  ImageType::SpacingType spacing;
  spacing[0] = isoSpacing;
  spacing[1] = isoSpacing;
  spacing[2] = isoSpacing;
  resampler->SetOutputSpacing(spacing);
  resampler->SetOutputOrigin(image->GetOrigin());
  resampler->SetOutputDirection(image->GetDirection());

  ImageType::SizeType inputSize = resampler->GetLargestPossibleRegion().GetSize();
  ImageType::SpacingType inputSpacing = resampler->GetSpacing();
  if (outputSize[0] == 0 || outputSize[1] == 0 || outputSize[2] == 0)
  {
    outputSize[0] = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
    outputSize[1] = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
    outputSize[2] = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
  }
  resampler->SetSize(outputSize);

  resampler->SetInput(this->image);
  this->image = resampler->GetOutput();

  if (isCenterImage)
  {
    using ImageInfoFilterType = itk::ChangeInformationImageFilter<ImageType>;
    ImageInfoFilterType::Pointer infoFilter = ImageInfoFilterType::New();
    infoFilter->SetInput(this->image);
    infoFilter->CenterImageOn();
    this->image = infoFilter->GetOutput();
  }

  try
  {
    resampler->Update();

    if (isBinary)
    {
      using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
      FilterType::Pointer filter = FilterType::New();
      filter->SetInput(this->image);
      filter->SetLowerThreshold(itk::NumericTraits<PixelType>::Zero);
      filter->SetInsideValue(itk::NumericTraits<PixelType>::One);
      filter->Update();
      this->image = filter->GetOutput();
    }
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Resample volumes to be isotropic failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Resample volumes to be isotropic succeeded!\n";
  return true;
}

} // Shapeworks


