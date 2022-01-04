#pragma once

#include "Mesh.h"

#include <itkPointSet.h>

namespace shapeworks {

template<class TransformType>
class ReconstructSurface
{
public:
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  using PointSetTraitsType = itk::DefaultStaticMeshTraits<double, 3, 3, double, double>;
  using PointSetType = itk::PointSet<double, 3, PointSetTraitsType>;
  using PointIdType = PointSetType::PointIdentifier;

  ReconstructSurface() = default;
  ReconstructSurface(const std::vector<std::string> &localPointsFiles);

  double computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices);

  void CheckMapping(TransformType transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints, Mesh::MeshPoints& mappedCorrespondences,
                    double& rms, double& rms_wo_mapping, double& maxmDist);

  void generateWarpedMeshes(TransformType transform, vtkSmartPointer<vtkPolyData>& outputMesh);

  Mesh getMesh(TransformType transform, std::vector<Point3> localPoints);

  void display();

  void surface(TransformType transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile);

  void samplesAlongPCAModes(TransformType transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile);

  // set operations //

  void setOutPrefix(std::string prefix) { this->outPrefix = prefix; }

private:
  float normalAngle = Pi/2.0;
  std::string denseFile;
  std::string sparseFile;
  std::string goodPointsFile;
  std::vector<std::string> localPointsFiles;
  std::vector<std::string> worldPointsFiles;
  std::vector<std::string> distanceTransformFiles;
  std::string outPrefix;
  bool denseDone = true;
  int modeIndex;
  int numberOfModes;
  float maximumVarianceCaptured;
  int numberOfParticles;
  float maxStdDev;
  int numberOfSamplesPerMode;

  vtkSmartPointer<vtkPolyData> denseMean;
  Mesh::MeshPoints sparseMean;
  std::vector<bool> goodPoints;

  Mesh::MeshPoints setSparseMean(const std::string& sparsePath);
  std::vector<bool> setGoodPoints(const std::string& pointsPath);

  std::vector<std::vector<Point3>> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<std::vector<Point3>> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
