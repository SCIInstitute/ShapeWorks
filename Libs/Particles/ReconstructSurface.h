#pragma once

#include "Shapeworks.h"
#include "Reconstruction.h"
#include "Image.h"

using namespace itk;

namespace shapeworks {

class ReconstructSurface
{
public:
  enum TransformType { ThinPlateSplineTransform, RBFSSparseTransform };
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  ReconstructSurface(TransformType transform, InterpType);

  void surface();
  void meanSurface();
  void samplesAlongPCAModes();

private:
  float normalAngle = Pi/2.0;
  std::string denseFile;
  std::string sparseFile;
  std::string goodPointsFile;
  std::vector<std::string> localPointsFile;
  std::string out_prefix;

  using PointArrayType = std::vector<Point3>;
  using TCoordRep = double;
  using ReconstructionType = Reconstruction <itk::ThinPlateSplineKernelTransform2,
                          itk::LinearInterpolateImageFunction,
                          TCoordRep, Image::PixelType, Image::ImageType>;
  
  ReconstructionType reconstructor_;

};

} // shapeworks
