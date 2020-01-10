#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkChangeInformationImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>

#include <limits>

namespace Shapeworks {

//todo: these filters are starting to feel homogeneous enough to wrap into a common try/catch function

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

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->image);
  writer->SetFileName(outFilename);
  writer->SetUseCompression(true);

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
// maxRMSError: range [0.0, 1.0], determines how fast the solver converges (larger is faster)
//
bool Image::antialias(unsigned numIterations, float maxRMSErr, unsigned numLayers)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }
  
  using FilterType = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetMaximumRMSError(maxRMSErr);
  filter->SetNumberOfIterations(numIterations);
  if (numLayers)
    filter->SetNumberOfLayers(numLayers);

  filter->SetInput(this->image);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();  
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
bool Image::binarize(PixelType threshold, PixelType inside, PixelType outside)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetLowerThreshold(threshold);
  filter->SetInsideValue(inside);
  filter->SetOutsideValue(outside);
  filter->SetInsideValue(itk::NumericTraits<PixelType>::One);

  filter->SetInput(this->image);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();  
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Binarize filter failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Binarize filter succeeded!\n";
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::recenter()
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->CenterImageOn();

  filter->SetInput(this->image);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();  
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Recenter image failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Recenter image succeeded!\n"; // todo: eventually remove debugging statements like this (or add a verbose flag)
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::resample(bool isBinary, float isoSpacing, Dims outputSize)
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
    resampler->SetDefaultPixelValue(-1);

    using InterpolatorType = itk::BSplineInterpolateImageFunction<ImageType, double, double>;
    InterpolatorType::Pointer bspline_interp = InterpolatorType::New();
    bspline_interp->SetSplineOrder(3);
    interpolator = bspline_interp;
  }
  else
  {
    resampler->SetDefaultPixelValue(0);

    using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
    interpolator = InterpolatorType::New();
  }
  resampler->SetInterpolator(interpolator);

  using TransformType = itk::IdentityTransform<double, Image::dims>;
  TransformType::Pointer transform = TransformType::New();
  transform->SetIdentity();
  resampler->SetTransform(transform);
  
  ImageType::SizeType inputSize = image->GetLargestPossibleRegion().GetSize();
  ImageType::SpacingType inputSpacing = image->GetSpacing();
  if (outputSize[0] == 0 || outputSize[1] == 0 || outputSize[2] == 0)
  {
    outputSize[0] = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
    outputSize[1] = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
    outputSize[2] = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
  }
  resampler->SetSize(outputSize);

  ImageType::SpacingType spacing;
  spacing[0] = isoSpacing;
  spacing[1] = isoSpacing;
  spacing[2] = isoSpacing;
  resampler->SetOutputSpacing(spacing);
  resampler->SetOutputOrigin(image->GetOrigin());
  resampler->SetOutputDirection(image->GetDirection());

  resampler->SetInput(this->image);
  this->image = resampler->GetOutput();

  try
  {
    resampler->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Resample images to be isotropic failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  if (isBinary)
    this->binarize();

  std::cout << "Resample images to be isotropic succeeded!\n";
  return true;
}

} // Shapeworks


