#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

namespace shapeworks {

class ImageUtils
{
public:
  /// calculate bounding box incrementally for images using the region of data <= the given isoValue
  static Image::Region boundingBox(std::vector<std::string> &filenames, Image::PixelType isoValue = 1.0);

  /// Generates the Transform necessary to move the contents of this binary image to the center.
  static Transform::Pointer createCenterOfMassTransform(const Image &image);

  /// Return Transform necessary to align this image with target.
  static Transform::Pointer rigidRegistration(const Image &img, Image &target, Image &source, float isoValue = 0.0, unsigned iterations = 20);

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

  /// isoresample
  ///
  /// create an isotropic resampling of the given image volume
  ///
  /// \param isoSpacing     size of an [isotropic (n x n x n)] output voxel [default n=1]
  /// \param outputSize     image size can be changed [default stays the same]
  template<typename T>
  static Image& isoresample(T& img, double isoSpacing = 1.0, Dims outputSize = Dims())
  {
    // When img is const, create new Image to return. Otherwise apply directly to [const_cast'd non-const] img.
    Image& image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image&>(img);

    Point3 spacing({isoSpacing, isoSpacing, isoSpacing});
    image.resample(spacing, outputSize);
    return image;
  }

};

} // shapeworks
