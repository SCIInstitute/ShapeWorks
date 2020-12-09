#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

namespace shapeworks {

/// Helper functions for image 
class ImageUtils
{
public:
  /// calculate bounding box incrementally for images using the region of data <= the given isoValue
  static Image::Region boundingBox(std::vector<std::string> &filenames, Image::PixelType isoValue = 1.0);

  /// calculate bounding box incrementally for shapework images using the region of data <= the given isoValue
  static Image::Region boundingBox(std::vector<Image> &images, Image::PixelType isoValue = 1.0);

  /// generates the Transform necessary to move the contents of this binary image to the center
  static TransformPtr createCenterOfMassTransform(const Image &image);

  /// creates transform from source distance map to target using ICP registration (isovalue is used to create meshes from dts passed to ICP)
  static TransformPtr createRigidRegistrationTransform(const Image &source_dt, const Image &target_dt, float isoValue = 0.0, unsigned iterations = 20);

  /// computes a warp transform from the source to the target landmarks
  static TransformPtr createWarpTransform(const std::string &source_landmarks, const std::string &target_landmarks, const int stride = 1);

  /// creates a feature image (by applying gradient then sigmoid filters), then passes it to the TPLevelSet filter [curvature flow filter is often applied to the image before this filter]
  static Image& topologyPreservingSmooth(Image& image, float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0);

  /// create an isotropic resampling of the given image volume
  static Image& isoresample(Image& image, double isoSpacing = 1.0, Image::InterpolationType interp = Image::Linear);
};

} // shapeworks
