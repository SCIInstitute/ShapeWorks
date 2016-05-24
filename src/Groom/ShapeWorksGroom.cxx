#include <ShapeWorksGroom.h>
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <vector>
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
#include "itkPSMImplicitSurfaceImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkThresholdImageFilter.h"
#include "itkBinaryFillholeImageFilter.h"
#include <map>

ShapeWorksGroom::ShapeWorksGroom(
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground, 
  double blurSigma, double sigmaFastMarch,
  double iso, size_t padding,  size_t iterations,
  bool verbose)
  :  images_(inputs), 
  background_(background), blurSigma_(blurSigma), sigmaFastMarch_(sigmaFastMarch),
  iso_value_(iso),
  foreground_(foreground), verbose_(verbose), padding_(padding),
  iterations_(iterations) {}

void ShapeWorksGroom::queueTool(std::string tool) {
  this->runTools_.insert(std::make_pair(tool, true));
}

void ShapeWorksGroom::run() {
  this->seed_.Fill(0);
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
  if (this->runTools_.count("auto_crop")) {
    this->auto_crop();
  }
  if (this->runTools_.count("auto_pad")) {
    this->auto_pad();
  }
  if (this->runTools_.count("antialias")) {
    this->antialias();
  }
  if (this->runTools_.count("blur")) {
    this->blur();
  }
  if (this->runTools_.count("fastmarching")) {
    this->fastmarching();
  }
}

std::map<std::string, bool> ShapeWorksGroom::tools() {
  return this->runTools_;
}

double ShapeWorksGroom::foreground() { return this->foreground_; }

std::vector<ImageType::Pointer> ShapeWorksGroom::getImages() {
  return this->images_;
}

void ShapeWorksGroom::isolate() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: isolate" << std::endl;
  }
  std::vector<ImageType::Pointer> newImages;
  for (auto img : this->images_) {

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
    auto imgIsolate = filter2->GetOutput();
    if (this->isEmpty(imgIsolate)) { return; }
    newImages.push_back(imgIsolate);
  }
  this->images_ = newImages;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: isolate" << std::endl;
  }
}

void ShapeWorksGroom::hole_fill() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: hole_fill" << std::endl;
  }
  std::vector<ImageType::Pointer> out;
  for (auto img : this->images_) {

    typedef itk::BinaryFillholeImageFilter< ImageType > HoleType;
    HoleType::Pointer hfilter = HoleType::New();
    hfilter->SetInput(img);
    hfilter->SetForegroundValue(itk::NumericTraits< PixelType >::min());
    hfilter->Update();
    if (this->isEmpty(hfilter->GetOutput())) { return; }
    out.push_back(hfilter->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: hole_fill" << std::endl;
  }
}

void ShapeWorksGroom::center() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: center" << std::endl;
  }
  std::vector<ImageType::Pointer> out;
  for (auto img : this->images_) {
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
    if (this->isEmpty(resampler->GetOutput())) { return; }
    out.push_back(resampler->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: center" << std::endl;
  }
}

void ShapeWorksGroom::auto_crop() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: auto_crop" << std::endl;
  }
  std::cout << "WARNING!  Assumes objects are centered in each image!" << std::endl;
  ImageType::SizeType maxsize;
  bool first = true;
  for (auto img : this->images_) {
    // Compute the bounding boxes.
    shapetools::bounding_box<float, 3> bb_tool;
    bb_tool.background() = this->background_;
    bb_tool(img);

    std::cout << bb_tool.region() << std::endl;

    ImageType::IndexType mycenter;
    for (unsigned i = 0; i < 3; i++) {
      mycenter[i] = img->GetRequestedRegion().GetIndex()[i]
        + (img->GetRequestedRegion().GetSize()[i] / 2);
    }

    if (first == true) {// save the first bounding box size 
      first = false;
      for (unsigned int i = 0; i < 3; i++) {
        maxsize[i] = bb_tool.region().GetSize()[i];
      }
    } else {
      // Keep the largest bounding box.
      ImageType::SizeType lowbdist, upbdist, sz;
      for (unsigned int i = 0; i < 3; i++) {
        // Compute the distance from lower bound to center
        lowbdist[i] = mycenter[i] - bb_tool.region().GetIndex()[i];

        // Compute the distance from upper bound to center
        upbdist[i] = (bb_tool.region().GetIndex()[i] + bb_tool.region().GetSize()[i]) - mycenter[i];

        // Take the larger of the two.
        if (lowbdist[i] > upbdist[i]){
          sz[i] = lowbdist[i];
        } else {
          sz[i] = upbdist[i];
        }

        // Use that to construct maxsize
        if (maxsize[i] < sz[i] * 2) {
          maxsize[i] = sz[i] * 2;
        }
      }
    }
  }

  for (unsigned int i = 0; i < 3; i++) {
    maxsize[i] += this->padding_;
  }
  if (this->verbose_) {
    std::cout << "crop size is " << maxsize << std::endl;
  }
  std::vector<ImageType::Pointer> imagesNew;
  for (auto img : this->images_) {

    itk::ExtractImageFilter<ImageType, ImageType>::Pointer extractor
      = itk::ExtractImageFilter<ImageType, ImageType>::New();

    // Construct region around center of image.
    ImageType::RegionType reg;
    ImageType::IndexType idx;

    ImageType::IndexType thisidx = img->GetRequestedRegion().GetIndex();
    ImageType::SizeType thissz = img->GetRequestedRegion().GetSize();

    int extra[3];
    bool pad = false;
    for (unsigned int i = 0; i < 3; i++) {
      extra[i] = 0;
      idx[i] = ((thisidx[i] + thissz[i] / 2) - maxsize[i] / 2);
      if (idx[i] < 0) {
        pad = true;
        extra[i] = -idx[i];
        if (this->verbose_) {
          std::cerr << "extra[" << i << "] = " << extra[i] << "\n";
        }
      }
    }
    itk::ConstantPadImageFilter<ImageType, ImageType>::Pointer padder
      = itk::ConstantPadImageFilter<ImageType, ImageType>::New();

    if (pad == true) {
      unsigned long lb[3];
      unsigned long up[3];
      for (unsigned int i = 0; i < 3; i++) {
        lb[i] = extra[i] * 2;
        up[i] = extra[i] * 2;
        if (this->verbose_) {
          std::cout << "pad lower bound [" << i << "] = " << lb[i] << std::endl;
          std::cout << "pad upper bound [" << i << "] = " << up[i] << std::endl;
        }
      }
      padder->SetConstant(this->background_);
      padder->SetPadLowerBound(lb);
      padder->SetPadUpperBound(up);
      padder->SetInput(img);
      padder->Update();
      if (this->verbose_) {
        std::cout << "Padding done" << std::endl;
      }
      thisidx = padder->GetOutput()->GetRequestedRegion().GetIndex();
      thissz = padder->GetOutput()->GetRequestedRegion().GetSize();

      for (unsigned int i = 0; i < 3; i++) {
        idx[i] = ((thisidx[i] + thissz[i] / 2) - maxsize[i] / 2);
      }
    }

    reg.SetSize(maxsize);
    reg.SetIndex(idx);

    if (this->verbose_) {
      std::cout << "extract maxsize = " << maxsize << std::endl;
      std::cout << "extract index = " << idx << std::endl;
    }
    if (pad == true) {
      extractor->SetInput(padder->GetOutput());
    } else {
      extractor->SetInput(img);
    }
    extractor->SetExtractionRegion(reg);
    extractor->Update();

    // Make sure the image information is correct.
    itk::Matrix<double, 3, 3> I;
    I.SetIdentity();
    extractor->GetOutput()->SetDirection(I);
    if (this->isEmpty(extractor->GetOutput())) { return; }
    imagesNew.push_back(extractor->GetOutput());
  }
  this->images_ = imagesNew;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: auto_crop" << std::endl;
  }
}

void ShapeWorksGroom::auto_pad() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: auto_pad" << std::endl;
  }

  ImageType::IndexType upper;
  ImageType::IndexType lower;

  bool first = true;
  for (auto img : this->images_) {
    if (first == true) // save the first bounding box
    {
      first = false;
      lower = img->GetLargestPossibleRegion().GetIndex();
      upper = lower + img->GetLargestPossibleRegion().GetSize();
    } else {
      // Keep the largest bounding box.
      ImageType::RegionType::IndexType lowerTmp
        = img->GetLargestPossibleRegion().GetIndex();
      ImageType::RegionType::IndexType upperTmp
        = lowerTmp + img->GetLargestPossibleRegion().GetSize();

      for (unsigned int i = 0; i < 3; i++)
      {
        if (lowerTmp[i] < lower[i])
        {
          lower[i] = lowerTmp[i];
        }
        if (upperTmp[i] > upper[i])
        {
          upper[i] = upperTmp[i];
        }
      }
    }
  }
  if (this->verbose_) {
    std::cout << "Lower bound = " << lower[0] << " " << lower[1]
      << " " << lower[2] << std::endl;
    std::cout << "Upper bound = " << upper[0] << " " << upper[1] << " "
      << upper[2] << std::endl;
  }
  // Make sure the origin is at the center of the image.
  double orig[3];
  for (unsigned int i = 0; i < 3; i++) {
    orig[i] = -static_cast<double>(upper[i] - lower[i]) / 2.0;
  }
  std::vector<ImageType::Pointer> newImages;
  for (auto img : this->images_) {
    itk::ConstantPadImageFilter<ImageType, ImageType>::Pointer padder
      = itk::ConstantPadImageFilter<ImageType, ImageType>::New();
    padder->SetConstant(0);
    padder->SetInput(img);

    // Find the necessary padding
    int diff[3];
    unsigned long lowpad[3];
    unsigned long hipad[3];
    bool flag = false;
    for (unsigned int i = 0; i < 3; i++) {
      diff[i] = (upper[i] - lower[i])
        - img->GetBufferedRegion().GetSize()[i];
      if (diff[i] < 0) {
        std::cerr << "auto_pad:: negative pad" << std::endl;
        throw 1;
      }
      lowpad[i] = diff[i] / 2;
      hipad[i] = diff[i] - lowpad[i];
      if (lowpad[i] != 0 || hipad[i] != 0) { flag = true; }
    }

    padder->SetPadUpperBound(hipad);
    padder->SetPadLowerBound(lowpad);
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
    if (this->isEmpty(padder->GetOutput())) { return; }
    newImages.push_back(padder->GetOutput());
  }
  this->images_ = newImages;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: auto_pad" << std::endl;
  }
}

bool ShapeWorksGroom::isEmpty(ImageType::Pointer image) {
  itk::ImageRegionConstIterator<ImageType>  it(image, image->GetRequestedRegion());
  while (!it.IsAtEnd()) {
    if (it.Get() != 0) {
      return false;
    }
    ++it;
  }
  std::cerr << "\tWarning! Tool failed and was skipped." << std::endl;
  return true;
}

void ShapeWorksGroom::antialias() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: antialias" << std::endl;
  }
  std::vector<ImageType::Pointer> out;
  for (auto img : this->images_) {
    itk::AntiAliasBinaryImageFilter<ImageType, ImageType>::Pointer anti
      = itk::AntiAliasBinaryImageFilter<ImageType, ImageType>::New();
    anti->SetInput(img);
    anti->SetNumberOfIterations(this->iterations_);
    anti->SetMaximumRMSError(0.024);
    anti->Update();
    if (this->isEmpty(anti->GetOutput())) { return; }
    out.push_back(anti->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: antialias" << std::endl;
  }
}

void ShapeWorksGroom::fastmarching() {
  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: fastmarching" << std::endl;
  }
  std::vector<ImageType::Pointer> out;
  for (auto img : this->images_) {
    itk::PSMImplicitSurfaceImageFilter<ImageType, ImageType>::Pointer P
      = itk::PSMImplicitSurfaceImageFilter<ImageType, ImageType>::New();
    P->SetSmoothingSigma(this->sigmaFastMarch_);
    P->SetIsosurfaceValue(this->iso_value_);
    P->SetInput(img);
    P->Update();
    if (this->isEmpty(P->GetOutput())) { return; }
    out.push_back(P->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: fastmarching" << std::endl;
  }
}

void ShapeWorksGroom::blur() {

  if (this->verbose_) {
    std::cout << "*** RUNNING TOOL: blur" << std::endl;
  }
  std::vector<ImageType::Pointer> out;
  for (auto img : this->images_) {
    itk::DiscreteGaussianImageFilter<ImageType, ImageType>::Pointer blur
      = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    blur->SetInput(img);
    blur->SetVariance(this->blurSigma_ * this->blurSigma_);
    blur->SetUseImageSpacingOff();
    blur->Update();
    if (this->isEmpty(blur->GetOutput())) { return; }
    out.push_back(blur->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: blur" << std::endl;
  }
}
