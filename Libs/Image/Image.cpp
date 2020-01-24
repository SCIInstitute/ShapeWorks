#include "Image.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkChangeInformationImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkTestingComparisonImageFilter.h>

#include <limits>

namespace shapeworks {

//todo: these filters are starting to feel homogeneous enough to wrap into a common try/catch function

/// read
/// \param filename
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

#if DEBUG_CONSOLIDATION
  std::cout << "Successfully read image " << inFilename << std::endl;
#endif
  this->image = reader->GetOutput();
  return true;
}

/// write
/// \param filename
/// \param useCompression
bool Image::write(const std::string &outFilename, bool useCompression)
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
  writer->SetUseCompression(useCompression);

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

#if DEBUG_CONSOLIDATION
  std::cout << "Successfully wrote image " << outFilename << std::endl;
#endif
  return true;
}

/// antialias
/// \param numIterations
/// \param maxRMSErr      range [0.0, 1.0], determines how fast the solver converges (larger is faster)
/// \param numLayers      size of region around a pixel to sample
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

#if DEBUG_CONSOLIDATION
 std::cout << "Antialias filter succeeded!\n";
#endif
  return true;
}

/// binarizes image into two regions separated by threshold value
/// \param threshold  values <= threshold are considereed "outside" and given that value [default is 0.0]
/// \param inside     value for inside region [default is 1]
/// \param outside    value for outside region [default is 0]
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

#if DEBUG_CONSOLIDATION
  std::cout << "Binarize filter succeeded!\n";
#endif
  return true;
}

/// recenter
/// recenters by changing origin (in the image header) to the physcial coordinates of the center of the image
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

#if DEBUG_CONSOLIDATION
  std::cout << "Recenter image succeeded!\n";
#endif
  return true;
}

/// resample
/// \param isoSpacing
/// \param binaryInput    binary input images (will antialias then using a bspline filter to produce better results)
/// \param outputSize     image size can be changed
bool Image::resample(float isoSpacing, bool binaryInput, Dims outputSize)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using ResampleFilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  ResampleFilterType::Pointer resampler = ResampleFilterType::New();
  
  ResampleFilterType::InterpolatorType::Pointer interpolator;

  // For binary input images, antialiasing then using a bspline filter produces better results
  if (binaryInput)
  {
    // todo (archana): take out bsplinefilter altogether, document (above) that this function only accepts continuous image
    using InterpolatorType = itk::BSplineInterpolateImageFunction<ImageType, double, double>;
    InterpolatorType::Pointer bspline_interp = InterpolatorType::New();
    bspline_interp->SetSplineOrder(3);
    interpolator = bspline_interp;
    this->antialias();
    resampler->SetDefaultPixelValue(-1.0);
  }
  else
  {
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
    // fixme: for some reason this isn't what we're supposed to do.
    // for some datasets (ellipsoid) it works, but for others it's unnecessary
    // more investigation needed.
    outputSize[0] = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
    outputSize[1] = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
    outputSize[2] = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
    // probably std:;ceil should be std::floor (ask Alan and Shireen)
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

#if DEBUG_CONSOLIDATION
  std::cout << "Resample images to be isotropic succeeded!\n";
#endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
bool Image::compare_equal(const Image &other)
{
  using DiffType = itk::Testing::ComparisonImageFilter<ImageType, ImageType>;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(other.image);
  diff->SetTestInput(this->image);
  diff->SetDifferenceThreshold(0);
  diff->SetToleranceRadius(0);

  try
  {
    diff->UpdateLargestPossibleRegion();
  } catch (itk::ExceptionObject &exp) {
    std::cerr << "Comparison failed" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  const unsigned long numberOfPixelsWithDifferences = diff->GetNumberOfPixelsWithDifferences();

  if (numberOfPixelsWithDifferences > 0.0) {
    return false;
  }

  return true;
}

bool Image::padvolume(int paddingSize, float paddingValue)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  ImageType::SizeType lowerExtendRegion;
  lowerExtendRegion[0] = paddingSize;
  lowerExtendRegion[1] = paddingSize;
  lowerExtendRegion[2] = paddingSize;

  ImageType::SizeType upperExtendRegion;
  upperExtendRegion[0] = paddingSize;
  upperExtendRegion[1] = paddingSize;
  upperExtendRegion[2] = paddingSize;

  ImageType::PixelType constantPixel = paddingValue;

  using ConstantPadImageFilterType = itk::ConstantPadImageFilter<ImageType, ImageType>;
  ConstantPadImageFilterType::Pointer padFilter = ConstantPadImageFilterType::New();

  padFilter->SetInput(this->image);
  padFilter->SetPadLowerBound(lowerExtendRegion);
  padFilter->SetPadUpperBound(upperExtendRegion);
  padFilter->SetConstant(constantPixel);
  this->image = padFilter->GetOutput();

  try
  {
    padFilter->Update();  
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Pad volume with constant failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Pad volume with constant succeeded!\n";
  return true;

}

} // Shapeworks


