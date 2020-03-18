#include "Image.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkBSplineInterpolateImageFunction.h>
#include <itkChangeInformationImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkTestingComparisonImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkReinitializeLevelSetImageFilter.h>
#include <itkTranslationTransform.h>
#include <itkBinaryFillholeImageFilter.h>
#include <itkReinitializeLevelSetImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkTPGACLevelSetImageFilter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkExtractImageFilter.h>
#include <itkImageRegion.h>

#include <sys/stat.h>

namespace shapeworks {

//todo: these filters are starting to feel homogeneous enough to wrap into a common try/catch function

/// read
///
/// reads image
///
/// \param filename
bool Image::read(const std::string &filename)
{
  if (filename.empty())
  {
    std::cerr << "Empty filename passed to read; returning false." << std::endl;
    return false;
  }

  if (Image::is_directory(filename))
  {
    return this->read_image_dir(filename);
  }

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename);

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Failed to read image " << filename << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Successfully read image " << filename << std::endl;
#endif
  this->image = reader->GetOutput();
  return true;
}

/// read_image_dir
/// \param pathname directory containing image series
bool Image::read_image_dir(const std::string &pathname)
{
  using ReaderType = itk::ImageSeriesReader<ImageType>;
  using ImageIOType = itk::GDCMImageIO;
  using InputNamesGeneratorType = itk::GDCMSeriesFileNames;

  ImageIOType::Pointer gdcm_io = ImageIOType::New();
  InputNamesGeneratorType::Pointer input_names = InputNamesGeneratorType::New();
  input_names->SetInputDirectory(pathname);

  const ReaderType::FileNamesContainer &filenames = input_names->GetInputFileNames();
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(gdcm_io);
  reader->SetFileNames(filenames);

  try
  {
    reader->Update();
  } 
  catch (itk::ExceptionObject &exp) 
  {
    std::cerr << "Failed to read dicom dir: " << pathname << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  this->image = reader->GetOutput();
  return true;
}

/// write
///
/// writes image
///
/// \param filename
/// \param compressed
bool Image::write(const std::string &filename, bool compressed)
{
  if (!this->image)
  {
    std::cerr << "No image to write, so returning false." << std::endl;
    //todo: just return
    return false;
  }
  if (filename.empty())
  {
    std::cerr << "Empty filename passed to write; returning false." << std::endl;
    return false;
  }

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->image);
  writer->SetFileName(filename);
  writer->SetUseCompression(compressed);

  try
  {
    writer->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Failed to write image to " << filename << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Successfully wrote image " << filename << std::endl;
#endif
  return true;
}

/// antialias
///
/// antialiases binary volumes
///
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

/// recenter
///
/// recenters by changing origin (in the image header) to the physcial coordinates of the center of the image
///
bool Image::recenter()
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(this->image);
  filter->CenterImageOn();
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

/// isoresample
///
/// create an isotropic resampling of the given volume (resample accepts only continuous images, so probably antialias binary images first)
///
/// \param isoSpacing     size of an output voxel [default 1.0)
/// \param outputSize     image size can be changed [default stays the same]
bool Image::isoresample(double isoSpacing, Dims outputSize)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using ResampleFilter = itk::ResampleImageFilter<ImageType, ImageType>;
  ResampleFilter::Pointer resampler = ResampleFilter::New();

  double spacing[] = { isoSpacing, isoSpacing, isoSpacing };
  resampler->SetOutputSpacing(spacing);
  resampler->SetOutputOrigin(image->GetOrigin());
  resampler->SetOutputDirection(image->GetDirection());

  if (outputSize[0] == 0 || outputSize[1] == 0 || outputSize[2] == 0)
  {
    ImageType::SizeType inputSize = image->GetLargestPossibleRegion().GetSize();
    ImageType::SpacingType inputSpacing = image->GetSpacing();
    outputSize[0] = std::floor(inputSize[0] * inputSpacing[0] / isoSpacing);
    outputSize[1] = std::floor(inputSize[1] * inputSpacing[1] / isoSpacing);
    outputSize[2] = std::floor(inputSize[2] * inputSpacing[2] / isoSpacing);
  }
  resampler->SetSize(outputSize);
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

/// operator ==
///
/// compares two images to see if they are identical
///
/// \param  Image   other image to compare
bool Image::operator==(const Image &other) const
{
  // we use the region of interest filter here with the full region because our
  // incoming image may be the output of an ExtractImageFilter or PadImageFilter
  // which modify indices and leave the origin intact.  These will not compare
  // properly against a saved NRRD file because the act of saving the image to
  // NRRD and back in will cause the origin (and indices) to be reset.
  using RegionFilterType = itk::RegionOfInterestImageFilter<ImageType, ImageType>;
  RegionFilterType::Pointer region_filter = RegionFilterType::New();
  region_filter->SetInput(this->image);
  region_filter->SetRegionOfInterest(this->image->GetLargestPossibleRegion());
  region_filter->UpdateLargestPossibleRegion();
  ImageType::Pointer itk_image = region_filter->GetOutput();

  // perform the same to the other image
  RegionFilterType::Pointer region_filter2 = RegionFilterType::New();
  region_filter2->SetInput(other.image);
  region_filter2->SetRegionOfInterest(other.image->GetLargestPossibleRegion());
  region_filter2->UpdateLargestPossibleRegion();
  ImageType::Pointer other_itk_image = region_filter2->GetOutput();

  using DiffType = itk::Testing::ComparisonImageFilter<ImageType, ImageType>;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(other_itk_image);
  diff->SetTestInput(itk_image);
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

bool Image::is_directory(const std::string &pathname)
{
  struct stat info;
  if (stat(pathname.c_str(), &info) != 0) {
    return false;
  }
  else if (info.st_mode & S_IFDIR) {
    return true;
  }
  return false;
}

/// pad
///
/// pads an image with constant value
///
/// \param padding  Number of voxels to be padded in each direction
/// \param value    Value to be used to fill padded voxels
bool Image::pad(int padding, PixelType value)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  ImageType::SizeType lowerExtendRegion;
  lowerExtendRegion[0] = padding;
  lowerExtendRegion[1] = padding;
  lowerExtendRegion[2] = padding;

  ImageType::SizeType upperExtendRegion;
  upperExtendRegion[0] = padding;
  upperExtendRegion[1] = padding;
  upperExtendRegion[2] = padding;

  using FilterType = itk::ConstantPadImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(this->image);
  filter->SetPadLowerBound(lowerExtendRegion);
  filter->SetPadUpperBound(upperExtendRegion);
  filter->SetConstant(value);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Pad image failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Pad image succeeded!\n";
#endif
  return true;
}

bool Image::extractLabel(PixelType label)
{
  threshold(label, label);

#if DEBUG_CONSOLIDATION
  std::cout << "Extract Label from Image succeeded!\n";
#endif
  return true;
}

bool Image::closeHoles()
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::BinaryFillholeImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(this->image);
  filter->SetForegroundValue(itk::NumericTraits<PixelType>::min());
  this->image = filter->GetOutput();

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Close Holes failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Close Holes succeeded!\n";
#endif
  return true;
}

bool Image::threshold(PixelType min, PixelType max)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(this->image);
  filter->SetLowerThreshold(min);
  filter->SetUpperThreshold(max);
  filter->SetInsideValue(1.0);
  filter->SetOutsideValue(0.0);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Threshold failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Threshold succeeded!\n";
#endif
  return true;
}

bool Image::fastMarch(float isoValue)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::ReinitializeLevelSetImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(this->image);
  filter->NarrowBandingOff();
  filter->SetLevelSetValue(isoValue);
  this->image = filter->GetOutput();

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Fast March failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Fast March succeeded!\n";
#endif
  return true;
}

bool Image::cropImage(FPoint3 desiredStart, FPoint3 desiredSize)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  // ImageType::RegionType desiredRegion(desiredStart, desiredSize);

  ImageType::RegionType desiredRegion();

  using FilterType = itk::ExtractImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  // filter->SetExtractionRegion(desiredRegion);
  filter->SetInput(this->image);
  filter->SetDirectionCollapseToIdentity();
  this->image = filter->GetOutput();

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Crop Image failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Crop Image succeeded!\n";
#endif
  return true;
}

/// centerOfMass
///
/// returns average spatial coordinate of black pixels in a binary volume
Point3 Image::centerOfMass() const
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  Point3 mean;

  itk::ImageRegionIteratorWithIndex<ImageType> imageIt(this->image, image->GetLargestPossibleRegion());
  int numPixels = 0;

  while (!imageIt.IsAtEnd())
  {
    PixelType val = imageIt.Get();
    ImageType::IndexType index;
    ImageType::PointType point;
    index = imageIt.GetIndex();

    if (val == 1)
    {
      numPixels += 1;
      image->TransformIndexToPhysicalPoint(index, point);
      mean[0] += point[0];
      mean[1] += point[1];
      mean[2] += point[2];
    }
    ++imageIt;
  }

  mean[0] /= static_cast<double>(numPixels);
  mean[1] /= static_cast<double>(numPixels);
  mean[2] /= static_cast<double>(numPixels);

  return mean;
}

/// size
///
/// return physical size of image
Point3 Image::size() const
{
  Dims dims(image->GetLargestPossibleRegion().GetSize());
  Point3 spacing(image->GetSpacing());
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = dims[i] * spacing[i];
  return ret;
}

void Image::print() const
{
  std::cout << "this image is really pretty... (todo)\n";
}

} // Shapeworks
