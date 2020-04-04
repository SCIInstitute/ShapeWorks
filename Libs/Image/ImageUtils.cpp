#include "ImageUtils.h"

namespace shapeworks {

/// createCenterOfMassTransform
///
/// Generates the Transform necessary to move the contents of this binary image to the center.
/// Example:
///   Transform xform = ImageUtils::createCenterOfMassTransform(image);
///   image.applyTransform(xform);
///
/// \param image the binary image from which to generate the transform
Transform ImageUtils::createCenterOfMassTransform(const Image &image)
{
  Point3 com = image.centerOfMass();
  Point3 center = image.size() / 2.0 + image.origin();

  Transform xform;
  xform.translate(center - com);

  return xform;
}

/// topologyPreservingSmooth
///
/// const input version of topologyPreservingSmooth
Image& ImageUtils::topologyPreservingSmooth(const Image &image, const Image &featureImage,
                                            float scaling, float sigmoidAlpha, float sigmoidBeta,
                                            bool applyCurvatureFilter, unsigned curvatureIterations)
{
  Image *smoothed = new Image(image);
  return topologyPreservingSmooth(*smoothed, featureImage, scaling, sigmoidAlpha, sigmoidBeta, applyCurvatureFilter, curvatureIterations);
}

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
Image& ImageUtils::topologyPreservingSmooth(Image &image, const Image &featureImage,
                                            float scaling, float sigmoidAlpha, float sigmoidBeta,
                                            bool applyCurvatureFilter, unsigned curvatureIterations)
{
  if (applyCurvatureFilter)
    image.applyCurvatureFilter(curvatureIterations);
  image.applyGradientFilter();
  image.applySigmoidFilter(sigmoidAlpha, sigmoidBeta);
  image.applyTPLevelSetFilter(featureImage, scaling);
  return image;
}

} //shapeworks
