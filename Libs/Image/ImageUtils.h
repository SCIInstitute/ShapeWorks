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
#if 0 // <ctc> currently disabled due to trickiness in creating Python binding (maybe not impossible, just no time right now)
  template<typename T>
  static Image& topologyPreservingSmooth(T& img, float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
                                         unsigned curvatureIterations = 10, bool applyCurvatureFilter = true)
  {
    // When img is const, create new Image to return. Otherwise apply directly to [const_cast'd non-const] img.
    Image& image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image&>(img);

    if (applyCurvatureFilter)
      image.applyCurvatureFilter(curvatureIterations);
    Image featureImage(image);

    featureImage.applyGradientFilter();
    featureImage.applySigmoidFilter(sigmoidAlpha, sigmoidBeta);

    image.applyTPLevelSetFilter(featureImage, scaling);
    return image;
  }
#else
  static Image& topologyPreservingSmooth(Image& image, float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
                                         unsigned curvatureIterations = 10, bool applyCurvatureFilter = true)
  {
    if (applyCurvatureFilter)
      image.applyCurvatureFilter(curvatureIterations);
    Image featureImage(image);

    featureImage.applyGradientFilter();
    featureImage.applySigmoidFilter(sigmoidAlpha, sigmoidBeta);

    image.applyTPLevelSetFilter(featureImage, scaling);
    return image;
  }
#endif

  /// isoresample
  ///
  /// create an isotropic resampling of the given image volume
  ///
  /// \param isoSpacing     size of an [isotropic (n x n x n)] output voxel [default n=1]
  /// \param outputSize     image size can be changed [default stays the same]
#if 0 // <ctc> currently disabled due to trickiness in creating Python binding (maybe not impossible, just no time right now)
  template<typename T>
  static Image& isoresample(T& img, double isoSpacing = 1.0, Dims outputSize = Dims())
  {
    // When img is const, create new Image to return. Otherwise apply directly to [const_cast'd non-const] img.
    Image& image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image&>(img);

    Point3 spacing({isoSpacing, isoSpacing, isoSpacing});
    image.resample(spacing, outputSize);
    return image;
  }
#else
  static Image& isoresample(Image& image, double isoSpacing = 1.0, Dims outputSize = Dims())
  {
    Point3 spacing({isoSpacing, isoSpacing, isoSpacing});
    image.resample(spacing, outputSize);
    return image;
  }
#endif
};

} // shapeworks
