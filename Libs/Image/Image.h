#pragma once

#include "Shapeworks.h"
#include "Transform.h"
#include <limits>
#include <itkImage.h>

namespace shapeworks {

class Image
{
public:
  static const unsigned dims = 3;
  using PixelType = float;
  using ImageType = itk::Image<PixelType, dims>;

  Image(const std::string &filename) { read(filename); }

  bool read(const std::string &filename);
  bool write(const std::string &filename, bool compressed = true);

  bool antialias(unsigned numIterations = 50, float maxRMSErr = 0.01f, unsigned numLayers = dims); //todo: no need for a return value
  bool recenter();
  bool isoresample(double isoSpacing = 1.0f, Dims outputSize = Dims());
  bool pad(int padding, PixelType value);
  bool applyTransform(const Transform &transform);
  bool extractLabel(PixelType label = 1.0);
  bool closeHoles();
  bool threshold(PixelType min = std::numeric_limits<PixelType>::epsilon(), PixelType max = std::numeric_limits<float>::max());

  bool operator==(const Image &other) const;

  Point3 centerOfMass() const;  // returns average spatial coordinate of black pixels in a binary volume
  Point3 center() const;        // returns spatial coordinate of center of volume
  Point3 size() const;          // returns spatial size of the volume
  void print() const; //print center, size, and other stuff about the image; todo: operator<<

private:
  friend struct SharedCommandData;

  Image() {}
  ImageType::Pointer image;
};

} // shapeworks
