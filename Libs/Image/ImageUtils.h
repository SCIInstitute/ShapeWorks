#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

#include <itkVectorLinearInterpolateImageFunction.h>
#include <itkGradientImageFilter.h>

namespace shapeworks {

/// Helper functions for image 
class ImageUtils
{
public:
  /// calculate bounding box incrementally for images using the region of data <= the given isoValue
  static Region boundingBox(std::vector<std::string> &filenames, Image::PixelType isoValue = 1.0);

  /// calculate bounding box incrementally for shapework images using the region of data <= the given isoValue
  static Region boundingBox(std::vector<Image> &images, Image::PixelType isoValue = 1.0);

  /// computes a warp transform from the source to the target landmarks
  static TransformPtr createWarpTransform(const std::string &source_landmarks, const std::string &target_landmarks, const int stride = 1);

  /// Returns a handy vector interpolator for the given Image (presumably a distance transform).
  /// Use it by passing points corresponding to locations in the Image's physical space:
  ///   `evaluator->Evaluate(pt);`
  /// It returns a Vector (which can be normalized using `v.Normalize()`).
  using GradientImageFilter = itk::GradientImageFilter<Image::ImageType>;
  using GradientInterpolator = itk::VectorLinearInterpolateImageFunction<
    GradientImageFilter::OutputImageType, typename Image::PixelType>;
  static itk::SmartPointer<GradientInterpolator> getGradientInterpolator(const Image& dt);

};

/// wrapper for the itk gradient interpreter so it can be bound to python (can this be done better?)
class VectorImageInterpolator
{
public:
  VectorImageInterpolator(itk::SmartPointer<ImageUtils::GradientInterpolator> I) : interpolator(I) {}
  Vector evaluate(Point p) { return toVector(interpolator->Evaluate(p)); }

private:
  itk::SmartPointer<ImageUtils::GradientInterpolator> interpolator;
};

} // shapeworks
