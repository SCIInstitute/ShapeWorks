#pragma once

#include "Image.h"

namespace shapeworks {

class ImageUtils
{
public:
  static Transform createCenterOfMassTransform(const Image &image);
  // static Image& topologyPreservingSmooth(Image &image, const Image &featureImage,
  //                                        float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
  //                                        bool applyCurvatureFilter = true, unsigned curvatureIterations = 10);
  template<typename IMAGE>
  static Image& topologyPreservingSmooth(IMAGE &image, const Image &featureImage,
                                         float scaling = 20.0, float sigmoidAlpha = 10.5, float sigmoidBeta = 10.0,
                                         bool applyCurvatureFilter = true, unsigned curvatureIterations = 10);
};

} // shapeworks
