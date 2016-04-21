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
#include <map>

ShapeWorksGroom::ShapeWorksGroom(
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground, 
  double sigma, double sigmaFastMarch,
  double iso, size_t padding,  size_t iterations,
  bool verbose)
  :  images_(inputs), 
  background_(background), sigma_(sigma), sigmaFastMarch_(sigmaFastMarch),
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
  if (this->runTools_.count("hole_fill")) {
    this->hole_fill();
  }
  if (this->runTools_.count("isolate")) {
    this->isolate();
  }
  if (this->runTools_.count("auto_crop")) {
    if (this->runTools_.count("fastmarching")) {
      std::cerr << 
        "Warning: auto_crop incompatible with fastmarching. "
        << "Skipping auto_crop." << std::endl;
    } else {
      this->auto_crop();
    }
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
    if (!this->runTools_.count("fastmarching") &&
      !this->runTools_.count("antialias")) {
      std::cerr <<
        "Warning: blur requires fastmarching or antialias. "
        << "Skipping blur." << std::endl;
    } else {
      this->blur();
    }
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
    typedef itk::CastImageFilter< ImageType, isolate_type > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(img);
    auto imgIsolate = filter->GetOutput();
    // Find the connected components in this image.
    itk::ConnectedComponentImageFilter<isolate_type, isolate_type>::Pointer
      ccfilter = itk::ConnectedComponentImageFilter<isolate_type, isolate_type>::New();

    ccfilter->SetInput(imgIsolate);
    ccfilter->FullyConnectedOn();
    ccfilter->Update();

    // First find the size of the connected components.
    std::map<int, int> sizes;
    itk::ImageRegionConstIterator<isolate_type> rit(ccfilter->GetOutput(),
      img->GetBufferedRegion());
    for (; !rit.IsAtEnd(); ++rit) {
      if (rit.Get() > 0) {
        sizes[rit.Get()] = sizes[rit.Get()] + 1;
      }
    }
    // Find largest connected component. Assumes connected component algorithm
    // enumerates sequentially. (This is true for the ITK algorithm).
    int maxsize = 0;
    int bigcomponent = 0;
    for (std::map<int, int>::const_iterator mapit = sizes.begin();
      mapit != sizes.end(); mapit++) {
      if ((*mapit).second > maxsize) {
        bigcomponent = (*mapit).first;
        maxsize = (*mapit).second;
      }
    }
    // Copy result into return image. Remove all but the largest connected component.
    itk::ImageRegionIterator<isolate_type> it(imgIsolate, imgIsolate->GetBufferedRegion());
    rit.GoToBegin();
    it.GoToBegin();
    for (; !rit.IsAtEnd(); ++it, ++rit) {
      if (rit.Get() == bigcomponent) it.Set(0.);
      else it.Set(1.);
    }
    typedef itk::CastImageFilter<isolate_type, ImageType > FilterType2;
    FilterType2::Pointer filter2 = FilterType2::New();
    filter2->SetInput(imgIsolate);
    filter2->Update();
    newImages.push_back(filter2->GetOutput());
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
    // Flood fill the background with the foreground value.
    itk::ConnectedThresholdImageFilter<ImageType, ImageType>::Pointer
      ccfilter = itk::ConnectedThresholdImageFilter<ImageType, ImageType>::New();
    ccfilter->SetInput(img);
    ccfilter->SetLower(this->background_);
    ccfilter->SetUpper(this->background_);
    ccfilter->SetSeed(this->seed_);
    ccfilter->SetReplaceValue(this->foreground_);
    ccfilter->Update();

    // "or" the original image with the inverse (foreground/background flipped)
    // of the flood-filled copy.

    itk::ImageRegionConstIterator<ImageType> rit(ccfilter->GetOutput(),
      img->GetBufferedRegion());
    itk::ImageRegionIterator<ImageType> it(img, img->GetBufferedRegion());
    for (; !rit.IsAtEnd(); ++rit, ++it) {
      if (rit.Get() == this->background_) {
        it.Set(this->foreground_);
      }
    }
    out.push_back(ccfilter->GetOutput());
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
    typedef itk::CastImageFilter< ImageType, crop_type > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetInput(img);
    filter->Update();
    auto imgCrop = filter->GetOutput();
    // Compute the bounding boxes.
    shapetools::bounding_box<unsigned char, 3> bb_tool;
    bb_tool.background() = this->background_;
    bb_tool(imgCrop);

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
    newImages.push_back(padder->GetOutput());
  }
  this->images_ = newImages;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: auto_pad" << std::endl;
  }
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
    blur->SetVariance(this->sigma_ * this->sigma_);
    //blur->SetUseImageSpacingOff();
    blur->Update();
    out.push_back(blur->GetOutput());
  }
  this->images_ = out;
  if (this->verbose_) {
    std::cout << "*** FINISHED RUNNING TOOL: blur" << std::endl;
  }
}
