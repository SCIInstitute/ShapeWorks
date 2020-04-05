#pragma once

#include "Image.h"

namespace shapeworks {

class ImageUtils
{
public:
  static Transform createCenterOfMassTransform(const Image &image);
  static Image& topologyPreservingSmooth(Image &image, const Image &featureImage,
  float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
  bool applyCurvatureFilter = true, unsigned curvatureIterations = 10);
  
  static Image& topologyPreservingSmooth(const Image &image, const Image &featureImage,
  float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
  bool applyCurvatureFilter = true, unsigned curvatureIterations = 10);
};

} // shapeworks
