#include "Image.h"
#include "Utils.h"
#include "itkTPGACLevelSetImageFilter.h"

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
#include <itkGradientMagnitudeImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImageDuplicator.h>

#include <exception>

// todo: finish converting these functions to throw exceptions when appropriate

//TODO: move this to Utils class (in Libs/Utils) -> tried, but something wrong with getting the right include
//TODO: in C++17 this is a standard function
bool is_directory(const std::string &pathname)
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

namespace shapeworks {

/// copy ctor
///
/// clones the input image
///
/// \param img const input image
Image::Image(const Image &img)
{
  this->image = nullptr;
  
  using DuplicatorType = itk::ImageDuplicator<ImageType>;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(img.image);
  duplicator->Update();
  this->image = duplicator->GetOutput();
}

/// operator=
///
/// assignment operator from const Image
///
/// \param img const input image
Image& Image::operator=(const Image &img)
{
  this->image = nullptr;
    
  using DuplicatorType = itk::ImageDuplicator<ImageType>;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(img.image);
  duplicator->Update();
  this->image = duplicator->GetOutput();

  return *this;
}

/// read
///
/// reads image (private function only used by constructor)
///
/// \param pathname
Image Image::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  if (is_directory(pathname))
  {
    return readDICOMImage(pathname);
  }

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(pathname);

  try {
    reader->Update();
  }
  catch (itk::ExceptionObject &exp) {
    throw std::invalid_argument(pathname + " does not exist (" + std::string(exp.what()) + ")");
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Successfully read image " << pathname << std::endl;
#endif
  return Image(reader->GetOutput());
}

/// readDICOMImage
///
/// reads a DICOM image (private function only used by constructor)
///
/// \param pathname directory containing a DICOM image stack
Image Image::readDICOMImage(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

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

  try {
    reader->Update();
  }
  catch (itk::ExceptionObject &exp) {
    throw std::invalid_argument("Failed to read DICOM from " + pathname + "(" + std::string(exp.what()) + ")");
  }

  return Image(reader->GetOutput());
}

/// write
///
/// writes image
///
/// \param filename
/// \param compressed
bool Image::write(const std::string &filename, bool compressed)
{
  if (!this->image) { throw std::invalid_argument("Image invalid"); }
  if (filename.empty()) { throw std::invalid_argument("Empty pathname"); }

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

  this->image = filter->GetOutput();
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

  this->image = filter->GetOutput();
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

  this->image = resampler->GetOutput();
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

  this->image = filter->GetOutput();
  return true;
}

bool Image::applyTransform(const Transform &transform)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  FilterType::Pointer resampler = FilterType::New();

  using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator(interpolator);
  resampler->SetTransform(transform.getItkTransform());
  resampler->SetInput(this->image);
  resampler->SetSize(image->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(image->GetOrigin());
  resampler->SetOutputDirection(image->GetDirection());
  resampler->SetOutputSpacing(image->GetSpacing());

  try
  {
    resampler->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Transform failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }
#if DEBUG_CONSOLIDATION
  std::cout << "Transform succeeded!\n";
#endif

  this->image = resampler->GetOutput();
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

  this->image = filter->GetOutput();
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

  this->image = filter->GetOutput();
  return true;
}

bool Image::computeDT(float isoValue)
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

  this->image = filter->GetOutput();
  return true;
}

bool Image::applyCurvatureFilter(unsigned iterations)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
  }

  using FilterType = itk::CurvatureFlowImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetTimeStep(0.0625);
  filter->SetNumberOfIterations(iterations);
  filter->SetInput(this->image);

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Curvature Flow failed:" << std::endl;
    std::cerr << exp << std::endl;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Curvature Flow succeeded!\n";
#endif

  this->image = filter->GetOutput();
  return true;
}

bool Image::applyGradientFilter()
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::GradientMagnitudeImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter  = FilterType::New();

  filter->SetInput(this->image);

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Gradient Magnitude failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Gradient Magnitude succeeded!\n";
#endif

  this->image = filter->GetOutput();
  return true;
}

bool Image::applySigmoidFilter(double alpha, double beta)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::SigmoidImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetAlpha(alpha);
  filter->SetBeta(beta);
  filter->SetOutputMinimum(0.0);
  filter->SetOutputMaximum(1.0);
  filter->SetInput(this->image);

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Sigmoid failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Sigmoid succeeded!\n";
#endif

  this->image = filter->GetOutput();
  return true;
}

bool Image::applyTPLevelSetFilter(const Image &featureImage, double scaling)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using FilterType = itk::TPGACLevelSetImageFilter<ImageType, ImageType>; // TODO: this is no longer part of ITK and should be updated
  FilterType::Pointer filter = FilterType::New();

  filter->SetPropagationScaling(scaling);
  filter->SetCurvatureScaling(1.0);
  filter->SetAdvectionScaling(1.0);
  filter->SetMaximumRMSError(0.0);
  filter->SetNumberOfIterations(20);
  filter->SetInput(this->image);
  filter->SetFeatureImage(featureImage.image);

  try
  {
    filter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Level Set failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Level Set succeeded!\n";
#endif

  this->image = filter->GetOutput();
  return true;
}

bool Image::gaussianBlur(double sigma)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  using BlurType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;
  BlurType::Pointer blur = BlurType::New();
  blur->SetInput(this->image);
  blur->SetVariance(sigma * sigma);
  
  try
  {
    blur->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Gaussian Blur failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Gaussian Blur succeeded!\n";
#endif

  this->image = blur->GetOutput();
  return true;
}

/// binaryBoundingBox
///
/// computes the logical coordinates of the largest region of binary data within these images
///
/// \param filenames the set of images to load, all of which must have identical dimensions.
/// \padding the amount of padding to add in all directions to this bounding box 
Image::Region Image::binaryBoundingBox(std::vector<std::string> &filenames, int padding)
{
  Image::Region bbox;
  
  Dims dims = read(filenames[0]).dims(); // make sure all images are the same size

  for (auto filename : filenames)
  {
    Image img(filename);

    if (img.dims() != dims) { throw std::invalid_argument("image sizes do not match (" + filename + ")"); }

    itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(this->image, image->GetLargestPossibleRegion());
    while (!imageIterator.IsAtEnd())
    {
      PixelType val = imageIterator.Get();

      if(val == 1.0)
      {
        bbox.min[0] = std::min(bbox.min[0], (int)imageIterator.GetIndex()[0]);
        bbox.min[1] = std::min(bbox.min[1], (int)imageIterator.GetIndex()[1]);
        bbox.min[2] = std::min(bbox.min[2], (int)imageIterator.GetIndex()[2]);

        bbox.max[0] = std::max(bbox.max[0], (int)imageIterator.GetIndex()[0]);
        bbox.max[1] = std::max(bbox.max[1], (int)imageIterator.GetIndex()[1]);
        bbox.max[2] = std::max(bbox.max[2], (int)imageIterator.GetIndex()[2]);
      }
      ++imageIterator;
    }
  }

  bbox.min[0] = std::max(0, bbox.min[0] - padding);
  bbox.min[1] = std::max(0, bbox.min[1] - padding);
  bbox.min[2] = std::max(0, bbox.min[2] - padding);
  bbox.max[0] = std::min(bbox.max[0] + padding, (int)dims[0]);
  bbox.max[1] = std::min(bbox.max[1] + padding, (int)dims[1]);
  bbox.max[2] = std::min(bbox.max[2] + padding, (int)dims[2]);

#if DEBUG_CONSOLIDATION
  std::cout << "binaryBoundingBox succeeded: " << bbox << "!\n";
#endif
  return bbox;
}

/// crop
bool Image::crop(const Region &region)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  if (!region.valid())
  {
    std::cerr << "Invalid region specified." << std::endl;
    return false;
  }

  ImageType::IndexType desiredStart;
  desiredStart[0] = region.min[0];
  desiredStart[1] = region.min[1];
  desiredStart[2] = region.min[2];

  ImageType::SizeType desiredSize;
  desiredSize[0] = region.max[0];
  desiredSize[1] = region.max[1];
  desiredSize[2] = region.max[2];

  ImageType::RegionType desiredRegion(desiredStart, desiredSize);

  using FilterType = itk::ExtractImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetExtractionRegion(desiredRegion);
  filter->SetInput(this->image);
  filter->SetDirectionCollapseToIdentity();

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

  this->image = filter->GetOutput();
  return true;
}

/// logicalToPhysical
///
/// returns voxel coordinate of this physical location, throwing exception if it doesn't exist
Point3 Image::logicalToPhysical(const IPoint3 &v) const
{
  if (!this->image) { throw std::invalid_argument("Image invalid"); }

  itk::Index<3> index;
  index[0] = v[0];
  index[1] = v[1];
  index[2] = v[2];
  Point3 value;
  image->TransformIndexToPhysicalPoint(index, value);
  return value;
}

/// physicalToLogical
///
/// returns physical location of this voxel coordinate, throwing exception if it doesn't exist
IPoint3 Image::physicalToLogical(const Point3 &p) const
{
  if (!this->image)
  {
    std::cerr << "No image loaded, throwing an exception." << std::endl;
    throw std::invalid_argument("this is an invalid Image");
  }

  itk::Index<3> coords = image->TransformPhysicalPointToIndex(p);
  IPoint3 icoords;
  icoords[0] = coords[0];
  icoords[1] = coords[1];
  icoords[2] = coords[2];
  return icoords;
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

  Point3 com;

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
      com[0] += point[0];
      com[1] += point[1];
      com[2] += point[2];
    }
    ++imageIt;
  }

  com[0] /= static_cast<double>(numPixels);
  com[1] /= static_cast<double>(numPixels);
  com[2] /= static_cast<double>(numPixels);

  std::cout<<"com: "<<com<<std::endl; //debug
  std::cout<<"val: "<<physicalToLogical(com)<<std::endl; //debug
  std::cout<<"...and back: "<<logicalToPhysical(physicalToLogical(com))<<std::endl; //debug
  return com;
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

/// operator<<
///
/// Stream insertion operator
/// Prints region
std::ostream& operator<<(std::ostream &os, const Image::Region &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

/// operator<<
///
/// Stream insertion operator
/// Print dims, origin, and size of the image.
std::ostream& operator<<(std::ostream &os, const Image &img)
{
  return os << "{\n\tdims: " << img.dims() << ",\n\torigin: "
            << img.origin() << ",\n\tsize: " << img.size() << "\n}";
}

} // Shapeworks
