#include <ShapeWorksGroom.h>

#include <sstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>

#include "bounding_box.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"

ShapeWorksGroom::ShapeWorksGroom(
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground,
  double blurSigma, size_t padding, size_t iterations,
  bool verbose)
  : images_(std::move(inputs)), background_(background), blurSigma_(blurSigma),
    foreground_(foreground), padding_(padding),
    iterations_(iterations), verbose_(verbose)
{
  this->transforms_.resize(this->images_.size());
}

void ShapeWorksGroom::queueTool(const std::string& tool)
{
  this->runTools_.insert(std::make_pair(tool, true));
}

void ShapeWorksGroom::run()
{
  if (this->runTools_.count("center")) {
    this->center();
  }
  if (this->runTools_.count("isolate")) {
    this->isolate();
  }
  if (this->runTools_.count("hole_fill")) {
    this->hole_fill();
  }
  if (this->runTools_.count("auto_pad")) {
    this->auto_pad();
  }
  if (this->runTools_.count("antialias")) {
    this->antialias();
  }
  if (this->runTools_.count("fastmarching")) {
    this->fastmarching();
  }
  if (this->runTools_.count("blur")) {
    this->blur();
  }
}

std::map<std::string, bool> ShapeWorksGroom::tools()
{
  return this->runTools_;
}

double ShapeWorksGroom::foreground() const
{ return this->foreground_; }

std::vector<ImageType::Pointer> ShapeWorksGroom::getImages()
{
  return this->images_;
}

void ShapeWorksGroom::isolate(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: isolate on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    typedef itk::Image<unsigned char, 3> IsolateType;
    typedef itk::CastImageFilter<ImageType, IsolateType> ToIntType;
    ToIntType::Pointer filter = ToIntType::New();
    filter->SetInput(img);
    filter->Update();
    // Find the connected components in this image.
    itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::Pointer
      ccfilter = itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::New();

    ccfilter->SetInput(filter->GetOutput());
    ccfilter->FullyConnectedOn();
    ccfilter->Update();
    typedef itk::RelabelComponentImageFilter<IsolateType, IsolateType> RelabelType;
    RelabelType::Pointer relabel = RelabelType::New();
    relabel->SetInput(ccfilter->GetOutput());
    relabel->SortByObjectSizeOn();
    relabel->Update();
    typedef itk::ThresholdImageFilter<IsolateType> ThreshType;
    ThreshType::Pointer thresh = ThreshType::New();
    thresh->SetInput(relabel->GetOutput());
    thresh->SetOutsideValue(0);
    thresh->ThresholdBelow(0);
    thresh->ThresholdAbove(1);
    thresh->Update();
    typedef itk::CastImageFilter<IsolateType, ImageType> FilterType;
    FilterType::Pointer filter2 = FilterType::New();
    filter2->SetInput(thresh->GetOutput());
    filter2->Update();
    this->images_[i] = filter2->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: isolate" << std::endl;
  }
}

void ShapeWorksGroom::hole_fill(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: antialias on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    typedef itk::BinaryFillholeImageFilter<ImageType> HoleType;
    HoleType::Pointer hfilter = HoleType::New();
    hfilter->SetInput(img);
    hfilter->SetForegroundValue(itk::NumericTraits<PixelType>::min());
    hfilter->Update();
    this->images_[i] = hfilter->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: hole_fill" << std::endl;
  }
}

void ShapeWorksGroom::center(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: center on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);

  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];

    // Copy the original image and find the center of mass.
    ImageType::Pointer simg = ImageType::New();
    simg->CopyInformation(img);
    simg->SetRegions(img->GetLargestPossibleRegion());
    simg->Allocate();

    itk::ImageRegionIteratorWithIndex<ImageType> oit(img, img->GetLargestPossibleRegion());
    itk::ImageRegionIteratorWithIndex<ImageType> sit(simg, img->GetLargestPossibleRegion());
    sit.GoToBegin();
    oit.GoToBegin();

    itk::Array<double> params(3);
    params.Fill(0.0);
    double count = 0.0;
    itk::Point<double, 3> point;
    for (; !oit.IsAtEnd(); ++oit, ++sit) {
      if (oit.Get() != this->background_) {
        sit.Set(oit.Get());
        // Get the physical index from the image index.
        img->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
        for (unsigned int j = 0; j < 3; j++) {
          params[j] += point[j];
        }
        count += 1.0;
      }
      else { sit.Set(this->background_); }
    }

    // Compute center of mass.
    for (unsigned int j = 0; j < 3; j++) {
      params[j] = params[j] / count;
    }

    double new_origin[3];
    new_origin[0] = -(img->GetLargestPossibleRegion().GetSize()[0] / 2.0) * img->GetSpacing()[0];
    new_origin[1] = -(img->GetLargestPossibleRegion().GetSize()[1] / 2.0) * img->GetSpacing()[1];
    new_origin[2] = -(img->GetLargestPossibleRegion().GetSize()[2] / 2.0) * img->GetSpacing()[2];

    img->SetOrigin(new_origin);
    if (this->verbose_) {
      std::cerr << "new origin = " << img->GetOrigin() << "\n";
    }
    // Zero out the original image.
    for (oit.GoToBegin(); !oit.IsAtEnd(); ++oit) {
      oit.Set(this->background_);
    }

    itk::MatrixOffsetTransformBase<double, 3, 3>::OutputVectorType tform;
    //TransformType::OutputVectorType translation;

    // Translate the segmentation back into the original image.
    itk::AffineTransform<double, 3>::Pointer trans = itk::AffineTransform<double, 3>::New();
    trans->SetIdentity();
    tform[0] = params[0];
    tform[1] = params[1];
    tform[2] = params[2];
    trans->SetTranslation(tform);

    this->transforms_[i] = trans->GetParameters();

    itk::NearestNeighborInterpolateImageFunction<ImageType, double>::Pointer
      interp = itk::NearestNeighborInterpolateImageFunction<ImageType, double>::New();

    itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampler
      = itk::ResampleImageFilter<ImageType, ImageType>::New();
    resampler->SetOutputParametersFromImage(img);
    resampler->SetTransform(trans);
    resampler->SetInterpolator(interp);
    resampler->SetInput(simg);
    resampler->Update();

    oit.GoToBegin();
    itk::ImageRegionIterator<ImageType> it(resampler->GetOutput(), img->GetLargestPossibleRegion());
    for (; !it.IsAtEnd(); ++it, ++oit) {
      oit.Set(it.Get());
    }
    this->images_[i] = resampler->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: center" << std::endl;
  }
}

void ShapeWorksGroom::auto_pad(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: auto_pad on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  bool first = true;
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  if (!this->paddingInit_) {
    for (const auto& img : this->images_) {
      if (first) {
        first = false;
        this->lower_ = img->GetLargestPossibleRegion().GetIndex();
        this->upper_ = this->lower_ + img->GetLargestPossibleRegion().GetSize();
      }
      else {
        // Keep the largest bounding box.
        ImageType::RegionType::IndexType lowerTmp
          = img->GetLargestPossibleRegion().GetIndex();
        ImageType::RegionType::IndexType upperTmp
          = lowerTmp + img->GetLargestPossibleRegion().GetSize();
        for (unsigned int j = 0; j < 3; j++) {
          if (lowerTmp[j] < this->lower_[j]) {
            this->lower_[j] = lowerTmp[j];
          }
          if (upperTmp[j] > this->upper_[j]) {
            this->upper_[j] = upperTmp[j];
          }
        }
      }
    }
    this->paddingInit_ = true;
  }
  if (this->verbose_) {
    std::cout << "Lower bound = " << this->lower_[0] << " " << this->lower_[1]
              << " " << this->lower_[2] << std::endl;
    std::cout << "Upper bound = " << this->upper_[0] << " " << this->upper_[1] << " "
              << this->upper_[2] << std::endl;
  }
  // Make sure the origin is at the center of the image.
  double orig[3];
  for (unsigned int i = 0; i < 3; i++) {
    orig[i] = -static_cast<double>(this->upper_[i] - this->lower_[i]) / 2.0;
  }
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    itk::ConstantPadImageFilter<ImageType, ImageType>::Pointer padder
      = itk::ConstantPadImageFilter<ImageType, ImageType>::New();
    padder->SetConstant(0);
    padder->SetInput(img);

    // set the desired padding
    auto pd = static_cast<unsigned long>((this->padding_ + 1) / 2);
    itk::Size<3> hi_pad{pd, pd, pd};
    itk::Size<3> low_pad{pd, pd, pd};

    padder->SetPadUpperBound(hi_pad);
    padder->SetPadLowerBound(low_pad);
    padder->UpdateLargestPossibleRegion();

    if (this->verbose_) {
      std::cout << "input region = " << img->GetBufferedRegion().GetSize() << "\n";
      std::cout << "lowpad: " << low_pad[0] << " " << low_pad[1] << " " << low_pad[2] << "\n";
      std::cout << "hipad: " << hi_pad[0] << " " << hi_pad[1] << " " << hi_pad[2] << "\n";
    }
    this->images_[i] = padder->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: auto_pad" << std::endl;
  }
}

void ShapeWorksGroom::antialias(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: antialias on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    itk::AntiAliasBinaryImageFilter<ImageType, ImageType>::Pointer anti
      = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>::New();
    anti->SetInput(img);
    anti->SetNumberOfIterations(this->iterations_);
    anti->SetMaximumRMSError(0.024);
    anti->Update();
    this->images_[i] = anti->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: antialias" << std::endl;
  }
}

void ShapeWorksGroom::fastmarching(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: fastmarching on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];

    using FilterType = itk::ReinitializeLevelSetImageFilter<ImageType>;
    FilterType::Pointer filter = FilterType::New();

    filter->SetInput(img);
    filter->NarrowBandingOff();
    filter->SetLevelSetValue(0);
    filter->Update();
    this->images_[i] = filter->GetOutput();

  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: fastmarching" << std::endl;
  }
}

void ShapeWorksGroom::blur(int which)
{
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: blur on " <<
              (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    itk::DiscreteGaussianImageFilter<ImageType, ImageType>::Pointer blur
      = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    blur->SetInput(img);
    blur->SetVariance(this->blurSigma_ * this->blurSigma_);
    blur->Update();
    this->images_[i] = blur->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: blur" << std::endl;
  }
}

std::vector<TransformType> ShapeWorksGroom::get_transforms()
{
  return this->transforms_;
}
