---
title: shapeworks::ReconstructSurface

---

# shapeworks::ReconstructSurface



 [More...](#detailed-description)

## Public Types

|                | Name           |
| -------------- | -------------- |
| using typename TransformType::Pointer | **[TransformTypePtr](../Classes/classshapeworks_1_1ReconstructSurface.md#using-transformtypeptr)**  |
| using typename TransformType::PointSetType | **[PointSetType](../Classes/classshapeworks_1_1ReconstructSurface.md#using-pointsettype)**  |
| using typename PointSetType::PointIdentifier | **[PointIdType](../Classes/classshapeworks_1_1ReconstructSurface.md#using-pointidtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ReconstructSurface](../Classes/classshapeworks_1_1ReconstructSurface.md#function-reconstructsurface)**() =default |
| | **[ReconstructSurface](../Classes/classshapeworks_1_1ReconstructSurface.md#function-reconstructsurface)**(const std::string & denseFile, const std::string & sparseFile, const std::string & goodPointsFile) |
| double | **[computeAverageDistanceToNeighbors](../Classes/classshapeworks_1_1ReconstructSurface.md#function-computeaveragedistancetoneighbors)**(Mesh::MeshPoints points, std::vector< int > particlesIndices) |
| void | **[checkMapping](../Classes/classshapeworks_1_1ReconstructSurface.md#function-checkmapping)**(TransformTypePtr transform, Mesh::MeshPoints sourcePoints, Mesh::MeshPoints targetPoints) |
| void | **[generateWarpedMeshes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-generatewarpedmeshes)**(TransformTypePtr transform, vtkSmartPointer< vtkPolyData > & outputMesh) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[getMesh](../Classes/classshapeworks_1_1ReconstructSurface.md#function-getmesh)**(PointArray localPoints) |
| Mesh::MeshPoints | **[convertToImageCoordinates](../Classes/classshapeworks_1_1ReconstructSurface.md#function-converttoimagecoordinates)**(Mesh::MeshPoints particles, const Vector & spacing, const Point3 & origin) |
| int | **[computeMedianShape](../Classes/classshapeworks_1_1ReconstructSurface.md#function-computemedianshape)**(std::vector< Eigen::MatrixXd > & shapeList) |
| void | **[performKMeansClustering](../Classes/classshapeworks_1_1ReconstructSurface.md#function-performkmeansclustering)**(std::vector< PointArray > worldPoints, int numberOfParticles, std::vector< int > & centroidIndices) |
| Eigen::MatrixXd | **[computeParticlesNormals](../Classes/classshapeworks_1_1ReconstructSurface.md#function-computeparticlesnormals)**(vtkSmartPointer< vtkPoints > particles, [Image](../Classes/classshapeworks_1_1Image.md) dt) |
| vtkSmartPointer< vtkPolyData > | **[getDenseMean](../Classes/classshapeworks_1_1ReconstructSurface.md#function-getdensemean)**(std::vector< PointArray > localPoints, std::vector< PointArray > worldPoints, std::vector< std::string > distance_transform) |
| void | **[computeDenseMean](../Classes/classshapeworks_1_1ReconstructSurface.md#function-computedensemean)**(std::vector< PointArray > localPoints, std::vector< PointArray > worldPoints, std::vector< std::string > distanceTransform) |
| std::vector< PointArray > | **[computeSparseMean](../Classes/classshapeworks_1_1ReconstructSurface.md#function-computesparsemean)**(std::vector< PointArray > localPoints, Point3 commonCenter) |
| void | **[writeMeanInfo](../Classes/classshapeworks_1_1ReconstructSurface.md#function-writemeaninfo)**() |
| void | **[surface](../Classes/classshapeworks_1_1ReconstructSurface.md#function-surface)**(const std::vector< std::string > localPointsFiles) |
| void | **[samplesAlongPCAModes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-samplesalongpcamodes)**(const std::vector< std::string > worldPointsFiles) |
| void | **[meanSurface](../Classes/classshapeworks_1_1ReconstructSurface.md#function-meansurface)**(const std::vector< std::string > distanceTransformFiles, const std::vector< std::string > localPointsFiles, const std::vector< std::string > worldPointsFiles) |
| void | **[setOutPrefix](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setoutprefix)**(std::string prefix) |
| void | **[setOutPath](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setoutpath)**(std::string path) |
| void | **[setDoProcrustes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setdoprocrustes)**(bool doProcrusts) |
| void | **[setDoProcrustesScaling](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setdoprocrustesscaling)**(bool doProcrustsScaling) |
| void | **[setPairwiseNormalsDiffForGoodBad](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setpairwisenormalsdiffforgoodbad)**(bool pairwiseNormalsDiffForGoodBad) |
| void | **[setMeanBeforeWarp](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmeanbeforewarp)**(bool meanBeforeWarp) |
| void | **[setEnableOutput](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setenableoutput)**(bool enableOutput) |
| void | **[setModeIndex](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmodeindex)**(int modeIndex) |
| void | **[setNumOfModes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofmodes)**(int numOfModes) |
| void | **[setNumOfSamplesPerMode](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofsamplespermode)**(int numOfSamplesPerMode) |
| void | **[setNumOfParticles](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofparticles)**(int numOfParticles) |
| void | **[setNumOfClusters](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofclusters)**(int numOfClusters) |
| void | **[setMaxStdDev](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmaxstddev)**(float maxStdDev) |
| void | **[setMaxVarianceCaptured](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmaxvariancecaptured)**(float maxVarianceCaptured) |
| void | **[setMaxAngleDegrees](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmaxangledegrees)**(float maxAngleDegrees) |

## Detailed Description

```cpp
template <class TransformType >
class shapeworks::ReconstructSurface;
```

## Public Types Documentation

### using TransformTypePtr

```cpp
using shapeworks::ReconstructSurface< TransformType >::TransformTypePtr =  typename TransformType::Pointer;
```


### using PointSetType

```cpp
using shapeworks::ReconstructSurface< TransformType >::PointSetType =  typename TransformType::PointSetType;
```


### using PointIdType

```cpp
using shapeworks::ReconstructSurface< TransformType >::PointIdType =  typename PointSetType::PointIdentifier;
```


## Public Functions Documentation

### function ReconstructSurface

```cpp
ReconstructSurface() =default
```


### function ReconstructSurface

```cpp
ReconstructSurface(
    const std::string & denseFile,
    const std::string & sparseFile,
    const std::string & goodPointsFile
)
```


### function computeAverageDistanceToNeighbors

```cpp
double computeAverageDistanceToNeighbors(
    Mesh::MeshPoints points,
    std::vector< int > particlesIndices
)
```


### function checkMapping

```cpp
void checkMapping(
    TransformTypePtr transform,
    Mesh::MeshPoints sourcePoints,
    Mesh::MeshPoints targetPoints
)
```


### function generateWarpedMeshes

```cpp
void generateWarpedMeshes(
    TransformTypePtr transform,
    vtkSmartPointer< vtkPolyData > & outputMesh
)
```


### function getMesh

```cpp
Mesh getMesh(
    PointArray localPoints
)
```


### function convertToImageCoordinates

```cpp
Mesh::MeshPoints convertToImageCoordinates(
    Mesh::MeshPoints particles,
    const Vector & spacing,
    const Point3 & origin
)
```


### function computeMedianShape

```cpp
int computeMedianShape(
    std::vector< Eigen::MatrixXd > & shapeList
)
```


### function performKMeansClustering

```cpp
void performKMeansClustering(
    std::vector< PointArray > worldPoints,
    int numberOfParticles,
    std::vector< int > & centroidIndices
)
```


### function computeParticlesNormals

```cpp
Eigen::MatrixXd computeParticlesNormals(
    vtkSmartPointer< vtkPoints > particles,
    Image dt
)
```


### function getDenseMean

```cpp
vtkSmartPointer< vtkPolyData > getDenseMean(
    std::vector< PointArray > localPoints,
    std::vector< PointArray > worldPoints,
    std::vector< std::string > distance_transform
)
```


### function computeDenseMean

```cpp
void computeDenseMean(
    std::vector< PointArray > localPoints,
    std::vector< PointArray > worldPoints,
    std::vector< std::string > distanceTransform
)
```


### function computeSparseMean

```cpp
std::vector< PointArray > computeSparseMean(
    std::vector< PointArray > localPoints,
    Point3 commonCenter
)
```


### function writeMeanInfo

```cpp
void writeMeanInfo()
```


### function surface

```cpp
void surface(
    const std::vector< std::string > localPointsFiles
)
```


### function samplesAlongPCAModes

```cpp
void samplesAlongPCAModes(
    const std::vector< std::string > worldPointsFiles
)
```


### function meanSurface

```cpp
void meanSurface(
    const std::vector< std::string > distanceTransformFiles,
    const std::vector< std::string > localPointsFiles,
    const std::vector< std::string > worldPointsFiles
)
```


### function setOutPrefix

```cpp
inline void setOutPrefix(
    std::string prefix
)
```


### function setOutPath

```cpp
inline void setOutPath(
    std::string path
)
```


### function setDoProcrustes

```cpp
inline void setDoProcrustes(
    bool doProcrusts
)
```


### function setDoProcrustesScaling

```cpp
inline void setDoProcrustesScaling(
    bool doProcrustsScaling
)
```


### function setPairwiseNormalsDiffForGoodBad

```cpp
inline void setPairwiseNormalsDiffForGoodBad(
    bool pairwiseNormalsDiffForGoodBad
)
```


### function setMeanBeforeWarp

```cpp
inline void setMeanBeforeWarp(
    bool meanBeforeWarp
)
```


### function setEnableOutput

```cpp
inline void setEnableOutput(
    bool enableOutput
)
```


### function setModeIndex

```cpp
inline void setModeIndex(
    int modeIndex
)
```


### function setNumOfModes

```cpp
inline void setNumOfModes(
    int numOfModes
)
```


### function setNumOfSamplesPerMode

```cpp
inline void setNumOfSamplesPerMode(
    int numOfSamplesPerMode
)
```


### function setNumOfParticles

```cpp
inline void setNumOfParticles(
    int numOfParticles
)
```


### function setNumOfClusters

```cpp
inline void setNumOfClusters(
    int numOfClusters
)
```


### function setMaxStdDev

```cpp
inline void setMaxStdDev(
    float maxStdDev
)
```


### function setMaxVarianceCaptured

```cpp
inline void setMaxVarianceCaptured(
    float maxVarianceCaptured
)
```


### function setMaxAngleDegrees

```cpp
inline void setMaxAngleDegrees(
    float maxAngleDegrees
)
```


-------------------------------

Updated on 2022-03-31 at 23:33:47 +0000