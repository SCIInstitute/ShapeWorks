#include <ShapeWorksGroom.h>

#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>

#include "vnl/vnl_vector.h"
#include "bounding_box.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkReinitializeLevelSetImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkNrrdImageIOFactory.h"
#include "itkMetaImageIOFactory.h"
#include "itkCastImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"
#include "itkApproximateSignedDistanceMapImageFilter.h"

ShapeWorksGroom::ShapeWorksGroom(
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground, 
  double blurSigma, size_t padding,  size_t iterations,
  bool verbose)
  : images_(inputs), background_(background), blurSigma_(blurSigma),
  foreground_(foreground),  padding_(padding),
  iterations_(iterations), verbose_(verbose)
  {

  this->paddingInit_ = false;
  this->upper_ = { 0,0,0 };
  this->lower_ = { 0,0,0 };
  this->transforms_.resize(this->images_.size());
}

void ShapeWorksGroom::queueTool(std::string tool) {
  this->runTools_.insert(std::make_pair(tool, true));
}

void ShapeWorksGroom::run() {

  size_t ran = 0;
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

std::map<std::string, bool> ShapeWorksGroom::tools() {
  return this->runTools_;
}

double ShapeWorksGroom::foreground() { return this->foreground_; }

std::vector<ImageType::Pointer> ShapeWorksGroom::getImages() {
  return this->images_;
}

void ShapeWorksGroom::isolate(int which) {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: isolate on " <<
      (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    typedef itk::Image<unsigned char, 3>  IsolateType;
    typedef itk::CastImageFilter< ImageType, IsolateType > ToIntType;
    ToIntType::Pointer filter = ToIntType::New();
    filter->SetInput(img);
    filter->Update();
    // Find the connected components in this image.
    itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::Pointer
      ccfilter = itk::ConnectedComponentImageFilter<IsolateType, IsolateType>::New();

    ccfilter->SetInput(filter->GetOutput());
    ccfilter->FullyConnectedOn();
    ccfilter->Update();
    typedef itk::RelabelComponentImageFilter<
      IsolateType, IsolateType >  RelabelType;
    RelabelType::Pointer relabel = RelabelType::New();
    relabel->SetInput(ccfilter->GetOutput());
    relabel->SortByObjectSizeOn();
    relabel->Update();
    typedef itk::ThresholdImageFilter< IsolateType >  ThreshType;
    ThreshType::Pointer thresh = ThreshType::New();
    thresh->SetInput(relabel->GetOutput());
    thresh->SetOutsideValue(0);
    thresh->ThresholdBelow(0);
    thresh->ThresholdAbove(1.001);
    thresh->Update();
    typedef itk::CastImageFilter< IsolateType, ImageType > FilterType;
    FilterType::Pointer filter2 = FilterType::New();
    filter2->SetInput(thresh->GetOutput());
    filter2->Update();
    this->images_[i] = filter2->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: isolate" << std::endl;
  }
}

void ShapeWorksGroom::hole_fill(int which) {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: antialias on " <<
      (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    typedef itk::BinaryFillholeImageFilter< ImageType > HoleType;
    HoleType::Pointer hfilter = HoleType::New();
    hfilter->SetInput(img);
    hfilter->SetForegroundValue(itk::NumericTraits< PixelType >::min());
    hfilter->Update();
    this->images_[i] = hfilter->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: hole_fill" << std::endl;
  }
}

void ShapeWorksGroom::center(int which) {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: center on " <<
      (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);

  for (size_t i = start; i < end; i++) {
    auto img = this->images_[i];
    ImageType::PointType origin = img->GetOrigin();

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
        for (unsigned int i = 0; i < 3; i++) {
          params[i] += point[i];
        }
        count += 1.0;
      } else { sit.Set(this->background_); }
    }

    // Compute center of mass.
    for (unsigned int i = 0; i < 3; i++) {
      params[i] = params[i] / count;
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
    for (oit.GoToBegin(); !oit.IsAtEnd(); ++oit){
      oit.Set(this->background_);
    }

    // Translate the segmentation back into the original image.
    itk::TranslationTransform<double, 3>::Pointer trans
      = itk::TranslationTransform<double, 3>::New();
    trans->SetParameters(params);

    this->transforms_[i] = params;

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

void ShapeWorksGroom::auto_pad(int which) {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: auto_pad on " <<
      (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  bool first = true;
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  if (!this->paddingInit_) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      auto img = this->images_[i];
      if (first == true) {
        first = false;
        this->lower_ = img->GetLargestPossibleRegion().GetIndex();
        this->upper_ = lower_ + img->GetLargestPossibleRegion().GetSize();
      } else {
        // Keep the largest bounding box.
        ImageType::RegionType::IndexType lowerTmp
          = img->GetLargestPossibleRegion().GetIndex();
        ImageType::RegionType::IndexType upperTmp
          = lowerTmp + img->GetLargestPossibleRegion().GetSize();
        for (unsigned int i = 0; i < 3; i++) {
          if (lowerTmp[i] < this->lower_[i]) { 
            this->lower_[i] = lowerTmp[i];
          }
          if (upperTmp[i] > this->upper_[i]){
            this->upper_[i] = upperTmp[i];
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
	itk::Size<3> hipad;
	hipad[0] = pd;
	hipad[1] = pd;
	hipad[2] = pd;
	
    unsigned long lowpad[3] = { pd, pd, pd };

    padder->SetPadUpperBound(hipad);
    padder->SetPadLowerBound(hipad);
    padder->UpdateLargestPossibleRegion();

    if (this->verbose_) {
      std::cout << "input region = "
        << img->GetBufferedRegion().GetSize()
        << std::endl;
      std::cout << "lowpad: " << lowpad[0] << " " << lowpad[1] << " " << lowpad[2]
        << std::endl;
      std::cout << "hipad: " << hipad[0] << " " << hipad[1] << " " << hipad[2]
        << std::endl;
    }
    this->images_[i] = padder->GetOutput();
  }
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: auto_pad" << std::endl;
  }
}

void ShapeWorksGroom::antialias(int which) {
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

void ShapeWorksGroom::fastmarching(int which) {
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

void ShapeWorksGroom::blur(int which) {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: blur on "<< 
      (which == -1 ? "all" : std::to_string(which)) << std::endl;
  }
  auto start = (which == -1 ? 0 : which);
  auto end = (which == -1 ? this->images_.size() : which + 1);
  for (size_t i = start ; i < end; i++) {
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

std::vector<transform_type> ShapeWorksGroom::get_transforms()
{
  return this->transforms_;
}
