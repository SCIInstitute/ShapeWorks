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
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[getMesh](../Classes/classshapeworks_1_1ReconstructSurface.md#function-getmesh)**(std::vector< Point3 > localPoints) |
| void | **[surface](../Classes/classshapeworks_1_1ReconstructSurface.md#function-surface)**(const std::vector< std::string > localPointsFiles) |
| void | **[samplesAlongPCAModes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-samplesalongpcamodes)**(const std::vector< std::string > worldPointsFiles) |
| void | **[setOutPrefix](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setoutprefix)**(std::string prefix) |
| void | **[setOutPath](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setoutpath)**(std::string path) |
| void | **[setModeIndex](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmodeindex)**(int modeIndex) |
| void | **[setNumOfModes](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofmodes)**(int numOfModes) |
| void | **[setMaxVarianceCaptured](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmaxvariancecaptured)**(float maxVarianceCaptured) |
| void | **[setNumOfParticles](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofparticles)**(int numOfParticles) |
| void | **[setMaxStdDev](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setmaxstddev)**(float maxStdDev) |
| void | **[setNumOfSamplesPerMode](../Classes/classshapeworks_1_1ReconstructSurface.md#function-setnumofsamplespermode)**(int numOfSamplesPerMode) |

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
    std::vector< Point3 > localPoints
)
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


### function setMaxVarianceCaptured

```cpp
inline void setMaxVarianceCaptured(
    float maxVarianceCaptured
)
```


### function setNumOfParticles

```cpp
inline void setNumOfParticles(
    int numOfParticles
)
```


### function setMaxStdDev

```cpp
inline void setMaxStdDev(
    float maxStdDev
)
```


### function setNumOfSamplesPerMode

```cpp
inline void setNumOfSamplesPerMode(
    int numOfSamplesPerMode
)
```


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000