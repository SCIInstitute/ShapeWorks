#pragma once

#include <itkThinPlateSplineKernelTransform.h>

#include "Image.h"

namespace shapeworks {

using PixelType = float;
using ImageType = itk::Image<PixelType, 3>;

/// Thin plate spline transform whose weights are solved with Eigen.
///
/// ITK builds a 3(N+4) square system and solves it with a netlib SVD, which takes about an hour at N=2048. The TPS
/// kernel is |r| times the identity, so that system is three independent (N+4) square systems sharing one matrix.
/// Factoring that matrix once is several thousand times faster and gives the same weights.
class EigenThinPlateSplineKernelTransform : public itk::ThinPlateSplineKernelTransform<double, 3> {
 public:
  using Self = EigenThinPlateSplineKernelTransform;
  using Superclass = itk::ThinPlateSplineKernelTransform<double, 3>;
  using Pointer = itk::SmartPointer<Self>;
  using ConstPointer = itk::SmartPointer<const Self>;

  itkNewMacro(Self);
  itkOverrideGetNameOfClassMacro(EigenThinPlateSplineKernelTransform);

  /// Hides the (non-virtual) KernelTransform::ComputeWMatrix
  void ComputeWMatrix();

 protected:
  EigenThinPlateSplineKernelTransform() = default;
};

/// Helper functions for image
class ImageUtils {
 public:
  /// calculate bounding box for images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, Image::PixelType isoValue = 1.0);

  /// calculate bounding box for shapework images using the region of data <= the given isoValue
  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Image>>& images,
                                    Image::PixelType isoValue = 1.0);

  /// computes a thin plate spline (TSP) transform from the source to the target landmarks (in the given files) using
  /// every stride points
  using TPSTransform = EigenThinPlateSplineKernelTransform;
  static TPSTransform::Pointer createWarpTransform(const std::string& source_landmarks_file,
                                                   const std::string& target_landmarks_file, const int stride = 1);

  static void register_itk_factories();

  static ImageType::Pointer make_axis_aligned(ImageType::Pointer input);
};

}  // namespace shapeworks
