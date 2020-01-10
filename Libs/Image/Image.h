#pragma once

#include "Shapeworks.h"
#include "ImageUtils.h"
#include <itkTranslationTransform.h>

namespace Shapeworks {

class Image
{
public:
  static const unsigned dims = 3;
  using PixelType = float;
  using ImageType = itk::Image<PixelType, dims>;

  Image() {}
  Image(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &inFilename);
  bool write(const std::string &outFilename);
  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = dims);
  bool binarize(PixelType threshold = itk::NumericTraits<PixelType>::Zero,
                PixelType inside = itk::NumericTraits<PixelType>::One,
                PixelType outside = itk::NumericTraits<PixelType>::Zero);
  bool recenter();
  bool resample(bool isBinary = false, float isoSpacing = 1.0f, Dims outputSize = Dims());
  // bool nextfunction(...);

private:
  ImageType::Pointer image;
};

} // Shapeworks
