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
  bool resamplevolume(bool isBinary, bool isCenterImage, float isoSpacing, Dims outputSize);
  // bool resamplevolumes(bool isBinary, float isoSpacing bool isCenterImageOn, Point3 size);  // maybe have to create a point3 class)

private:
  ImageType::Pointer image;
};

} // Shapeworks
