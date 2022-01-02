#pragma once

#include "Mesh.h"
#include "itkThinPlateSplineKernelTransform2.h"
#include "itkCompactlySupportedRBFSparseKernelTransform.h"

#include <itkPointSet.h>

namespace shapeworks {

class ReconstructSurface
{
public:
  enum TransformType { ThinPlateSplineTransform, RBFSSparseTransform };
  enum InterpType { LinearInterpolation, BSplineInterpolation };

  using transform1 = itk::ThinPlateSplineKernelTransform2<double, 3>;
  using transform2 = itk::CompactlySupportedRBFSparseKernelTransform<double, 3>;

  using PointSetTraitsType = itk::DefaultStaticMeshTraits<double, 3, 3, double, double>;
  using PointSetType = itk::PointSet<double, 3, PointSetTraitsType>;
  using PointIdType = PointSetType::PointIdentifier;

  ReconstructSurface(TransformType transform, InterpType interp);

  double computeAverageDistanceToNeighbors(Mesh::MeshPoints points, std::vector<int> particlesIndices);

  template<class T>
  void CheckMapping(Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints, typename T::Pointer transform, Mesh::MeshPoints& mappedCorrespondences,
                    double& rms, double& rms_wo_mapping, double& maxmDist);

  template<class T>
  void generateWarpedMeshes(typename T::Pointer transform, vtkSmartPointer<vtkPolyData>& outputMesh);

  template<class T>
  // template < template < typename Image::PixelType, unsigned > class T
  Mesh getMesh(typename T::Pointer transform, std::vector<Point3> localPoints);

  Eigen::MatrixXd computeParticlesNormals(Mesh::MeshPoints particles, Image dt);

  vtkSmartPointer<vtkPolyData> getDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distance_transform);

  void computeDenseMean(std::vector<std::vector<Point>> localPts, std::vector<std::vector<Point>> worldPts, std::vector<std::string> distance_transform);

  Mesh::MeshPoints convertToImageCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  Mesh::MeshPoints convertToPhysicalCoordinates(Mesh::MeshPoints particles, int numParticles, const Vector& spacing, const Point3& origin);

  void display();

  template<class T>
  void surface(typename T::Pointer transform, std::string denseFile, std::string sparseFile, std::string goodPointsFile);

  void meanSurface(const std::vector<std::string> distanceTransformFiles, const std::vector<std::string> localPointsFiles, const std::vector<std::string> worldPointsFiles);

  template<class T>
  void samplesAlongPCAModes(typename T::Pointer transform);

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
  std::string outPath; // check on this
  bool denseDone = true; // check on this
  int modeIndex;
  int numberOfModes;
  float maximumVarianceCaptured;
  int numberOfParticles;
  float maxStdDev;
  int numberOfSamplesPerMode;

  bool ifTransform1 = false;
  bool ifTransform2 = false;

  Mesh denseMean;
  Mesh::MeshPoints sparseMean;
  std::vector<bool> goodPoints;

  Mesh::MeshPoints setSparseMean(const std::string& sparsePath);
  std::vector<bool> setGoodPoints(const std::string& pointsPath);

  void setDistanceTransformFiles(const std::vector<std::string> dtFiles);
  std::vector<std::vector<Point3>> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<std::vector<Point3>> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
