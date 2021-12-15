#pragma once

// #include "Shapeworks.h"
#include "Mesh.h"

// using namespace itk;

namespace shapeworks {

class ReconstructSurface
{
public:
  enum TransformType { ThinPlateSplineTransform, RBFSSparseTransform };
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  ReconstructSurface(const std::string& dense, const std::string& sparse, const std::string& goodPoints);

  void surface();
  void meanSurface();
  int samplesAlongPCAModes(int mode_index = -1, int number_of_modes = -1, float maximum_variance_captured = 0.95, int number_of_samples_per_mode = 10);
  Mesh getMesh(std::vector<Point3> local_pts);
  void writeOutFiles();

private:
  float normalAngle = Pi/2.0;
  std::string denseFile;
  std::string sparseFile;
  std::string goodPointsFile;
  std::vector<std::string> localPointsFile;
  std::vector<std::string> worldPointsFile;
  std::string out_prefix;

  Mesh denseMean;
  vtkSmartPointer<vtkPoints> sparseMean;
  std::vector<bool> goodPoints;

  static vtkSmartPointer<vtkPoints> getSparseMean(const std::string& sparsePath);
  std::vector<bool> getGoodPoints(const std::string& pointsPath);

};

} // shapeworks
