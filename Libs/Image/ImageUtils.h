#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

namespace shapeworks {

/// Helper functions for image 
class ImageUtils
{
public:
  /// calculate bounding box incrementally for images using the region of data <= the given isoValue
  static Image::Region boundingBox(std::vector<std::string> &filenames);

  /// Generates the Transform necessary to move the contents of this binary image to the center.
  static TransformPtr createCenterOfMassTransform(const Image &image);

  /// Computes a warp transform from source to target
  static TransformPtr createWarpTransform(const std::string &source_file, const std::string &target_file, const int pointFactor = 1);

  /// Return Transform necessary to align this image with target.
  static Image& rigidRegistration(Image &img, const Image &target, const Image &source, float isoValue = 0.0, unsigned iterations = 20);

  /// topologyPreservingSmooth
  ///
  /// Topology preserving smoothing first creates a feature image (by applying gradient then sigmoid filters, 
  /// then passes them to the TPLevelSet filter.
  /// Note that the the curvature flow filter is often applied to the image before this filter.
  static Image& topologyPreservingSmooth(Image& img, float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0);

  /// Create an isotropic resampling of the given image volume
  ///
  /// \param isoSpacing     size of an [isotropic (n x n x n)] output voxel [default n=1]
  /// \param outputSize     image size can be changed [default stays the same]
  static Image& isoresample(Image& img, double isoSpacing = 1.0, Dims outputSize = Dims());
};

} // shapeworks
