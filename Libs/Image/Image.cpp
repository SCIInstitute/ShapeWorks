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
#include <itkRegionOfInterestImageFilter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include "itkThinPlateSplineKernelTransform.h"
#include "itkPointSet.h"

#include <sys/stat.h>

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

  if (Image::is_directory(inFilename))
  {
    return this->read_image_dir(inFilename);
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
  } catch (itk::ExceptionObject &exp) {
    std::cerr << "Failed to read dicom dir: " << pathname << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

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
/// recenters by changing origin (in the image header) to the physical coordinates of the center of the image
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
/// create an isotropic resampling of the given volume
/// resample accepts only continuous images, so probably antialias binary images first.
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

///////////////////////////////////////////////////////////////////////////////
bool Image::compare_equal(const Image &other)
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

  using PadFilter = itk::ConstantPadImageFilter<ImageType, ImageType>;
  PadFilter::Pointer padFilter = PadFilter::New();

  padFilter->SetInput(this->image);
  padFilter->SetPadLowerBound(lowerExtendRegion);
  padFilter->SetPadUpperBound(upperExtendRegion);
  padFilter->SetConstant(value);
  this->image = padFilter->GetOutput();

  try
  {
    padFilter->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Pad image with constant failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Pad image with constant succeeded!\n";
#endif
  return true;
}

bool Image::warp(std::string &source_file, std::string &target_file, int pointFactor)
{ 
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }
  // typedef double CoordinateRepType;
  // typedef itk::ThinPlateSplineKernelTransform< CoordinateRepType, ImageDimension> TransformType;
  // typedef   itk::Point< CoordinateRepType, ImageDimension >  PointType;
  // typedef   std::vector< PointType > PointArrayType;
  // typedef   TransformType::PointSetType PointSetType;
  // typedef   PointSetType::Pointer  PointSetPointer;
  // typedef   PointSetType::PointIdentifier PointIdType;
  using ResamplerType = itk::ResampleImageFilter< ImageType, ImageType>;
  typedef   itk::LinearInterpolateImageFunction< ImageType, double > InterpolatorType;
  // first need to read the source and target container
  PointSetType::Pointer sourceLandMarks = PointSetType::New();
  PointSetType::Pointer targetLandMarks = PointSetType::New();
  PointType p1;     
  PointType p2;
  PointSetType::PointsContainer::Pointer sourceLandMarkContainer = sourceLandMarks->GetPoints();
  PointSetType::PointsContainer::Pointer targetLandMarkContainer = targetLandMarks->GetPoints();
  PointIdType id = itk::NumericTraits< PointIdType >::Zero;
  std::ifstream insourcefile;
  std::ifstream intargetfile;
  insourcefile.open( source_file.c_str());
  intargetfile.open( target_file.c_str());
  int count = 0;

  while (!insourcefile.eof() && !intargetfile.eof()){
      insourcefile >>  p1[0] >> p1[1] >> p1[2];
      intargetfile >> p2[0] >> p2[1] >>p2[2];
      if(count % pointFactor == 0){
      targetLandMarkContainer->InsertElement( id, p2 );
      sourceLandMarkContainer->InsertElement( id, p1 );
      id++;
      }
      count++;
  }
  insourcefile.close();
  intargetfile.close();

  // now perform the warping
  TransformType::Pointer tps = TransformType::New();
  tps->SetSourceLandmarks(sourceLandMarks);
  tps->SetTargetLandmarks(targetLandMarks);
  tps->ComputeWMatrix();
  // Set the resampler params
  ResamplerType::Pointer resampler = ResamplerType::New();
  InterpolatorType::Pointer interpolator = InterpolatorType::New();
  resampler->SetInterpolator( interpolator );
  ImageType::SpacingType spacing = image->GetSpacing();
  ImageType::PointType   origin  = image->GetOrigin();
  ImageType::DirectionType direction  = image->GetDirection();
  ImageType::RegionType region = image->GetBufferedRegion();
  ImageType::SizeType   size =  region.GetSize();
  resampler->SetOutputSpacing( spacing );
  resampler->SetOutputDirection( direction );
  resampler->SetOutputOrigin(  origin  );
  resampler->SetSize( size );
  resampler->SetTransform( tps );
  resampler->SetOutputStartIndex(  region.GetIndex() );
  resampler->SetInput( this->image );
  this->image = resampler->GetOutput();

  try
  {
    resampler->Update();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Pad image with constant failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

#if DEBUG_CONSOLIDATION
  std::cout << "Pad image with constant succeeded!\n";
#endif
  return true;

}
} // Shapeworks
