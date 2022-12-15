---
title: Libs/Image/Image.h

---

# Libs/Image/Image.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Image](../Classes/classshapeworks_1_1Image.md)**  |




## Source code

```cpp
#pragma once

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkStatisticsImageFilter.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <limits>

#include "Region.h"
#include "Shapeworks.h"

namespace shapeworks {

class Mesh;

class Image {
 public:
  enum InterpolationType { Linear, NearestNeighbor };

  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;
  using StatsPtr = itk::StatisticsImageFilter<ImageType>::Pointer;
  using ImageIterator = itk::ImageRegionIterator<ImageType>;
  using InterpolatorType = itk::LinearInterpolateImageFunction<ImageType, Image::PixelType>;

  // constructors and assignment operators //
  Image(const Dims dims);
  Image(const std::string& pathname) : itk_image_(read(pathname)) {}
  Image(ImageType::Pointer imagePtr) : itk_image_(imagePtr) {
    if (!itk_image_) throw std::invalid_argument("null imagePtr");
  }
  Image(const vtkSmartPointer<vtkImageData> vtkImage);
  Image(Image&& img) : itk_image_(nullptr) { this->itk_image_.Swap(img.itk_image_); }
  Image(const Image& img) : itk_image_(cloneData(img.itk_image_)) {}
  Image& operator=(const Image& img);  
  Image& operator=(Image&& img);       

  operator ImageType::Pointer() { return itk_image_; }
  ImageType::Pointer getITKImage() const { return itk_image_; }

  vtkSmartPointer<vtkImageData> getVTKImage() const;

  // modification functions //

  Image& operator-();

  Image operator+(const Image& other) const;
  Image& operator+=(const Image& other);

  Image operator-(const Image& other) const;
  Image& operator-=(const Image& other);

  Image operator*(const Image& other) const;

  Image operator*(const PixelType x) const;
  Image& operator*=(const PixelType x);

  Image operator/(const PixelType x) const;
  Image& operator/=(const PixelType x);

  Image operator+(const PixelType x) const;
  Image& operator+=(const PixelType x);

  Image operator-(const PixelType x) const;
  Image& operator-=(const PixelType x);

  Image& antialias(unsigned iterations = 50, double maxRMSErr = 0.01f, int layers = 3);

  Image& recenter();

  Image& resample(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing,
                  const ImageType::DirectionType direction, InterpolationType interp = NearestNeighbor);

  Image& resample(const Vector& physicalSpacing, InterpolationType interp = Linear);

  Image& resample(double isoSpacing = 1.0, InterpolationType interp = Linear);

  Image& resize(Dims logicalDims, InterpolationType interp = Linear);

  Image& pad(int padding, PixelType value = 0.0);

  Image& pad(int padx, int pady, int padz, PixelType value = 0.0);

  Image& pad(IndexRegion& region, PixelType value = 0.0);

  Image& translate(const Vector3& v);

  Image& scale(const Vector3& v);

  Image& rotate(const double angle, const Vector3& axis);

  Image& rotate(const double angle, Axis axis);

  TransformPtr createCenterOfMassTransform();

  TransformPtr createRigidRegistrationTransform(const Image& target_dt, float isoValue = 0.0, unsigned iterations = 20);

  Image& applyTransform(const TransformPtr transform, InterpolationType interp = Linear);

  Image& applyTransform(const TransformPtr transform, const Point3 origin, const Dims dims, const Vector3 spacing,
                        const ImageType::DirectionType direction, InterpolationType interp = NearestNeighbor);

  Image& extractLabel(const PixelType label = 1.0);

  Image& closeHoles(const PixelType foreground = 0.0);

  Image& binarize(PixelType minVal = 0.0, PixelType maxVal = std::numeric_limits<PixelType>::max(),
                  PixelType innerVal = 1.0, PixelType outerVal = 0.0);

  Image& computeDT(PixelType isoValue = 0.0);

  Image& applyCurvatureFilter(unsigned iterations = 10);

  Image& applyGradientFilter();

  Image& applySigmoidFilter(double alpha = 10.0, double beta = 10.0);

  Image& applyTPLevelSetFilter(const Image& featureImage, double scaling = 20.0);

  Image& topologyPreservingSmooth(float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0);

  Image& applyIntensityFilter(double minVal, double maxVal);

  Image& gaussianBlur(double sigma = 0.0);

  Image& crop(PhysicalRegion region, const int padding = 0);

  Image& clip(const Plane plane, const PixelType val = 0.0);

  Image& reflect(const Axis& axis);

  Image& setOrigin(Point3 origin = Point3({0, 0, 0}));

  Image& setSpacing(Vector3 spacing);

  Image& setCoordsys(ImageType::DirectionType coordsys);

  Image& isolate();

  // query functions //

  Dims dims() const { return itk_image_->GetLargestPossibleRegion().GetSize(); }

  Point3 size() const { return toPoint(spacing()) * toPoint(dims()); }

  Vector spacing() const { return itk_image_->GetSpacing(); }

  Point3 origin() const { return itk_image_->GetOrigin(); }

  Point3 center() const { return origin() + size() / 2.0; }

  ImageType::DirectionType coordsys() const { return itk_image_->GetDirection(); };

  Point3 centerOfMass(PixelType minVal = 0.0, PixelType maxVal = 1.0) const;

  PixelType min();

  PixelType max();

  PixelType mean();

  PixelType std();

  IndexRegion logicalBoundingBox() const;

  PhysicalRegion physicalBoundingBox() const;

  PhysicalRegion physicalBoundingBox(PixelType isovalue) const;

  PhysicalRegion logicalToPhysical(IndexRegion region) const;

  IndexRegion physicalToLogical(PhysicalRegion region) const;

  Point3 logicalToPhysical(const Coord& c) const;

  Coord physicalToLogical(const Point3& p) const;

  ImageIterator iterator();

  bool compare(const Image& other, bool verifyall = true, double tolerance = 0.0, double precision = 1e-12) const;

  bool operator==(const Image& other) const { return compare(other); }

  // export functions //

  Image& write(const std::string& filename, bool compressed = true);

  Mesh toMesh(PixelType isovalue) const;

  Image::PixelType evaluate(Point p);

  static std::vector<std::string> getSupportedTypes() { return {"nrrd", "nii", "nii.gz", "mhd", "tiff", "jpeg", "jpg", "png", "dcm", "ima"}; }

 private:
  friend struct SharedCommandData;
  Image()
      : itk_image_(nullptr) {
  }  // only for use by SharedCommandData since an Image should always be valid, never "empty"

  static ImageType::Pointer read(const std::string& filename);
  static ImageType::Pointer readDICOMImage(const std::string& pathname);

  static ImageType::Pointer cloneData(const ImageType::Pointer img);

  static vtkSmartPointer<vtkPolyData> getPolyData(const Image& image, PixelType isoValue = 0.0);

  Image& pad(Dims lowerExtendRegion, Dims upperExtendRegion, PixelType value = 0.0);

  StatsPtr statsFilter();

  ImageType::Pointer itk_image_;

  InterpolatorType::Pointer interpolator_;
};

std::ostream& operator<<(std::ostream& os, const Image& img);

template <>
Image operator*(const Image& img, const double x);
template <>
Image operator/(const Image& img, const double x);
template <>
Image& operator*=(Image& img, const double x);
template <>
Image& operator/=(Image& img, const double x);

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-12-15 at 07:26:46 +0000
