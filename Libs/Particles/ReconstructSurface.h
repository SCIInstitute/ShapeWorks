#pragma once

#include "Libs/Alignment/Transforms/itkThinPlateSplineKernelTransform2.h"
#include "Libs/Alignment/Transforms/itkCompactlySupportedRBFSparseKernelTransform.h"
#include "Mesh.h"

#include <itkPointSet.h>

namespace shapeworks {

/// Transforms that can be used for ReconstructSurface
using ThinPlateSplineTransform = itk::ThinPlateSplineKernelTransform2<double, 3>;
using RBFSSparseTransform      = itk::CompactlySupportedRBFSparseKernelTransform<double, 3>;

template<class TransformType>
class ReconstructSurface
{
public:
  using TransformTypePtr = typename TransformType::Pointer;
  using PointSetType = typename TransformType::PointSetType;
  using PointIdType = typename PointSetType::PointIdentifier;

  ReconstructSurface() = default;
  ReconstructSurface(const std::string &denseFile, const std::string &sparseFile, const std::string &goodPointsFile);

  double computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices);

  void checkMapping(TransformTypePtr transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints);

  void generateWarpedMeshes(TransformTypePtr transform, vtkSmartPointer<vtkPolyData>& outputMesh);

  Mesh getMesh(PointArray localPoints);

  Mesh::MeshPoints convertToImageCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  Mesh::MeshPoints convertToPhysicalCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  int computeMedianShape(std::vector<Eigen::MatrixXd>& shapeList);

  void performKMeansClustering(std::vector<PointArray> worldPoints, int numberOfParticles, std::vector<int>& centroidIndices);

  Eigen::MatrixXd computeParticlesNormals(vtkSmartPointer<vtkPoints> particles, Image dt);

  vtkSmartPointer<vtkPolyData> extractIsoSurface(Image volData);

  vtkSmartPointer<vtkPolyData> getDenseMean(std::vector<PointArray> localPoints,
                                            std::vector<PointArray> worldPoints,
                                            std::vector<std::string> distance_transform);

  void computeDenseMean(std::vector<PointArray> localPoints,
                        std::vector<PointArray> worldPoints,
                        std::vector<std::string> distanceTransform);

  std::vector<PointArray> computeSparseMean(std::vector<PointArray> localPoints, Point3 commonCenter);

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
  bool meanBeforeWarp = true;
  bool outputEnabled = true;
  int modeIndex;
  int numOfModes;
  int numOfSamplesPerMode;
  int numOfParticles;
  size_t numOfClusters;
  float maxStdDev;
  float maxVarianceCaptured;
  float maxAngleDegrees;

  Mesh::MeshPoints setSparseMean(const std::string& sparsePath);
  std::vector<bool> setGoodPoints(const std::string& pointsPath);
  std::vector<PointArray> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<PointArray> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
