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
#include <itkScalableAffineTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
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

Image& Image::operator=(const Image& img)
{
  this->image = Image::cloneData(image);
  return *this;
}

Image& Image::operator=(Image&& img)
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

Image& Image::operator-()
{
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    iter.Set(-iter.Value());
    ++iter;
  }

  return *this;
}

Image Image::operator+(const Image& other) const
{
  Image ret(*this);
  ret += other;
  return ret;
}

Image& Image::operator+=(const Image& other)
{
  if (dims() != other.dims()) { throw std::invalid_argument("images must have same logical dims"); }
  
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> otherIter(other.image, other.image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd() && !otherIter.IsAtEnd())
  {
    iter.Set(iter.Value() + otherIter.Value());
    ++iter; ++otherIter;
  }

  return *this;
}

Image Image::operator-(const Image& other) const
{
  Image ret(*this);
  ret -= other;
  return ret;
}

Image& Image::operator-=(const Image& other)
{
  if (dims() != other.dims()) { throw std::invalid_argument("images must have same logical dims"); }
  
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> otherIter(other.image, other.image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd() && !otherIter.IsAtEnd())
  {
    iter.Set(iter.Value() - otherIter.Value());
    ++iter; ++otherIter;
  }

  return *this;
}

Image Image::operator+(const PixelType x) const
{
  Image ret(*this);
  ret += x;
  return ret;
}

Image& Image::operator+=(const PixelType x)
{
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    iter.Set(iter.Value() + x);
    ++iter;
  }

  return *this;
}

Image Image::operator-(const PixelType x) const
{
  Image ret(*this);
  ret -= x;
  return ret;
}

Image& Image::operator-=(const PixelType x)
{
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    iter.Set(iter.Value() - x);
    ++iter;
  }

  return *this;
}

Image Image::operator*(const PixelType x) const
{
  Image ret(*this);
  ret *= x;
  return ret;
}

Image& Image::operator*=(const PixelType x)
{
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    iter.Set(iter.Value() * x);
    ++iter;
  }

  return *this;
}

Image Image::operator/(const PixelType x) const
{
  Image ret(*this);
  ret /= x;
  return ret;
}

Image& Image::operator/=(const PixelType x)
{
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    iter.Set(iter.Value() / x);
    ++iter;
  }

  return *this;
}

template<>
Image operator*(const Image& img, const double x) { return img.operator*(x); }

template<>
Image operator/(const Image& img, const double x) { return img.operator/(x); }

template<>
Image& operator*=(Image& img, const double x) { return img.operator*=(x); }

template<>
Image& operator/=(Image& img, const double x) { return img.operator/=(x); }

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

Image& Image::antialias(unsigned iterations, double maxRMSErr, int layers)
{
  using FilterType = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetMaximumRMSError(maxRMSErr);
  filter->SetNumberOfIterations(iterations);
  if (layers)
    filter->SetNumberOfLayers(layers);
  filter->SetInput(this->image);
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

Image& Image::recenter()
{
  return setOrigin(negate(size() / 2.0));
}

Image& Image::resample(const TransformPtr transform, const Point3 origin, Dims dims, const Vector3 spacing,
                       const ImageType::DirectionType direction, Image::InterpolationType interp)
{
  using FilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  FilterType::Pointer resampler = FilterType::New();

  switch (interp) {
    case Linear:
      // linear interpolation is the default
      break;
    case NearestNeighbor:
    {
      using InterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      break;
    }
    default:
      throw std::invalid_argument("Unknown Image::InterpolationType");
  }

  resampler->SetInput(this->image);
  resampler->SetTransform(transform ? transform : IdentityTransform::New());
  resampler->SetOutputOrigin(origin);
  resampler->SetOutputSpacing(spacing);
  resampler->SetSize(dims);
  resampler->SetOutputDirection(direction);

  resampler->Update();
  this->image = resampler->GetOutput();

  return *this;
}

Image& Image::resample(const Vector3& spacing, Image::InterpolationType interp)
{
  // compute logical dimensions that keep all image data for this spacing
  Dims inputDims(this->dims());
  Vector3 inputSpacing(this->spacing());
  Dims dims({ static_cast<unsigned>(std::floor(inputDims[0] * inputSpacing[0] / spacing[0])),
              static_cast<unsigned>(std::floor(inputDims[1] * inputSpacing[1] / spacing[1])),
              static_cast<unsigned>(std::floor(inputDims[2] * inputSpacing[2] / spacing[2])) });
  
  return resample(IdentityTransform::New(), origin(), dims, spacing, coordsys(), interp);
}

Image& Image::resize(Dims dims, Image::InterpolationType interp)
{
  // use existing dims for any that are unspecified
  Dims inputDims(this->dims());
  if (dims[0] == 0) dims[0] = inputDims[0];
  if (dims[1] == 0) dims[1] = inputDims[1];
  if (dims[2] == 0) dims[2] = inputDims[2];

  // compute new spacing so physical image size remains the same
  Vector3 inputSpacing(spacing());
  Vector3 spacing(makeVector({ inputSpacing[0] * inputDims[0] / dims[0],
                               inputSpacing[1] * inputDims[1] / dims[1],
                               inputSpacing[2] * inputDims[2] / dims[2] }));

  return resample(IdentityTransform::New(), origin(), dims, spacing, coordsys(), interp);
}

bool Image::compare(const Image& other, bool verifyall, double tolerance, double precision) const
{
  if (tolerance > 1 || tolerance < 0) { throw std::invalid_argument("tolerance value must be between 0 and 1 (inclusive)"); }
  
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
  diff->SetDifferenceThreshold(precision);
  diff->SetToleranceRadius(0);
  diff->SetVerifyInputInformation(verifyall);

  try
  {
    diff->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &exp)
  {
    std::cerr << "Comparison failed: " << exp.GetDescription() << std::endl;

    // if metadata differs but dims do not, re-run compare to identify pixel differences (but still return false)
    if (std::string(exp.what()).find("Inputs do not occupy the same physical space!") != std::string::npos)
      if (dims() == other.dims())
        if (compare(other, false, precision))
          std::cerr << "0 pixels differ\n";

    return false;
  }

  auto numberOfPixelsWithDifferences = diff->GetNumberOfPixelsWithDifferences();
  Dims dim = dims();
  auto allowedPixelDifference = tolerance * dim[0] * dim[1] * dim[2];
  if (numberOfPixelsWithDifferences > allowedPixelDifference)
  {
    std::cerr << numberOfPixelsWithDifferences << " pixels differ\n";
    return false;
  }

  return true;
}

Image& Image::pad(int padding, PixelType value)
{
  return this->pad(padding, padding, padding, value);
}

Image& Image::pad(int padx, int pady, int padz, PixelType value)
{
  ImageType::SizeType lowerExtendRegion;
  lowerExtendRegion[0] = padx;
  lowerExtendRegion[1] = pady;
  lowerExtendRegion[2] = padz;

  ImageType::SizeType upperExtendRegion;
  upperExtendRegion[0] = padx;
  upperExtendRegion[1] = pady;
  upperExtendRegion[2] = padz;

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

Image& Image::translate(const Vector3 &v)
{
  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(-v);            // negate v because ITK applies transformations backwards.

  return applyTransform(xform);
}

Image& Image::scale(const Vector3 &s)
{
  if (s[0] == 0 || s[1] == 0 || s[2] == 0)
    throw std::invalid_argument("Invalid scale point");

  auto origOrigin(origin());       // scale centered at origin, so temporarily set origin to be the center
  recenter();

  AffineTransformPtr xform(AffineTransform::New());
  xform->Scale(invert(Vector(s))); // invert scale ratio because ITK applies transformations backwards.  
  applyTransform(xform);
  setOrigin(origOrigin);           // restore origin
  
  return *this;
}

Image& Image::rotate(const double angle, const Vector3 &axis)
{
  if (!axis_is_valid(axis)) { throw std::invalid_argument("Invalid axis"); }

  auto origOrigin(origin());       // rotation is around origin, so temporarily set origin to be the center
  recenter();

  AffineTransformPtr xform(AffineTransform::New());
  xform->Rotate3D(axis, -angle);   // negate angle because ITK applies transformations backwards.  
  applyTransform(xform);
  setOrigin(origOrigin);           // restore origin
  
  return *this;
}

Image& Image::applyTransform(const TransformPtr transform, Image::InterpolationType interp)
{
  return applyTransform(transform, origin(), dims(), spacing(), coordsys(), interp);
}

Image& Image::applyTransform(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing,
                             const ImageType::DirectionType coordsys, Image::InterpolationType interp)
{
  return resample(transform, origin, dims, spacing, coordsys, interp);
}

Image& Image::extractLabel(const PixelType label)
{
  binarize(label - std::numeric_limits<PixelType>::epsilon(), label + std::numeric_limits<PixelType>::epsilon());

  return *this;
}

Image& Image::closeHoles(const PixelType foreground)
{
  using FilterType = itk::BinaryFillholeImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetForegroundValue(foreground + std::numeric_limits<PixelType>::epsilon());
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

Image& Image::binarize(PixelType minval, PixelType maxval, PixelType inner_value, PixelType outer_value)
{
  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->image);
  filter->SetLowerThreshold(minval + std::numeric_limits<PixelType>::epsilon());
  filter->SetUpperThreshold(maxval);
  filter->SetInsideValue(inner_value);
  filter->SetOutsideValue(outer_value);
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

Image& Image::applyTPLevelSetFilter(const Image& featureImage, double scaling)
{
  if (!featureImage.image) { throw std::invalid_argument("Invalid feature image"); }

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

    if (val >= isoValue)
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

  filter->SetExtractionRegion(Region(region).clip(*this));
  filter->SetInput(this->image);
  filter->SetDirectionCollapseToIdentity();
  filter->Update();
  this->image = filter->GetOutput();

  return *this;
}

vtkSmartPointer<vtkPolyData> Image::getPolyData(const Image& image, PixelType isoValue)
{
  using FilterType = itk::VTKImageExport<ImageType>;
  FilterType::Pointer itkTargetExporter = FilterType::New();
  itkTargetExporter->SetInput(image.image);

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
  if (!axis_is_valid(n)) { throw std::invalid_argument("Invalid clipping plane (zero length normal)"); }

  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->image, image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd())
  {
    Vector pq(logicalToPhysical(iter.GetIndex()) - q);

    // if n dot pq is < 0, point q is on the back side of the plane.
    if (n * pq < 0.0)
      iter.Set(val);
      
    ++iter;
  }

  return *this;
}

Image& Image::reflect(const Axis &axis)
{
  if (!axis_is_valid(axis))
    throw std::invalid_argument("Invalid axis");

  Vector scale(makeVector({1,1,1}));
  scale[axis] = -1;

  
  using ScalableTransform = itk::ScalableAffineTransform<double, 3>;
  ScalableTransform::Pointer xform(ScalableTransform::New());
  xform->SetScale(scale);
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

  if (numPixels > 0)
    com /= static_cast<double>(numPixels);
  else
    com = center();  // an image with no mass still has a center

  return com;
}

Point3 Image::logicalToPhysical(const Coord &v) const
{
  // return image->TransformIndexToPhysicalPoint(v); // not sure why this call won't work directly
  Point3 value;
  image->TransformIndexToPhysicalPoint(v, value);
  return value;
}

Coord Image::physicalToLogical(const Point3 &p) const
{
  return image->TransformPhysicalPointToIndex(p);
}

std::ostream& operator<<(std::ostream &os, const Image::Region &r)
{
  return os << "{\n\tmin: [" << r.min[0] << ", " << r.min[1] << ", " << r.min[2] << "]"
            << ",\n\tmax: [" << r.max[0] << ", " << r.max[1] << ", " << r.max[2] << "]\n}";
}

std::ostream& operator<<(std::ostream &os, const Image& img)
{
  return os << "{\n\tdims: " << img.dims() << ",\n\torigin: "
            << img.origin() << ",\n\tsize: " << img.size() << "\n}";
}

} // shapeworks
