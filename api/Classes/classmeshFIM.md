---
title: meshFIM

---

# meshFIM





## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[LabelType](../Classes/classmeshFIM.md#enum-labeltype)** { MaskPoint, SeedPoint, ActivePoint, FarPoint, StopPoint, AlivePoint, ToBeAlivePoint} |
| typedef int | **[VoxelIndexType](../Classes/classmeshFIM.md#typedef-voxelindextype)**  |
| typedef int | **[index](../Classes/classmeshFIM.md#typedef-index)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[ComputeDistanceToLandmarksGivenTriangleInfo](../Classes/classmeshFIM.md#function-computedistancetolandmarksgiventriangleinfo)**(TriMesh * mesh, const char * infilename, const char * outfilename) |
| void | **[computeFIM](../Classes/classmeshFIM.md#function-computefim)**(TriMesh * mesh, const char * vertT_filename) |
| void | **[GetFeatureValues](../Classes/classmeshFIM.md#function-getfeaturevalues)**(point x, std::vector< float > & vals) |
| void | **[ReadFaceIndexMap](../Classes/classmeshFIM.md#function-readfaceindexmap)**(const char * infilename) |
| void | **[ReadFeatureFromFile](../Classes/classmeshFIM.md#function-readfeaturefromfile)**(const char * infilename) |
| void | **[ReadFeatureGradientFromFile](../Classes/classmeshFIM.md#function-readfeaturegradientfromfile)**(const char * infilename) |
| point | **[GetFeatureDerivative](../Classes/classmeshFIM.md#function-getfeaturederivative)**(point p, int fIndex) |
| void | **[need_abs_curvatures](../Classes/classmeshFIM.md#function-need-abs-curvatures)**() |
| void | **[need_edge_lengths](../Classes/classmeshFIM.md#function-need-edge-lengths)**() |
| void | **[need_speed](../Classes/classmeshFIM.md#function-need-speed)**() |
| void | **[need_oneringfaces](../Classes/classmeshFIM.md#function-need-oneringfaces)**() |
| void | **[need_kdtree](../Classes/classmeshFIM.md#function-need-kdtree)**() |
| void | **[SetMesh](../Classes/classmeshFIM.md#function-setmesh)**(TriMesh * mesh) |
| void | **[SetStopDistance](../Classes/classmeshFIM.md#function-setstopdistance)**(float d) |
| void | **[setSpeedType](../Classes/classmeshFIM.md#function-setspeedtype)**(int st) |
| | **[meshFIM](../Classes/classmeshFIM.md#function-meshfim)**() |
| | **[~meshFIM](../Classes/classmeshFIM.md#function-~meshfim)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| TriMesh * | **[m_meshPtr](../Classes/classmeshFIM.md#variable-m-meshptr)**  |
| int | **[NumComputation](../Classes/classmeshFIM.md#variable-numcomputation)**  |
| float | **[imageOrigin](../Classes/classmeshFIM.md#variable-imageorigin)**  |
| float | **[imageSpacing](../Classes/classmeshFIM.md#variable-imagespacing)**  |
| int | **[imageSize](../Classes/classmeshFIM.md#variable-imagesize)**  |
| int | **[imageIndex](../Classes/classmeshFIM.md#variable-imageindex)**  |
| std::vector< Color > | **[colors](../Classes/classmeshFIM.md#variable-colors)**  |

## Public Types Documentation

### enum LabelType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| MaskPoint | |   |
| SeedPoint | |   |
| ActivePoint | |   |
| FarPoint | |   |
| StopPoint | |   |
| AlivePoint | |   |
| ToBeAlivePoint | |   |




### typedef VoxelIndexType

```cpp
typedef int meshFIM::VoxelIndexType;
```


### typedef index

```cpp
typedef int meshFIM::index;
```


## Public Functions Documentation

### function ComputeDistanceToLandmarksGivenTriangleInfo

```cpp
void ComputeDistanceToLandmarksGivenTriangleInfo(
    TriMesh * mesh,
    const char * infilename,
    const char * outfilename
)
```


### function computeFIM

```cpp
void computeFIM(
    TriMesh * mesh,
    const char * vertT_filename
)
```


### function GetFeatureValues

```cpp
void GetFeatureValues(
    point x,
    std::vector< float > & vals
)
```


### function ReadFaceIndexMap

```cpp
void ReadFaceIndexMap(
    const char * infilename
)
```


### function ReadFeatureFromFile

```cpp
void ReadFeatureFromFile(
    const char * infilename
)
```


### function ReadFeatureGradientFromFile

```cpp
void ReadFeatureGradientFromFile(
    const char * infilename
)
```


### function GetFeatureDerivative

```cpp
point GetFeatureDerivative(
    point p,
    int fIndex
)
```


### function need_abs_curvatures

```cpp
void need_abs_curvatures()
```


### function need_edge_lengths

```cpp
void need_edge_lengths()
```


### function need_speed

```cpp
void need_speed()
```


### function need_oneringfaces

```cpp
void need_oneringfaces()
```


### function need_kdtree

```cpp
void need_kdtree()
```


### function SetMesh

```cpp
void SetMesh(
    TriMesh * mesh
)
```


### function SetStopDistance

```cpp
inline void SetStopDistance(
    float d
)
```


### function setSpeedType

```cpp
inline void setSpeedType(
    int st
)
```


### function meshFIM

```cpp
inline meshFIM()
```


### function ~meshFIM

```cpp
inline ~meshFIM()
```


## Public Attributes Documentation

### variable m_meshPtr

```cpp
TriMesh * m_meshPtr;
```


### variable NumComputation

```cpp
int NumComputation;
```


### variable imageOrigin

```cpp
float imageOrigin;
```


### variable imageSpacing

```cpp
float imageSpacing;
```


### variable imageSize

```cpp
int imageSize;
```


### variable imageIndex

```cpp
int imageIndex;
```


### variable colors

```cpp
std::vector< Color > colors;
```


-------------------------------

Updated on 2022-01-07 at 20:14:45 +0000