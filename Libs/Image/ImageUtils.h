#pragma once

#include "Image.h"

namespace shapeworks {

class ImageUtils
{
public:
  static Transform createCenterOfMassTransform(const Image &image);

  /// topologyPreservingSmooth
  ///
  /// Applies topology preserving smooth then returns the image. Smoothing is four filter steps:
  ///   curvature flow filter
  ///   gradient filter
  ///   sigmoid filter
  ///   TPLevelSet filter (formerly part of ITK but now only included in ShapeWorks)
  ///
  /// \param image the image to be smoothed
  /// \param featureImage the other image to use for the TPLevelSet filter
  /// \param scaling
  /// \param sigmoidAlpha
  /// \param sigmoidBeta
  /// \param curvatureIterations number of iterations to use for curvature smoothing
  /// \param applyCurvatureFilter default it true, but in some cases it has already been applied
  template<typename T>
  static Image& topologyPreservingSmooth(T& img, const Image& featureImage,
                                         float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
                                         bool applyCurvatureFilter = true, unsigned curvatureIterations = 10)
  {
    // Works but requires a const_cast in case foo is const, even though when it is const a new
    // object is created because when foo is a const ref it can't be assigned to a non-const.
    Image& image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image&>(img);

    // debug: just print it to feel good that it's what we think it is
    if (std::is_const<std::remove_reference<decltype(image)>>::value) // consider that a reference to a const is not itself const
      std::cout << "argument is const ref\n";
    else
      std::cout << "argument is non-const ref\n";

    if (applyCurvatureFilter)
      image.applyCurvatureFilter(curvatureIterations);
    image.applyGradientFilter();
    image.applySigmoidFilter(sigmoidAlpha, sigmoidBeta);
    image.applyTPLevelSetFilter(featureImage, scaling);
    return image;
  }

};

} // shapeworks
