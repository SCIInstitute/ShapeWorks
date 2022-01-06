#pragma once

#include "Mesh.h"

#include <itkPointSet.h>

namespace shapeworks {

template<class TransformType>
class ReconstructSurface
{
public:
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  using TransformTypePtr = typename TransformType::Pointer;
  using PointSetType = typename TransformType::PointSetType;
  using PointIdType = typename PointSetType::PointIdentifier;

  ReconstructSurface() = default;
  ReconstructSurface(const std::string &denseFile, const std::string &sparseFile, const std::string &goodPointsFile);

  double computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices);

  void CheckMapping(TransformTypePtr transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints, Mesh::MeshPoints mappedCorrespondences,
                    double rms, double rms_wo_mapping, double maxmDist);

  void generateWarpedMeshes(TransformTypePtr transform, vtkSmartPointer<vtkPolyData>& outputMesh);

  Mesh getMesh(PointArray localPoints);

  Mesh::MeshPoints convertToImageCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  Mesh::MeshPoints convertToPhysicalCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  Eigen::MatrixXd computeParticlesNormals(vtkSmartPointer<vtkPoints> particles, Image dt);

  vtkSmartPointer<vtkPolyData> getDenseMean(std::vector<PointArray> localPoints,
                                            std::vector<PointArray> worldPoints, std::vector<std::string> distance_transform);

  void computeDenseMean(std::vector<PointArray> localPoints, std::vector<PointArray> worldPoints,
                        std::vector<std::string> distanceTransform);

  std::vector<PointArray> computeSparseMean(std::vector<PointArray> localPoints, Point3 commonCenter,
                                            bool doProcrustes, bool doProcrustesScaling);

  void surface(const std::vector<std::string> localPointsFiles);

  void samplesAlongPCAModes(const std::vector<std::string> worldPointsFiles);

  void meanSurface(const std::vector<std::string> distanceTransformFiles, const std::vector<std::string> localPointsFiles, const std::vector<std::string> worldPointsFiles);

  // set operations //

  void setOutPrefix(std::string prefix) { this->outPrefix = prefix; }

  void setOutPath(std::string path) { this->outPath = path; }

  void setModeIndex(int modeIndex) { this->modeIndex = modeIndex; }

  void setNumOfModes(int numOfModes) { this->numOfModes = numOfModes; }

  void setNumOfParticles(int numOfParticles) { this->numOfParticles = numOfParticles; }

  void setNumOfSamplesPerMode(int numOfSamplesPerMode) { this->numOfSamplesPerMode = numOfSamplesPerMode; }

  void setMaxStdDev(float maxStdDev) { this->maxStdDev = maxStdDev; }

  void setMaxVarianceCaptured(float maxVarianceCaptured) { this->maxVarianceCaptured = maxVarianceCaptured; }

  void setProcrustes(bool doProcrusts) { this->doProcrustes = doProcrustes; }

  void setProcrustesScaling(bool doProcrustsScaling) { this->doProcrustesScaling = doProcrustesScaling; }

private:
  float normalAngle = Pi/2.0;
  std::vector<std::string> localPointsFiles;
  std::vector<std::string> worldPointsFiles;
  std::vector<std::string> distanceTransformFiles;
  vtkSmartPointer<vtkPolyData> denseMean;
  Mesh::MeshPoints sparseMean;
  std::vector<bool> goodPoints;
  std::string outPrefix;
  std::string outPath;
  bool denseDone = true;
  bool doProcrustes;
  bool doProcrustesScaling;
  bool usePairwiseNormalsDifferencesForGoodBad;
  int modeIndex;
  int numOfModes;
  int numOfParticles;
  int numOfSamplesPerMode;
  float maxStdDev;
  float maxVarianceCaptured;
  float maxAngleDegrees;

  Mesh::MeshPoints setSparseMean(const std::string& sparsePath);
  std::vector<bool> setGoodPoints(const std::string& pointsPath);
  std::vector<PointArray> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<PointArray> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
