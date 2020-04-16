#pragma once

#include "Image.h"

namespace shapeworks {

class ImageUtils
{
public:

  /// createCenterOfMassTransform
  ///
  /// Generates the Transform necessary to move the contents of this binary image to the center.
  /// Example:
  ///   Transform xform = ImageUtils::createCenterOfMassTransform(image);
  ///   image.applyTransform(xform);
  ///
  /// \param image the binary image from which to generate the transform
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

  template <typename T>
  static Image &reflect(T &img, double axis)
  {
    Image &image = std::is_const<std::remove_reference<decltype(image)>>::value ? *new Image(img) : const_cast<Image &>(img);

    double x = 1, y = 1, z = 1;
    if (axis == 0)
      x = -1;
    if (axis == 1)
      y = -1;
    if (axis == 2)
      z = -1;

    Matrix33 reflection;
    reflection[0][0] = x;
    reflection[0][1] = reflection[0][2] = 0;
    reflection[1][1] = y;
    reflection[1][0] = reflection[1][2] = 0;
    reflection[2][2] = z;
    reflection[2][0] = reflection[2][1] = 0;

    Point3 origin = image.origin();
    image.recenter();
    Transform xform;
    xform.setMatrix(reflection);
    image.applyTransform(xform);
    image.changeOrigin();
    return image;
  }

};

} // shapeworks
