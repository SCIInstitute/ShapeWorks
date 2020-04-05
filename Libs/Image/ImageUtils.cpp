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
// Image& ImageUtils::topologyPreservingSmooth(const Image &image, const Image &featureImage,
//                                             float scaling, float sigmoidAlpha, float sigmoidBeta,
//                                             bool applyCurvatureFilter, unsigned curvatureIterations)
// {
//   Image *smoothed = new Image(image);
//   return topologyPreservingSmooth(*smoothed, featureImage, scaling, sigmoidAlpha, sigmoidBeta, applyCurvatureFilter, curvatureIterations);
// }

} //shapeworks
