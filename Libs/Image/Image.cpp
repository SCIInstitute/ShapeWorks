#include "Image.h"
#include "utils.h"
#include "itkTPGACLevelSetImageFilter.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkAntiAliasBinaryImageFilter.h>
#include <itkResampleImageFilter.h>
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
#include <itkVTKImageExport.h>
#include <vtkImageImport.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>

#include <exception>
#include <cmath>

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
    return readDICOMImage(pathname);

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(pathname);

  try {
    reader->Update();
  }
  catch (itk::ExceptionObject &exp) {
    throw std::invalid_argument(pathname + " does not exist (" + std::string(exp.what()) + ")");
  }

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
Image& Image::write(const std::string &filename, bool compressed)
{
  if (!this->image) { throw std::invalid_argument("Image invalid"); }
  if (filename.empty()) { throw std::invalid_argument("Empty pathname"); }

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->image);
  writer->SetFileName(filename);
  writer->SetUseCompression(compressed);
  writer->Update();

  return *this;
}

/// antialias
///
/// antialiases binary volumes
///
/// \param numIterations
/// \param maxRMSErr      range [0.0, 1.0], determines how fast the solver converges (larger is faster)
/// \param numLayers      size of region around a pixel to sample
Image& Image::antialias(unsigned numIterations, float maxRMSErr, unsigned numLayers)
{
  using FilterType = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetMaximumRMSError(maxRMSErr);
  filter->SetNumberOfIterations(numIterations);
  if (numLayers)
    filter->SetNumberOfLayers(numLayers);
  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// recenter
///
/// recenters by changing origin (in the image header) to the physcial coordinates of the center of the image
///
Image& Image::recenter()
{
  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->CenterImageOn();
  filter->Update();
  this->image = filter->GetOutput();
  
  return *this;
}

Image& Image::resample(const Point3& spacing, Dims outputSize)
{
  using ResampleFilter = itk::ResampleImageFilter<ImageType, ImageType>;
  ResampleFilter::Pointer resampler = ResampleFilter::New();

  resampler->SetOutputSpacing(spacing.GetDataPointer());
  resampler->SetOutputOrigin(origin());
  resampler->SetOutputDirection(image->GetDirection());

  if (outputSize[0] == 0 || outputSize[1] == 0 || outputSize[2] == 0)
  {
    ImageType::SizeType inputSize = image->GetLargestPossibleRegion().GetSize();
    ImageType::SpacingType inputSpacing = image->GetSpacing();
    outputSize[0] = std::floor(inputSize[0] * inputSpacing[0] / spacing[0]);
    outputSize[1] = std::floor(inputSize[1] * inputSpacing[1] / spacing[1]);
    outputSize[2] = std::floor(inputSize[2] * inputSpacing[2] / spacing[2]);
  }

  resampler->SetSize(outputSize);
  resampler->SetInput(this->image);
  resampler->Update();
  this->image = resampler->GetOutput();

  return *this;
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
  diff->SetDifferenceThreshold(0.0);
  diff->SetToleranceRadius(0);

  try
  {
    diff->UpdateLargestPossibleRegion();
  } 
  catch (itk::ExceptionObject &exp) 
  {
    std::cerr << "Comparison failed" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  const unsigned long numberOfPixelsWithDifferences = diff->GetNumberOfPixelsWithDifferences();

  if (numberOfPixelsWithDifferences > 0) {
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
Image& Image::pad(int padding, PixelType value)
{
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
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// applyTransform
///
/// applies the computed transformation to the image by using resampling filter
///
/// \param transform      computed transformation    
Image& Image::applyTransform(const Transform::Pointer &transform)
{
  using FilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  FilterType::Pointer resampler = FilterType::New();

  using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator(interpolator);
  resampler->SetTransform(transform);
  resampler->SetInput(this->image);
  resampler->SetSize(image->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(origin());
  resampler->SetOutputDirection(image->GetDirection());
  resampler->SetOutputSpacing(image->GetSpacing());
  resampler->Update();
  this->image = resampler->GetOutput();

  return *this;
}

/// extractLabel
///
/// extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image
///
/// \param label      label value which has to be extracted. [default 1.0]
Image& Image::extractLabel(PixelType label)
{
  threshold(label, label);

  return *this;
}

/// closeHoles
///
/// closes holes in a given binary volume
Image& Image::closeHoles()
{
  using FilterType = itk::BinaryFillholeImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetForegroundValue(itk::NumericTraits<PixelType>::min());
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// threshold
///
/// threholds image into binary label based on upper and lower intensity bounds given by user
///
/// \param min      lower threshold level (optional, default = epsilon)
/// \param max      upper threshold level (optional, default = FLT_MAX)
Image& Image::threshold(PixelType min, PixelType max)
{
  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetLowerThreshold(min);
  filter->SetUpperThreshold(max);
  filter->SetInsideValue(1.0);
  filter->SetOutsideValue(0.0);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// computeDT
///
/// computes distance transform volume from a binary (antialiased) image
///
/// \param isoValue     level set value that defines the interface between foreground and background
Image& Image::computeDT(float isoValue)
{
  using FilterType = itk::ReinitializeLevelSetImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->NarrowBandingOff();
  filter->SetLevelSetValue(isoValue);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// applyCurvatureFilter
///
/// applies curvature flow image filter
///
/// \param iterations     number of iterations
Image& Image::applyCurvatureFilter(unsigned iterations)
{
  using FilterType = itk::CurvatureFlowImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetTimeStep(0.0625);
  filter->SetNumberOfIterations(iterations);
  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// applyGradientFilter
///
/// applies gradient magnitude image filter
Image& Image::applyGradientFilter()
{
  using FilterType = itk::GradientMagnitudeImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter  = FilterType::New();

  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();
  
  return *this;
}

/// applySigmoidFilter
///
/// applies sigmoid image filter
///
/// \param alpha     value of alpha
/// \param beta      value of beta
Image& Image::applySigmoidFilter(double alpha, double beta)
{
  using FilterType = itk::SigmoidImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetAlpha(alpha);
  filter->SetBeta(beta);
  filter->SetOutputMinimum(0.0);
  filter->SetOutputMaximum(1.0);
  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// applyTPLevelSetFilter
///
/// applies TPLevelSet level set image filter
///
/// \param featureImage     path of feature image for filter
/// \param scaling          value of scale [default: 20]
Image& Image::applyTPLevelSetFilter(const Image &featureImage, double scaling)
{
  using FilterType = itk::TPGACLevelSetImageFilter<ImageType, ImageType>; // TODO: this is no longer part of ITK and should be updated
  FilterType::Pointer filter = FilterType::New();

  filter->SetPropagationScaling(scaling);
  filter->SetCurvatureScaling(1.0);
  filter->SetAdvectionScaling(1.0);
  filter->SetMaximumRMSError(0.0);
  filter->SetNumberOfIterations(20);
  filter->SetInput(this->image);
  filter->SetFeatureImage(featureImage.image);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// gaussianBlur
///
/// applies gaussian blur
///
/// \param sigma      value of sigma
Image& Image::gaussianBlur(double sigma)
{
  using BlurType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;
  BlurType::Pointer blur = BlurType::New();

  blur->SetInput(this->image);
  blur->SetVariance(sigma * sigma);
  blur->Update();
  this->image = blur->GetOutput();

  return *this;
}

/// binaryBoundingBox
///
/// computes the logical coordinates of the largest region of binary data within these images
///
/// \param filenames      the set of images to load, all of which must have identical dimensions.
/// \param padding      the amount of padding to add in all directions to this bounding box
Image::Region Image::binaryBoundingBox(std::vector<std::string> &filenames, int padding)
{
  Image::Region bbox;
  Dims dims = read(filenames[0]).dims(); // make sure all images are the same size

  for (auto filename : filenames)
  {
    Image img(filename);

    if (img.dims() != dims) { throw std::invalid_argument("image sizes do not match (" + filename + ")"); }

    itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(img.image, img.image->GetLargestPossibleRegion());
    while (!imageIterator.IsAtEnd())
    {
      PixelType val = imageIterator.Get();

      if (val == 1.0)
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

  // ensure bounding box is not larger than the images themselves
  bbox.min[0] = std::max(0, bbox.min[0] - padding);
  bbox.min[1] = std::max(0, bbox.min[1] - padding);
  bbox.min[2] = std::max(0, bbox.min[2] - padding);
  bbox.max[0] = std::min(bbox.max[0] + padding, (int)dims[0]);
  bbox.max[1] = std::min(bbox.max[1] + padding, (int)dims[1]);
  bbox.max[2] = std::min(bbox.max[2] + padding, (int)dims[2]);

  return bbox;
}

/// crop
///
/// performs translational alignment of shape image based on its center of mass or given 3D point
///
/// \param region     computed region to perform crop
Image& Image::crop(const Region &region)
{
  if (!region.valid())
    std::cerr << "Invalid region specified." << std::endl;

  using FilterType = itk::ExtractImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetExtractionRegion(region);
  filter->SetInput(this->image);
  filter->SetDirectionCollapseToIdentity();
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

vtkSmartPointer<vtkPolyData> Image::convert(const Image &img, float isoValue)
{
  using FilterType = itk::VTKImageExport<ImageType>;
  FilterType::Pointer itkTargetExporter = FilterType::New();
  itkTargetExporter->SetInput(img.image);

  vtkImageImport *vtkTargetImporter = vtkImageImport::New();
  connectPipelines(itkTargetExporter, vtkTargetImporter);
  vtkTargetImporter->Update();

  vtkContourFilter *targetContour = vtkContourFilter::New();
  targetContour->SetInputData(vtkTargetImporter->GetOutput());
  targetContour->SetValue(0, isoValue);
  targetContour->Update();

  return targetContour->GetOutput();
}

Image& Image::clip(Matrix cuttingPlane)
{
  Point3 spacing = image->GetSpacing();
  Point3 curOrigin = origin();
  double o[] = {0.0, 0.0, 0.0};
  double p1[] = {0.0, 0.0, 0.0};
  double p2[] = {0.0, 0.0, 0.0};
  double res[] = {0.0, 0.0, 0.0};
  double temp = 0.0;

  o[0] = (cuttingPlane[0][0] - curOrigin[0]) / spacing[0];
  o[1] = (cuttingPlane[0][1] - curOrigin[1]) / spacing[1];
  o[2] = (cuttingPlane[0][2] - curOrigin[2]) / spacing[2];
  p1[0] = (cuttingPlane[1][0] - curOrigin[0]) / spacing[0];
  p1[1] = (cuttingPlane[1][1] - curOrigin[1]) / spacing[1];
  p1[2] = (cuttingPlane[1][2] - curOrigin[2]) / spacing[2];
  p2[0] = (cuttingPlane[2][0] - curOrigin[0]) / spacing[0];
  p2[1] = (cuttingPlane[2][1] - curOrigin[1]) / spacing[1];
  p2[2] = (cuttingPlane[2][2] - curOrigin[2]) / spacing[2];

  // find the cross product vector
  res[0] = (p1[1] - o[1]) * (p2[2] - o[2]) - (p1[2] - o[2]) * (p2[1] - o[1]);
  res[1] = (p1[2] - o[2]) * (p2[0] - o[0]) - (p1[0] - o[0]) * (p2[2] - o[2]);
  res[2] = (p1[0] - o[0]) * (p2[1] - o[1]) - (p1[1] - o[1]) * (p2[0] - o[0]);

  // normalize
  res[0] = res[0] / (sqrt(res[0] * res[0] + res[1] * res[1] + res[2] * res[2]));
  res[1] = res[1] / (sqrt(res[0] * res[0] + res[1] * res[1] + res[2] * res[2]));
  res[2] = res[2] / (sqrt(res[0] * res[0] + res[1] * res[1] + res[2] * res[2]));

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(this->image, image->GetLargestPossibleRegion());
  while (!imageIterator.IsAtEnd())
  {
    temp = (double(imageIterator.GetIndex()[0]) - o[0]) * res[0] +
           (double(imageIterator.GetIndex()[1]) - o[1]) * res[1] +
           (double(imageIterator.GetIndex()[2]) - o[2]) * res[2];

    if (temp < 0.0)
      imageIterator.Set(0);
      
    ++imageIterator;
  }

  return *this;
}

Image& Image::changeOrigin(Point3 origin)
{
  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetOutputOrigin(origin);
  filter->ChangeOriginOn();
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

/// logicalToPhysical
///
/// returns voxel coordinate of this physical location, throwing exception if it doesn't exist
Point3 Image::logicalToPhysical(const IPoint3 &v) const
{
  if (!this->image)
  {
    throw std::invalid_argument("Image invalid");
  }

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

Point3 Image::centerOfMass(PixelType minval, PixelType maxval) const
{
  itk::ImageRegionIteratorWithIndex<ImageType> imageIt(this->image, image->GetLargestPossibleRegion());
  int numPixels = 0;
  Point3 com({0.0, 0.0, 0.0});

  while (!imageIt.IsAtEnd())
  {
    PixelType val = imageIt.Get();
    if (val > minval && val <= maxval)
    {
      numPixels++;
      com += image->TransformIndexToPhysicalPoint<double>(imageIt.GetIndex());
    }
    ++imageIt;
  }
  com /= static_cast<double>(numPixels);

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
/// Prints dims, origin, and size of the image
std::ostream& operator<<(std::ostream &os, const Image &img)
{
  return os << "{\n\tdims: " << img.dims() << ",\n\torigin: "
            << img.origin() << ",\n\tsize: " << img.size() << "\n}";
}

} // shapeworks
