#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

namespace shapeworks {

/// Helper functions for image 
class ImageUtils
{
public:
  /// calculate bounding box incrementally for images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, Image::PixelType isoValue = 1.0);

  /// calculate bounding box incrementally for shapework images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<const Image>& images, Image::PixelType isoValue = 1.0);

  /// computes a warp transform from the source to the target landmarks
  static TransformPtr createWarpTransform(const std::string &source_landmarks, const std::string &target_landmarks, const int stride = 1);

};

} // shapeworks
