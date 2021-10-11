#pragma once

#include "Shapeworks.h"
#include "Reconstruction.h"
#include "Image.h"

using namespace itk;

namespace shapeworks {

class ReconstructSurface
{
public:
  ReconstructSurface();

  void surface();

private:
  // template<class TCoordRep, unsigned int> class TransformType;
  float normalAngle = Pi/2.0;
  // using TransformType = itk::CompactlySupportedRBFSparseKernelTransform<double, 3>;
  std::string denseFile;
  std::string sparseFile;
  std::string goodPointsFile;
  std::vector<std::string> localPointsFile;
  
  using ReconstructionType = Reconstruction <Reconstruction::TransformType, itk::LinearInterpolateImageFunction, double, float, Image::ImageType>;
  using PointArrayType = std::vector<Point3>;

};

} // shapeworks
