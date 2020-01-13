#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"
#include <itkTranslationTransform.h>

namespace shapeworks {

class Image
{
public:
  static const unsigned dims = 3;
  using PixelType = float;
  using ImageType = itk::Image<PixelType, dims>;

  Image() {}
  Image(const std::string &inFilename) { read(inFilename); }

  /// read
  /// \param filename
  bool read(const std::string &inFilename);

  /// write
  /// \param filename
  /// \param useCompression
  bool write(const std::string &outFilename, bool useCompression = true);

  /// antialias
  /// \param numIterations
  /// \param maxRMSErr
  /// \param numLayers      size of region around a pixel to sample
  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = dims);

  /// binarizes image into two regions separated by threshold value
  /// \param threshold  values <= threshold are considereed "outside" and given that value [default is 0.0]
  /// \param inside     value for inside region [default is 1]
  /// \param outside    value for outside region [default is 0]
  bool binarize(PixelType threshold = itk::NumericTraits<PixelType>::Zero,
                PixelType inside = itk::NumericTraits<PixelType>::One,
                PixelType outside = itk::NumericTraits<PixelType>::Zero);

  /// recenters by changing origin (in the image header) to the physcial coordinates of the center of the image
  bool recenter();

  /// resample
  /// \param isoSpacing
  /// \param outputSize     image size can be changed
  /// \param bSplineInterp  use bspline interpolation (default is linear)
  bool resample(float isoSpacing = 1.0f, Dims outputSize = Dims(), bool bSplineInterp = false);

  /// nextfunction
  /// \param isoSpacing
  // bool nextfunction(...);

  bool compare_equal(const Image &other);

private:
  ImageType::Pointer image;
};

} // shapeworks
