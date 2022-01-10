---
title: Libs/Particles/ReconstructSurface.h

---

# Libs/Particles/ReconstructSurface.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ReconstructSurface](../Classes/classshapeworks_1_1ReconstructSurface.md)**  |




## Source code

```cpp
#pragma once

#include "Libs/Alignment/Transforms/itkThinPlateSplineKernelTransform2.h"
#include "Libs/Alignment/Transforms/itkCompactlySupportedRBFSparseKernelTransform.h"
#include "Mesh.h"

#include <itkPointSet.h>

namespace shapeworks {

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

  Mesh getMesh(std::vector<Point3> localPoints);

  void surface(const std::vector<std::string> localPointsFiles);

  void samplesAlongPCAModes(const std::vector<std::string> worldPointsFiles);

  // set operations //

  void setOutPrefix(std::string prefix) { this->outPrefix = prefix; }

  void setOutPath(std::string path) { this->outPath = path; }

  void setModeIndex(int modeIndex) { this->modeIndex = modeIndex; }

  void setNumOfModes(int numOfModes) { this->numOfModes = numOfModes; }

  void setMaxVarianceCaptured(float maxVarianceCaptured) { this->maxVarianceCaptured = maxVarianceCaptured; }

  void setNumOfParticles(int numOfParticles) { this->numOfParticles = numOfParticles; }

  void setMaxStdDev(float maxStdDev) { this->maxStdDev = maxStdDev; }

  void setNumOfSamplesPerMode(int numOfSamplesPerMode) { this->numOfSamplesPerMode = numOfSamplesPerMode; }  

private:
  float normalAngle = Pi/2.0;
  std::vector<std::string> localPointsFiles;
  std::vector<std::string> worldPointsFiles;
  vtkSmartPointer<vtkPolyData> denseMean;
  Mesh::MeshPoints sparseMean;
  std::vector<bool> goodPoints;
  std::string outPrefix;
  std::string outPath;
  bool denseDone = true;
  int modeIndex = -1;
  int numOfModes;
  float maxVarianceCaptured = 0.95;
  int numOfParticles;
  float maxStdDev = 2.0;
  int numOfSamplesPerMode;

  Mesh::MeshPoints setSparseMean(const std::string& sparsePath);
  std::vector<bool> setGoodPoints(const std::string& pointsPath);
  std::vector<std::vector<Point3>> setLocalPointsFiles(const std::vector<std::string> localPointsFiles);
  std::vector<std::vector<Point3>> setWorldPointsFiles(const std::vector<std::string> worldPointsFiles);

};

} // shapeworks
```


-------------------------------

Updated on 2022-01-10 at 16:27:27 +0000
