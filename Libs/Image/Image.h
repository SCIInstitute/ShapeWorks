#pragma once

#include "Shapeworks.h"
#include "Transform.h"
#include <limits>
#include <itkImage.h>

namespace shapeworks {

class Image
{
public:
  using PixelType = float;
  using ImageType = itk::Image<PixelType, 3>;

  Image(const std::string &filename) { read(filename); }

  bool read(const std::string &filename);
  bool write(const std::string &filename, bool compressed = true);

  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = 3); //todo: no need for a return value
  bool recenter();
  bool isoresample(double isoSpacing = 1.0f, Dims outputSize = Dims());
  bool pad(int padding, PixelType value);
  bool extractLabel(PixelType label = 1.0);
  bool closeHoles();
  bool threshold(PixelType min = std::numeric_limits<PixelType>::epsilon(), PixelType max = std::numeric_limits<PixelType>::max());
  bool fastMarch(float isoValue = 0.0);

  bool operator==(const Image &other) const;

  Point3 centerOfMass() const;  
  Point3 origin() const { return image->GetOrigin(); }
  Dims dims() const { return image->GetLargestPossibleRegion().GetSize(); }
  Point3 size() const;                                      // spatial size of image
  Point3 center() const { return origin() + size() / 2.0; } // spatial coordinates of center of this image

  void print() const; //print center, size, and other stuff about the image; todo: operator<<

private:
  friend struct SharedCommandData;
  Image() {}

  static bool is_directory(const std::string &pathname); // TODO: Move this function to Libs/Utils
  bool read_image_dir(const std::string &pathname);

  ImageType::Pointer image;
};

} // shapeworks
