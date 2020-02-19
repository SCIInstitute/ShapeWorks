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
// #include <itkTranslationTransform.h>

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

//need to call antialias before and isoresample after for binary images
bool Image::centerofmassalign(bool useCenterOfMass, float centerX, float centerY, float centerZ, const std::string &dataFilename)
{
  if (!this->image)
  {
    std::cerr << "No image loaded, so returning false." << std::endl;
    return false;
  }

  double imageCenterX, imageCenterY, imageCenterZ;
  const unsigned int Dimension = 3;

  using TransformType = itk::TranslationTransform<double, Dimension>;
  TransformType::Pointer transform = TransformType::New();
  TransformType::OutputVectorType translation;

  if(useCenterOfMass == true)
  {
    // getting the origin of the shape
    itk::ImageRegionIteratorWithIndex <ImageType> imageIt(this->image, image->GetLargestPossibleRegion());
    float numPixels = 0.0, meanX = 0.0, meanY = 0.0, meanZ = 0.0;
    while(!imageIt.IsAtEnd())
    {
        PixelType val = imageIt.Get();
        ImageType::IndexType index;
        ImageType::PointType point;
        index = imageIt.GetIndex();

        if(val == 1)
        {
            numPixels = numPixels+1;
            image->TransformIndexToPhysicalPoint(index, point);
            meanX = meanX + point[0];
            meanY = meanY + point[1];
            meanZ = meanZ + point[2];
        }
        ++imageIt;
    }

    meanX = meanX/numPixels;
    meanY = meanY/numPixels;
    meanZ = meanZ/numPixels;

    imageCenterX = meanX;
    imageCenterY = meanY;
    imageCenterZ = meanZ;
  }
  else  /*using center of ostium coordinates*/
  {
    imageCenterX = centerX;
    imageCenterY = centerY;
    imageCenterZ = centerZ;
  }

  ImageType::PointType origin = image->GetOrigin();
  ImageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

  ImageType::IndexType index;
  ImageType::PointType point;
  ImageType::PointType center;

  index[0] = 0; index[1] = 0; index[2] = 0;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] = point[0]; center[1] = point[1]; center[2] = point[2];

  index[0] = 0; index[1] = 0; index[2] = size[2]-1;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = 0; index[1] = size[1]-1; index[2] = 0;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = 0; index[1] = size[1]-1; index[2] = size[2]-1;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = size[0]-1; index[1] = 0; index[2] = 0;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = size[0]-1; index[1] = 0; index[2] = size[2]-1;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = size[0]-1; index[1] = size[1]-1; index[2] = 0;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  index[0] = size[0]-1; index[1] = size[1]-1; index[2] = size[2]-1;
  image->TransformIndexToPhysicalPoint(index, point);
  center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

  center[0] /= 8.0; center[1] /= 8.0; center[2] /= 8.0;

  // move object's origin to zero then move to the region center
  translation[0] = -1*(-imageCenterX + center[0]);
  translation[1] = -1*(-imageCenterY + center[1]);
  translation[2] = -1*(-imageCenterZ + center[2]);

  // transform->Translate(translation);

  if (dataFilename.empty())
  {
    std::cerr << "Empty filename passed to write data; returning false." << std::endl;
    return false;
  }

  std::ofstream ofs;
  std::string fname = dataFilename;
  const char *filename = fname.c_str();
  ofs.open(filename);

  ofs << "translation:" << translation[0] << " " << translation[1] << " " << translation[2] << "\n";
  ofs << "origin:" << origin[0] << " " << origin[1] << " " << origin[2] << "\n";
  ofs << "object center:" << imageCenterX << " " << imageCenterY << " " << imageCenterZ << "\n";
  ofs << "image center:" << center[0] << " " << center[1] << " " << center[2] << "\n";

  ofs.close();

  try
  {
    // transform->Update();
    transform->Translate(translation);
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Center of mass alignment failed:" << std::endl;
    std::cerr << exp << std::endl;
    return false;
  }

  std::cout << "Center of mass alignment succeeded!\n";
  return true;

}

} // Shapeworks
