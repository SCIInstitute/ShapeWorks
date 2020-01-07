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
  bool antialias(float maxRMSErr = 0.01f, int numIter = 50);
  bool resamplevolume(bool isBinary = false, bool recenter = false, float isoSpacing = 1.0f, Dims outputSize = Dims());
  bool padvolume(int paddingSize, float paddingValue);
  // bool nextfunction(...);

private:
  ImageType::Pointer image;
};

} // Shapeworks
