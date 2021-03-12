#pragma once

#include "Image.h"

#include <itkVectorLinearInterpolateImageFunction.h>
#include <itkGradientImageFilter.h>

namespace shapeworks {

/// Image composed of vectors instead of just scalars
//
// TODO: generalize Image class instead of this:
//       https://github.com/SCIInstitute/ShapeWorks/issues/1053
class VectorImage
{
public:

  using GradientImageFilter = itk::GradientImageFilter<Image::ImageType>;
  using ImageType = GradientImageFilter::OutputImageType;
  using GradientInterpolator = itk::VectorLinearInterpolateImageFunction<
    ImageType, typename Image::PixelType>;

  /// Creates a gradient vector image of image (presumably a distance transform)
  VectorImage(const Image& dt_img);
  VectorImage() = delete;
  ~VectorImage() = default;

  /// Returns a Vector (which can be normalized using `v.Normalize()`).
  Vector evaluate(Point p) { return toVector(interpolator->Evaluate(p)); }

private:
  itk::SmartPointer<ImageType> image;
  itk::SmartPointer<GradientInterpolator> interpolator;
};

} // shapeworks
