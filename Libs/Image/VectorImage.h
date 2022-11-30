#pragma once

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>

#include "Image.h"

namespace shapeworks {

//! Gradient (vector) image
class VectorImage {
 public:
  using GradientImageFilter = itk::GradientImageFilter<Image::ImageType>;
  using ImageType = itk::Image<Covariant, 3>;
  using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<ImageType, Image::PixelType>;
  using ImageIterator = itk::ImageRegionIterator<ImageType>;

  /// Creates a gradient vector image of image (presumably a distance transform)
  VectorImage(const Image& dt_img);
  VectorImage() = delete;
  ~VectorImage() = default;

  /// Returns a Vector (which can be normalized using `v.Normalize()`).
  Vector evaluate(Point p);
  ImageIterator iterator();

 private:
  itk::SmartPointer<ImageType> itk_image_;
  itk::SmartPointer<GradientInterpolatorType> interpolator_;
};

}  // namespace shapeworks
