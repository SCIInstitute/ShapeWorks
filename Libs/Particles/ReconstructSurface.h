#pragma once

#include "Mesh.h"

namespace shapeworks {

class ReconstructSurface
{
public:
  enum TransformType { ThinPlateSplineTransform, RBFSSparseTransform };
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  // ReconstructSurface(const std::string& dense, const std::string& sparse, const std::string& goodPoints);

  void surface();
  void meanSurface(const std::vector<std::string> distanceTransformFiles, const std::vector<std::string> localPointsFiles, const std::vector<std::string> worldPointsFiles);
  int samplesAlongPCAModes(int mode_index = -1, int number_of_modes = -1, float maximum_variance_captured = 0.95, int number_of_samples_per_mode = 10);
  Mesh getMesh(std::vector<Point3> local_pts);
  void display();
  Eigen::MatrixXd computeParticlesNormals(vtkSmartPointer<vtkPoints> particles, Image dt);
  vtkSmartPointer<vtkPolyData> getDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distance_transform);
  void computeDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distance_transform);
  void writeOutFiles();
  
  Mesh::MeshPoints convertToImageCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);
  
  Mesh::MeshPoints convertToPhysicalCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

private:
  float normalAngle = Pi/2.0;
  std::string denseFile;
  std::string sparseFile;
  std::string goodPointsFile;
  std::vector<std::string> localPointsFiles;
  std::vector<std::string> worldPointsFiles;
  std::vector<std::string> distanceTransformFiles;
  std::string out_prefix;
  std::string out_path; // check on this

  Mesh denseMean;
  vtkSmartPointer<vtkPoints> sparseMean;
  std::vector<bool> goodPoints;

  bool denseDone_ = true; // check on this

  static vtkSmartPointer<vtkPoints> getSparseMean(const std::string& sparsePath);
  std::vector<bool> getGoodPoints(const std::string& pointsPath);
  void setDistanceTransformFiles(const std::vector<std::string> dtFiles);
  std::vector<std::vector<Point3>> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<std::vector<Point3>> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
