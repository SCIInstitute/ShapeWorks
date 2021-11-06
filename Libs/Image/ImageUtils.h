#pragma once

#include "Image.h"
#include "ShapeworksUtils.h"

#include <itkThinPlateSplineKernelTransform.h>

namespace shapeworks {

/// Helper functions for image 
class ImageUtils
{
public:
  /// calculate bounding box for images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames,
                                    Image::PixelType isoValue = 1.0);

  /// calculate bounding box for shapework images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Image>>& images,
                                    Image::PixelType isoValue = 1.0);

  /// computes a thin plate spline (TSP) transform from the source to the target landmarks (in the given files) using every stride points
  using TPSTransform = itk::ThinPlateSplineKernelTransform<double, 3>;
  static TPSTransform::Pointer createWarpTransform(const std::string& source_landmarks_file,
                                                   const std::string& target_landmarks_file,
                                                   const int stride = 1);

};

} // shapeworks

