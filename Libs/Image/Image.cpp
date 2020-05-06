#include "Image.h"
#include "ShapeworksUtils.h"
#include "itkTPGACLevelSetImageFilter.h"  // actually a shapeworks class, not itk

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

namespace shapeworks {

Image::ImageType::Pointer Image::cloneData(const Image::ImageType::Pointer image)
{
  using DuplicatorType = itk::ImageDuplicator<ImageType>;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(image);
  duplicator->Update();
  return duplicator->GetOutput();
}

Image& Image::operator=(const Image &img)
{
  this->image = Image::cloneData(image);
  return *this;
}

Image& Image::operator=(Image &&img)
{
  this->image = nullptr;        // make sure to free existing image by setting it to nullptr (works b/c it's a smart ptr)
  this->image.Swap(img.image);
  return *this;
}

Image::ImageType::Pointer Image::read(const std::string &pathname)
{
  if (pathname.empty()) { throw std::invalid_argument("Empty pathname"); }

  if (ShapeworksUtils::is_directory(pathname))
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

  return reader->GetOutput();
}

Image::ImageType::Pointer Image::readDICOMImage(const std::string &pathname)
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

  return reader->GetOutput();
}

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

Image& Image::antialias(unsigned numIterations, double maxRMSErr, unsigned numLayers)
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

Transform Image::translate(const Vector3 &v) const
{
  Transform xform;
  xform->Translate(v);

  return xform;
}

Transform Image::scale(const Vector3 &v)
{
  Transform xform;
  xform->Scale(v);

  return xform;
}

Transform Image::rotate(const Vector3 &v, const double angle)
{
  Transform xform;
  xform->Rotate3D(v, angle);

  return xform;
}

Image& Image::applyTransform(const Transform &transform)
{
  using FilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  FilterType::Pointer resampler = FilterType::New();

  using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();

  resampler->SetInterpolator(interpolator);
  resampler->SetTransform(transform);
  resampler->SetInput(this->image);
  resampler->SetSize(dims());
  std::cout << dims();
  resampler->SetOutputOrigin(origin());
  resampler->SetOutputDirection(image->GetDirection());
  resampler->SetOutputSpacing(image->GetSpacing());
  resampler->Update();
  this->image = resampler->GetOutput();

  return *this;
}

Image& Image::extractLabel(PixelType label)
{
  threshold(label, label);

  return *this;
}

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

Image& Image::threshold(PixelType minval, PixelType maxval)
{
  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetLowerThreshold(minval);
  filter->SetUpperThreshold(maxval);
  filter->SetInsideValue(1.0);
  filter->SetOutsideValue(0.0);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

Image& Image::computeDT(PixelType isoValue)
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

Image& Image::applyGradientFilter()
{
  using FilterType = itk::GradientMagnitudeImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter  = FilterType::New();

  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();
  
  return *this;
}

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

Image::Region Image::boundingBox(PixelType isoValue) const
{
  Image::Region bbox;

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(image, image->GetLargestPossibleRegion());
  while (!imageIterator.IsAtEnd())
  {
    PixelType val = imageIterator.Get();

    if (val <= isoValue)
      bbox.expand(imageIterator.GetIndex());

    ++imageIterator;
  }

  return bbox;
}

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

vtkSmartPointer<vtkPolyData> Image::getPolyData(const Image &img, PixelType isoValue)
{
  using FilterType = itk::VTKImageExport<ImageType>;
  FilterType::Pointer itkTargetExporter = FilterType::New();
  itkTargetExporter->SetInput(img.image);

  vtkImageImport *vtkTargetImporter = vtkImageImport::New();
  ShapeworksUtils::connectPipelines(itkTargetExporter, vtkTargetImporter);
  vtkTargetImporter->Update();

  vtkContourFilter *targetContour = vtkContourFilter::New();
  targetContour->SetInputData(vtkTargetImporter->GetOutput());
  targetContour->SetValue(0, isoValue);
  targetContour->Update();

  return targetContour->GetOutput();
}

Image& Image::clip(const Point &o, const Point &p1, const Point &p2, const PixelType val)
{
  // clipping plane normal n = (p1-o) x (p2-o)
  Vector v1(makeVector({p1[0] - o[0], p1[1] - o[1], p1[2] - o[2]}));
  Vector v2(makeVector({p2[0] - o[0], p2[1] - o[1], p2[2] - o[2]}));

  return clip(cross(v1, v2), o, val);
}

Image& Image::clip(const Vector &n, const Point &q, const PixelType val)
{
  const double eps = 1E-6;
  if (n.GetSquaredNorm() < eps) { throw std::invalid_argument("invalid clipping plane (zero length normal)"); }

  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    Point p({static_cast<double>(iter.GetIndex()[0]), static_cast<double>(iter.GetIndex()[1]), static_cast<double>(iter.GetIndex()[2])});
    Vector pq(p - q);

    // if n dot pq is < 0, point q is on the back side of the plane.
    if (n * pq < 0.0)
      iter.Set(val);
      
    ++iter;
  }

  return *this;
}

Image& Image::reflect(const Vector3 &normal)
{
  Matrix reflection;
  reflection.Fill(0);
  reflection[0][0] = -normal[0];
  reflection[1][1] = -normal[1];
  reflection[2][2] = -normal[2];

  Transform xform;
  xform->SetMatrix(reflection);
  Point3 currentOrigin(origin());
  recenter().applyTransform(xform).setOrigin(currentOrigin);

  return *this;
}

Image& Image::setOrigin(Point3 origin)
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

Point3 Image::size() const
{
  Dims dims(image->GetLargestPossibleRegion().GetSize());
  Point3 spacing(image->GetSpacing());
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = dims[i] * spacing[i];
  return ret;
}

std::ostream& operator<<(std::ostream &os, const Image::Region &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

std::ostream& operator<<(std::ostream &os, const Image &img)
{
  return os << "{\n\tdims: " << img.dims() << ",\n\torigin: "
            << img.origin() << ",\n\tsize: " << img.size() << "\n}";
}

} // shapeworks
