#pragma once

#include "ImageUtils.h"
#include <itkTranslationTransform.h>

namespace Shapeworks {

class Image
{
public:
  typedef float PixelType;
  typedef itk::Image<PixelType, 3/*dimension*/> ImageType;

  Image() {}
  Image(const std::string &inFilename) { read(inFilename); }

  bool read(const std::string &inFilename);
  bool write(const std::string &outFilename);
  bool antialias(float maxRMSErr = 0.01f, int numIter = 50);
  //bool crop(

private:
  ImageType::Pointer image;
};

} // Shapeworks
