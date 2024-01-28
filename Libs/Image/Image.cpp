#include "Image.h"

#include <itkAntiAliasBinaryImageFilter.h>
#include <itkBinaryFillholeImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkChangeInformationImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkConstantPadImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>
#include <itkGradientMagnitudeImageFilter.h>
#include <itkImageDuplicator.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itkImageToVTKImageFilter.h>
#include <itkIntensityWindowingImageFilter.h>
#include <itkMultiplyImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkOrientImageFilter.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkReinitializeLevelSetImageFilter.h>
#include <itkRelabelComponentImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkScalableAffineTransform.h>
#include <itkSigmoidImageFilter.h>
#include <itkTestingComparisonImageFilter.h>
#include <itkThresholdImageFilter.h>
#include <itkVTKImageExport.h>
#include <itkVTKImageToImageFilter.h>
#include <vtkContourFilter.h>
#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkImageImport.h>

#include <cmath>
#include <exception>

#include "Exception.h"
#include "MeshUtils.h"
#include "ShapeworksUtils.h"
#include "itkTPGACLevelSetImageFilter.h"  // actually a shapeworks class, not itk

namespace shapeworks {

Image::Image(const Dims dims) : itk_image_(ImageType::New()) {
  ImageType::RegionType region;
  region.SetSize(dims);
  region.SetIndex(Coord({0, 0, 0}));

  itk_image_->SetRegions(region);
  itk_image_->Allocate();
}

Image::Image(const vtkSmartPointer<vtkImageData> vtkImage) {
  // ensure input image data is PixelType (note: it'll either be float or double)
  auto cast = vtkSmartPointer<vtkImageCast>::New();
  cast->SetInputData(vtkImage);
  if (typeid(PixelType) == typeid(float)) {
    cast->SetOutputScalarTypeToFloat();
  }
  cast->Update();

  using FilterType = itk::VTKImageToImageFilter<Image::ImageType>;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput(cast->GetOutput());
  filter->Update();

  this->itk_image_ = cloneData(filter->GetOutput());
}

vtkSmartPointer<vtkImageData> Image::getVTKImage() const {
  using connectorType = itk::ImageToVTKImageFilter<Image::ImageType>;
  connectorType::Pointer connector = connectorType::New();
  connector->SetInput(this->itk_image_);
  connector->Update();

  return connector->GetOutput();
}

Image::ImageType::Pointer Image::cloneData(const Image::ImageType::Pointer image) {
  using DuplicatorType = itk::ImageDuplicator<ImageType>;
  DuplicatorType::Pointer duplicator = DuplicatorType::New();
  duplicator->SetInputImage(image);
  duplicator->Update();
  return duplicator->GetOutput();
}

Image& Image::operator=(const Image& img) {
  this->itk_image_ = Image::cloneData(img.itk_image_);
  return *this;
}

Image& Image::operator=(Image&& img) {
  this->itk_image_ = nullptr;  // make sure to free existing image by setting it to nullptr (works b/c it's a smart ptr)
  this->itk_image_.Swap(img.itk_image_);
  return *this;
}

Image::ImageType::Pointer Image::read(const std::string& pathname) {
  if (pathname.empty()) {
    throw std::invalid_argument("Empty pathname");
  }

  if (ShapeworksUtils::is_directory(pathname)) return readDICOMImage(pathname);

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(pathname);

  try {
    reader->Update();
  } catch (itk::ExceptionObject& exp) {
    throw shapeworks_exception(std::string(exp.what()));
  }

  // reorient the image to RAI if it's not already
  ImageType::Pointer img = reader->GetOutput();
  if (itk::SpatialOrientationAdapter().FromDirectionCosines(img->GetDirection()) !=
      itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI) {
    using Orienter = itk::OrientImageFilter<ImageType, ImageType>;
    Orienter::Pointer orienter = Orienter::New();
    orienter->UseImageDirectionOn();
    // set orientation to RAI
    orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAI);
    orienter->SetInput(img);
    orienter->Update();
    img = orienter->GetOutput();
  }

  return img;
}

Image& Image::operator-() {
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(-iter.Value());
    ++iter;
  }

  return *this;
}

Image Image::operator+(const Image& other) const {
  Image ret(*this);
  ret += other;
  return ret;
}

Image& Image::operator+=(const Image& other) {
  if (dims() != other.dims()) {
    throw std::invalid_argument("images must have same logical dims");
  }

  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> otherIter(other.itk_image_,
                                                         other.itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd() && !otherIter.IsAtEnd()) {
    iter.Set(iter.Value() + otherIter.Value());
    ++iter;
    ++otherIter;
  }

  return *this;
}

Image Image::operator-(const Image& other) const {
  Image ret(*this);
  ret -= other;
  return ret;
}

Image& Image::operator-=(const Image& other) {
  if (dims() != other.dims()) {
    throw std::invalid_argument("images must have same logical dims");
  }

  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  itk::ImageRegionIteratorWithIndex<ImageType> otherIter(other.itk_image_,
                                                         other.itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd() && !otherIter.IsAtEnd()) {
    iter.Set(iter.Value() - otherIter.Value());
    ++iter;
    ++otherIter;
  }

  return *this;
}

Image Image::operator+(const PixelType x) const {
  Image ret(*this);
  ret += x;
  return ret;
}

Image& Image::operator+=(const PixelType x) {
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Value() + x);
    ++iter;
  }

  return *this;
}

Image Image::operator-(const PixelType x) const {
  Image ret(*this);
  ret -= x;
  return ret;
}

Image& Image::operator-=(const PixelType x) {
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Value() - x);
    ++iter;
  }

  return *this;
}

Image Image::operator*(const Image& other) const {
  using FilterType = itk::MultiplyImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput1(this->itk_image_);
  filter->SetInput2(other.itk_image_);
  filter->Update();

  return Image(filter->GetOutput());
}

Image Image::operator*(const PixelType x) const {
  Image ret(*this);
  ret *= x;
  return ret;
}

Image& Image::operator*=(const PixelType x) {
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Value() * x);
    ++iter;
  }

  return *this;
}

Image Image::operator/(const PixelType x) const {
  Image ret(*this);
  ret /= x;
  return ret;
}

Image& Image::operator/=(const PixelType x) {
  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Value() / x);
    ++iter;
  }

  return *this;
}

template <>
Image operator*(const Image& img, const double x) {
  return img.operator*(x);
}

template <>
Image operator/(const Image& img, const double x) {
  return img.operator/(x);
}

template <>
Image& operator*=(Image& img, const double x) {
  return img.operator*=(x);
}

template <>
Image& operator/=(Image& img, const double x) {
  return img.operator/=(x);
}

Image::ImageType::Pointer Image::readDICOMImage(const std::string& pathname) {
  if (pathname.empty()) {
    throw std::invalid_argument("Empty pathname");
  }

  using ReaderType = itk::ImageSeriesReader<ImageType>;
  using ImageIOType = itk::GDCMImageIO;
  using InputNamesGeneratorType = itk::GDCMSeriesFileNames;

  ImageIOType::Pointer gdcm_io = ImageIOType::New();
  InputNamesGeneratorType::Pointer input_names = InputNamesGeneratorType::New();
  input_names->SetInputDirectory(pathname);

  const ReaderType::FileNamesContainer& filenames = input_names->GetInputFileNames();
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetImageIO(gdcm_io);
  reader->SetFileNames(filenames);

  try {
    reader->Update();
  } catch (itk::ExceptionObject& exp) {
    throw std::invalid_argument("Failed to read DICOM from " + pathname + "(" + std::string(exp.what()) + ")");
  }

  return reader->GetOutput();
}

Image& Image::write(const std::string& filename, bool compressed) {
  if (!this->itk_image_) {
    throw std::invalid_argument("Image invalid");
  }
  if (filename.empty()) {
    throw std::invalid_argument("Empty pathname");
  }

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(this->itk_image_);
  writer->SetFileName(filename);
  writer->SetUseCompression(compressed);
  writer->Update();

  return *this;
}

Image& Image::antialias(unsigned iterations, double maxRMSErr, int layers) {
  if (layers < 0) {
    throw std::invalid_argument("layers must be >= 0");
  }

  using FilterType = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetMaximumRMSError(maxRMSErr);
  filter->SetNumberOfIterations(iterations);
  if (layers) {
    filter->SetNumberOfLayers(layers);
  }
  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::recenter() { return setOrigin(negate(size() / 2.0)); }

Image& Image::resample(const TransformPtr transform, const Point3 origin, Dims dims, const Vector3 spacing,
                       const ImageType::DirectionType direction, Image::InterpolationType interp) {
  using FilterType = itk::ResampleImageFilter<ImageType, ImageType>;
  FilterType::Pointer resampler = FilterType::New();

  switch (interp) {
    case Linear:
      // linear interpolation is the default
      break;
    case NearestNeighbor: {
      using InterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
      InterpolatorType::Pointer interpolator = InterpolatorType::New();
      resampler->SetInterpolator(interpolator);
      break;
    }
    default:
      throw std::invalid_argument("Unknown Image::InterpolationType");
  }

  resampler->SetInput(this->itk_image_);
  resampler->SetTransform(transform ? transform : IdentityTransform::New());
  resampler->SetOutputOrigin(origin);
  resampler->SetOutputSpacing(spacing);
  resampler->SetSize(dims);
  resampler->SetOutputDirection(direction);

  resampler->Update();
  this->itk_image_ = resampler->GetOutput();

  return *this;
}

Image& Image::resample(const Vector3& spacing, Image::InterpolationType interp) {
  // compute logical dimensions that keep all image data for this spacing
  Dims inputDims(this->dims());
  Vector3 inputSpacing(this->spacing());
  Dims dims({static_cast<unsigned>(std::floor(inputDims[0] * inputSpacing[0] / spacing[0])),
             static_cast<unsigned>(std::floor(inputDims[1] * inputSpacing[1] / spacing[1])),
             static_cast<unsigned>(std::floor(inputDims[2] * inputSpacing[2] / spacing[2]))});

  Point3 new_origin = origin() + toPoint(0.5 * (spacing - inputSpacing));  // O' += 0.5 * (p' - p)

  return resample(IdentityTransform::New(), new_origin, dims, spacing, coordsys(), interp);
}

Image& Image::resample(double isoSpacing, Image::InterpolationType interp) {
  return resample(makeVector({isoSpacing, isoSpacing, isoSpacing}), interp);
}

Image& Image::resize(Dims dims, Image::InterpolationType interp) {
  // use existing dims for any that are unspecified
  Dims inputDims(this->dims());
  if (dims[0] == 0) dims[0] = inputDims[0];
  if (dims[1] == 0) dims[1] = inputDims[1];
  if (dims[2] == 0) dims[2] = inputDims[2];

  // compute new spacing so physical image size remains the same
  Vector3 inputSpacing(spacing());
  Vector3 spacing(makeVector({inputSpacing[0] * inputDims[0] / dims[0], inputSpacing[1] * inputDims[1] / dims[1],
                              inputSpacing[2] * inputDims[2] / dims[2]}));

  return resample(IdentityTransform::New(), origin(), dims, spacing, coordsys(), interp);
}

bool Image::compare(const Image& other, bool verifyall, double tolerance, double precision) const {
  if (tolerance > 1 || tolerance < 0) {
    throw std::invalid_argument("tolerance value must be between 0 and 1 (inclusive)");
  }

  // we use the region of interest filter here with the full region because our
  // incoming image may be the output of an ExtractImageFilter or PadImageFilter
  // which modify indices and leave the origin intact.  These will not compare
  // properly against a saved NRRD file because the act of saving the image to
  // NRRD and back in will cause the origin (and indices) to be reset.
  using RegionFilterType = itk::RegionOfInterestImageFilter<ImageType, ImageType>;
  RegionFilterType::Pointer region_filter = RegionFilterType::New();
  region_filter->SetInput(this->itk_image_);
  region_filter->SetRegionOfInterest(this->itk_image_->GetLargestPossibleRegion());
  region_filter->UpdateLargestPossibleRegion();
  ImageType::Pointer itk_image = region_filter->GetOutput();

  // perform the same to the other image
  RegionFilterType::Pointer region_filter2 = RegionFilterType::New();
  region_filter2->SetInput(other.itk_image_);
  region_filter2->SetRegionOfInterest(other.itk_image_->GetLargestPossibleRegion());
  region_filter2->UpdateLargestPossibleRegion();
  ImageType::Pointer other_itk_image = region_filter2->GetOutput();

  using DiffType = itk::Testing::ComparisonImageFilter<ImageType, ImageType>;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(other_itk_image);
  diff->SetTestInput(itk_image);
  diff->SetDifferenceThreshold(precision);
  diff->SetToleranceRadius(0);
  diff->SetVerifyInputInformation(verifyall);

  try {
    diff->UpdateLargestPossibleRegion();
  } catch (itk::ExceptionObject& exp) {
    return false;
  }

  auto numberOfPixelsWithDifferences = diff->GetNumberOfPixelsWithDifferences();
  Dims dim = dims();
  auto allowedPixelDifference = tolerance * dim[0] * dim[1] * dim[2];
  if (numberOfPixelsWithDifferences > allowedPixelDifference) {
    return false;
  }

  return true;
}

Image& Image::pad(int padding, PixelType value) { return this->pad(padding, padding, padding, value); }

Image& Image::pad(int padx, int pady, int padz, PixelType value) {
  ImageType::SizeType lowerExtendRegion;
  lowerExtendRegion[0] = padx;
  lowerExtendRegion[1] = pady;
  lowerExtendRegion[2] = padz;

  ImageType::SizeType upperExtendRegion;
  upperExtendRegion[0] = padx;
  upperExtendRegion[1] = pady;
  upperExtendRegion[2] = padz;

  return this->pad(lowerExtendRegion, upperExtendRegion, value);
}

Image& Image::pad(IndexRegion& region, PixelType value) {
  auto bbox = logicalBoundingBox();

  // compute positive numbers to pad in each direction
  ImageType::SizeType lowerExtendRegion;
  lowerExtendRegion[0] = std::max(Coord::IndexValueType(0), -region.min[0]);
  lowerExtendRegion[1] = std::max(Coord::IndexValueType(0), -region.min[1]);
  lowerExtendRegion[2] = std::max(Coord::IndexValueType(0), -region.min[2]);

  ImageType::SizeType upperExtendRegion;
  upperExtendRegion[0] = std::max(Coord::IndexValueType(0), region.max[0] - bbox.max[0]);
  upperExtendRegion[1] = std::max(Coord::IndexValueType(0), region.max[1] - bbox.max[1]);
  upperExtendRegion[2] = std::max(Coord::IndexValueType(0), region.max[2] - bbox.max[2]);

  return this->pad(lowerExtendRegion, upperExtendRegion, value);
}

Image& Image::pad(Dims lowerExtendRegion, Dims upperExtendRegion, PixelType value) {
  using FilterType = itk::ConstantPadImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetPadLowerBound(lowerExtendRegion);
  filter->SetPadUpperBound(upperExtendRegion);
  filter->SetConstant(value);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::translate(const Vector3& v) {
  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(-v);  // negate v because ITK applies transformations backwards.

  return applyTransform(xform);
}

Image& Image::scale(const Vector3& s) {
  if (s[0] == 0 || s[1] == 0 || s[2] == 0) {
    throw std::invalid_argument("Invalid scale point");
  }

  auto origOrigin(origin());  // scale centered at origin, so temporarily set origin to be the center
  recenter();

  AffineTransformPtr xform(AffineTransform::New());
  xform->Scale(invertValue(Vector(s)));  // invert scale ratio because ITK applies transformations backwards.
  applyTransform(xform);
  setOrigin(origOrigin);  // restore origin

  return *this;
}

Image& Image::rotate(const double angle, Axis axis) {
  switch (axis) {
    case X:
      return rotate(angle, makeVector({1.0, 0.0, 0.0}));
    case Y:
      return rotate(angle, makeVector({0.0, 1.0, 0.0}));
    case Z:
      return rotate(angle, makeVector({0.0, 0.0, 1.0}));
    default:
      throw std::invalid_argument("Unknown axis.");
  }
}

Image& Image::rotate(const double angle, const Vector3& axis) {
  if (!axis_is_valid(axis)) {
    throw std::invalid_argument("Invalid axis");
  }

  auto origOrigin(origin());  // rotation is around origin, so temporarily set origin to be the center
  recenter();

  AffineTransformPtr xform(AffineTransform::New());
  xform->Rotate3D(axis, -angle);  // negate angle because ITK applies transformations backwards.
  applyTransform(xform);
  setOrigin(origOrigin);  // restore origin

  return *this;
}

Image& Image::applyTransform(const TransformPtr transform, Image::InterpolationType interp) {
  return applyTransform(transform, origin(), dims(), spacing(), coordsys(), interp);
}

Image& Image::applyTransform(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing,
                             const ImageType::DirectionType coordsys, Image::InterpolationType interp) {
  return resample(transform, origin, dims, spacing, coordsys, interp);
}

Image& Image::extractLabel(const PixelType label) {
  binarize(label - std::numeric_limits<PixelType>::epsilon(), label + std::numeric_limits<PixelType>::epsilon());

  return *this;
}

Image& Image::closeHoles(const PixelType foreground) {
  using FilterType = itk::BinaryFillholeImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetForegroundValue(foreground + std::numeric_limits<PixelType>::epsilon());
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::binarize(PixelType minVal, PixelType maxVal, PixelType innerVal, PixelType outerVal) {
  using FilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetLowerThreshold(minVal + std::numeric_limits<PixelType>::epsilon());
  filter->SetUpperThreshold(maxVal);
  filter->SetInsideValue(innerVal);
  filter->SetOutsideValue(outerVal);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::computeDT(PixelType isoValue) {
  using FilterType = itk::ReinitializeLevelSetImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->NarrowBandingOff();
  filter->SetLevelSetValue(isoValue);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::applyCurvatureFilter(unsigned iterations) {
  if (iterations < 0) {
    throw std::invalid_argument("iterations must be >= 0");
  }

  using FilterType = itk::CurvatureFlowImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetTimeStep(0.0625);
  filter->SetNumberOfIterations(iterations);
  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::applyGradientFilter() {
  using FilterType = itk::GradientMagnitudeImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::applySigmoidFilter(double alpha, double beta) {
  using FilterType = itk::SigmoidImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetAlpha(alpha);
  filter->SetBeta(beta);
  filter->SetOutputMinimum(0.0);
  filter->SetOutputMaximum(1.0);
  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::applyTPLevelSetFilter(const Image& featureImage, double scaling) {
  if (!featureImage.itk_image_) {
    throw std::invalid_argument("Invalid feature image");
  }

  using FilterType =
      itk::TPGACLevelSetImageFilter<ImageType, ImageType>;  // TODO: this is no longer part of ITK and should be updated
  FilterType::Pointer filter = FilterType::New();

  filter->SetPropagationScaling(scaling);
  filter->SetCurvatureScaling(1.0);
  filter->SetAdvectionScaling(1.0);
  filter->SetMaximumRMSError(0.0);
  filter->SetNumberOfIterations(20);
  filter->SetInput(this->itk_image_);
  filter->SetFeatureImage(featureImage.itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::topologyPreservingSmooth(float scaling, float sigmoidAlpha, float sigmoidBeta) {
  Image featureImage(*this);
  featureImage.applyGradientFilter().applySigmoidFilter(sigmoidAlpha, sigmoidBeta);

  return applyTPLevelSetFilter(featureImage, scaling);
}

Image& Image::applyIntensityFilter(double minVal, double maxVal) {
  using FilterType = itk::IntensityWindowingImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetWindowMinimum(minVal);
  filter->SetWindowMaximum(maxVal);
  filter->SetOutputMinimum(0.0);
  filter->SetOutputMaximum(255.0);
  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::gaussianBlur(double sigma) {
  using BlurType = itk::DiscreteGaussianImageFilter<ImageType, ImageType>;
  BlurType::Pointer blur = BlurType::New();

  blur->SetInput(this->itk_image_);
  blur->SetVariance(sigma * sigma);
  blur->Update();
  this->itk_image_ = blur->GetOutput();

  return *this;
}

Image& Image::crop(PhysicalRegion region, const int padding) {
  region.shrink(physicalBoundingBox());  // clip region to fit inside image
  if (!region.valid()) {
    throw std::invalid_argument("Invalid region specified (it may lie outside physical bounds of image).");
  }

  using FilterType = itk::RegionOfInterestImageFilter<ImageType, ImageType>;
  FilterType::Pointer filter = FilterType::New();

  IndexRegion indexRegion(physicalToLogical(region));
  indexRegion.pad(padding);
  filter->SetRegionOfInterest(ImageType::RegionType(indexRegion.min, indexRegion.size()));
  filter->SetInput(this->itk_image_);
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::clip(const Plane plane, const PixelType val) {
  if (!axis_is_valid(getNormal(plane))) {
    throw std::invalid_argument("Invalid clipping plane (zero length normal)");
  }

  itk::ImageRegionIteratorWithIndex<ImageType> iter(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    Vector pq(logicalToPhysical(iter.GetIndex()) - getOrigin(plane));

    // if n dot pq is < 0, point q is on the back side of the plane.
    if (getNormal(plane) * pq < 0.0) iter.Set(val);

    ++iter;
  }

  return *this;
}

Image& Image::reflect(const Axis& axis) {
  if (!axis_is_valid(axis)) {
    throw std::invalid_argument("Invalid axis");
  }

  Vector scale(makeVector({1, 1, 1}));
  scale[axis] = -1;

  using ScalableTransform = itk::ScalableAffineTransform<double, 3>;
  ScalableTransform::Pointer xform(ScalableTransform::New());
  xform->SetScale(scale);
  Point3 currentOrigin(origin());
  recenter().applyTransform(xform).setOrigin(currentOrigin);

  return *this;
}

Image& Image::setOrigin(Point3 origin) {
  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetOutputOrigin(origin);
  filter->ChangeOriginOn();
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::setSpacing(Vector3 spacing) {
  if (spacing[0] <= 0 || spacing[1] <= 0 || spacing[2] <= 0) {
    throw std::invalid_argument("Spacing cannot b <= 0");
  }

  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetOutputSpacing(spacing);
  filter->ChangeSpacingOn();
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::setCoordsys(ImageType::DirectionType coordsys) {
  using FilterType = itk::ChangeInformationImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->SetOutputDirection(coordsys);
  filter->ChangeDirectionOn();
  filter->Update();
  this->itk_image_ = filter->GetOutput();

  return *this;
}

Image& Image::isolate() {
  typedef itk::Image<unsigned char, 3> IsolateType;
  typedef itk::CastImageFilter<ImageType, IsolateType> ToIntType;
  ToIntType::Pointer filter = ToIntType::New();
  filter->SetInput(this->itk_image_);
  filter->Update();

  // Find the connected components in this image.
  auto cc = itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::New();
  cc->SetInput(filter->GetOutput());
  cc->FullyConnectedOn();
  cc->Update();

  auto relabel = itk::RelabelComponentImageFilter<IsolateType, IsolateType>::New();
  relabel->SetInput(cc->GetOutput());
  relabel->SortByObjectSizeOn();
  relabel->Update();

  auto thresh = itk::ThresholdImageFilter<IsolateType>::New();
  thresh->SetInput(relabel->GetOutput());
  thresh->SetOutsideValue(0);
  thresh->ThresholdBelow(0);
  thresh->ThresholdAbove(1);
  thresh->Update();

  auto cast = itk::CastImageFilter<IsolateType, ImageType>::New();
  cast->SetInput(thresh->GetOutput());
  cast->Update();

  this->itk_image_ = cast->GetOutput();

  return *this;
}

Point3 Image::centerOfMass(PixelType minVal, PixelType maxVal) const {
  itk::ImageRegionIteratorWithIndex<ImageType> imageIt(this->itk_image_, itk_image_->GetLargestPossibleRegion());
  int numPixels = 0;
  Point3 com({0.0, 0.0, 0.0});

  while (!imageIt.IsAtEnd()) {
    PixelType val = imageIt.Get();
    if (val > minVal && val <= maxVal) {
      numPixels++;
      com += itk_image_->TransformIndexToPhysicalPoint<double>(imageIt.GetIndex());
    }
    ++imageIt;
  }

  if (numPixels > 0)
    com /= static_cast<double>(numPixels);
  else
    com = center();  // an image with no mass still has a center

  return com;
}

Image::StatsPtr Image::statsFilter() {
  using FilterType = itk::StatisticsImageFilter<ImageType>;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->itk_image_);
  filter->UpdateLargestPossibleRegion();
  return filter;
}

Image::PixelType Image::min() {
  StatsPtr filter = statsFilter();
  return filter->GetMinimum();
}

Image::PixelType Image::max() {
  StatsPtr filter = statsFilter();
  return filter->GetMaximum();
}

Image::PixelType Image::mean() {
  StatsPtr filter = statsFilter();
  return filter->GetMean();
}

Image::PixelType Image::std() {
  StatsPtr filter = statsFilter();
  return sqrt(filter->GetVariance());
}

IndexRegion Image::logicalBoundingBox() const {
  IndexRegion region(Coord({0, 0, 0}), toCoord(dims() - Dims({1, 1, 1})));
  return region;
}

PhysicalRegion Image::physicalBoundingBox() const {
  PhysicalRegion region(origin(), origin() + dotProduct(toVector(dims() - Dims({1, 1, 1})), spacing()));
  return region;
}

PhysicalRegion Image::physicalBoundingBox(PixelType isoValue) const {
  PhysicalRegion bbox;

  itk::ImageRegionIteratorWithIndex<ImageType> imageIterator(itk_image_, itk_image_->GetLargestPossibleRegion());
  while (!imageIterator.IsAtEnd()) {
    PixelType val = imageIterator.Get();

    if (val >= isoValue) bbox.expand(logicalToPhysical(imageIterator.GetIndex()));

    ++imageIterator;
  }

  return bbox;
}

PhysicalRegion Image::logicalToPhysical(const IndexRegion region) const {
  return PhysicalRegion(logicalToPhysical(region.min), logicalToPhysical(region.max));
}

IndexRegion Image::physicalToLogical(const PhysicalRegion region) const {
  return IndexRegion(physicalToLogical(region.min), physicalToLogical(region.max));
}

Point3 Image::logicalToPhysical(const Coord& v) const {
  Point3 value;
  itk_image_->TransformIndexToPhysicalPoint(v, value);
  return value;
}

Coord Image::physicalToLogical(const Point3& p) const { return itk_image_->TransformPhysicalPointToIndex(p); }

bool Image::isInside(const Point3 &p) const {
  auto itk_image = getITKImage();
  auto region = itk_image->GetLargestPossibleRegion();
  Image::ImageType::IndexType index;
  Image::ImageType::PointType pitk;
  pitk[0] = p[0];
  pitk[1] = p[1];
  pitk[2] = p[2];
  itk_image->TransformPhysicalPointToIndex( pitk, index );
  return region.IsInside(index);
}

Image::ImageIterator Image::iterator() {
  ImageIterator iter(this->itk_image_, itk_image_->GetRequestedRegion());
  return iter;
}

Mesh Image::toMesh(PixelType isoValue) const {
  auto vtkImage = getVTKImage();

  auto targetContour = vtkSmartPointer<vtkContourFilter>::New();
  targetContour->SetInputData(vtkImage);
  targetContour->SetValue(0, isoValue);
  targetContour->Update();

  return Mesh(targetContour->GetOutput());
}

Image::PixelType Image::evaluate(Point p) {
  if (!interpolator_) {
    interpolator_ = InterpolatorType::New();
    interpolator_->SetInputImage(itk_image_);
  }
  return interpolator_->Evaluate(p);
}

TransformPtr Image::createCenterOfMassTransform() {
  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(-(center() - centerOfMass()));  // ITK translations go in a counterintuitive direction
  return xform;
}

TransformPtr Image::createRigidRegistrationTransform(const Image& target_dt, float isoValue, unsigned iterations) {
  if (!target_dt.itk_image_) {
    throw std::invalid_argument("Invalid target. Expected distance transform image");
  }

  Mesh sourceContour = toMesh(isoValue);
  Mesh targetContour = target_dt.toMesh(isoValue);

  try {
    auto mat = MeshUtils::createICPTransform(sourceContour, targetContour, Mesh::Rigid, iterations);
    return shapeworks::createTransform(ShapeworksUtils::getMatrix(mat), ShapeworksUtils::getOffset(mat));
  } catch (std::invalid_argument) {
    std::cerr << "failed to create ICP transform.\n";
    if (sourceContour.numPoints() == 0) {
      std::cerr << "\tspecified isoValue (" << isoValue << ") results in an empty mesh for source\n";
    }
    if (targetContour.numPoints() == 0) {
      std::cerr << "\tspecified isoValue (" << isoValue << ") results in an empty mesh for target\n";
    }
  }
  return AffineTransform::New();
}

std::ostream& operator<<(std::ostream& os, const Image& img) {
  return os << "{\n\tdims: " << img.dims() << ",\n\torigin: " << img.origin() << ",\n\tsize: " << img.size()
            << ",\n\tspacing: " << img.spacing() << "\n}";
}

}  // namespace shapeworks
