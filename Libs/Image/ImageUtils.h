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
  ///   sigmoid filter...
  ///   ...then take the results of curvature and apply the TPLevelSet filter, using the results of sigmoid as its feature image
  ///
  /// \param image the image to be smoothed
  /// \param scaling
  /// \param sigmoidAlpha
  /// \param sigmoidBeta
  /// \param curvatureIterations number of iterations to use for curvature smoothing
  /// \param applyCurvatureFilter default it true, but in some cases it has already been applied
  template<typename T>
  static Image& topologyPreservingSmooth(T& img, float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
                                         unsigned curvatureIterations = 10, bool applyCurvatureFilter = true)
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
    Image &featureImage(image);

    featureImage.applyGradientFilter();
    featureImage.applySigmoidFilter(sigmoidAlpha, sigmoidBeta);

    image.applyTPLevelSetFilter(featureImage, scaling);
    return image;
  }

};

} // shapeworks
