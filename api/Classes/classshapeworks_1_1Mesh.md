---
title: shapeworks::Mesh

---

# shapeworks::Mesh

**Module:** **[Mesh Classes](../Modules/group__Group-Mesh.md)**



 [More...](#detailed-description)


`#include <Mesh.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[AlignmentType](../Classes/classshapeworks_1_1Mesh.md#enum-alignmenttype)** { Rigid, Similarity, Affine} |
| enum| **[DistanceMethod](../Classes/classshapeworks_1_1Mesh.md#enum-distancemethod)** { PointToPoint, PointToCell} |
| enum| **[CurvatureType](../Classes/classshapeworks_1_1Mesh.md#enum-curvaturetype)** { Principal, Gaussian, Mean} |
| enum| **[SubdivisionType](../Classes/classshapeworks_1_1Mesh.md#enum-subdivisiontype)** { Butterfly, Loop} |
| using vtkSmartPointer< vtkPolyData > | **[MeshType](../Classes/classshapeworks_1_1Mesh.md#using-meshtype)**  |
| using vtkSmartPointer< vtkPoints > | **[MeshPoints](../Classes/classshapeworks_1_1Mesh.md#using-meshpoints)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(const std::string & pathname) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(MeshType meshPtr) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & orig) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) && orig) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator=](../Classes/classshapeworks_1_1Mesh.md#function-operator=)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & orig) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator=](../Classes/classshapeworks_1_1Mesh.md#function-operator=)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) && orig) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator+=](../Classes/classshapeworks_1_1Mesh.md#function-operator+=)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & otherMesh)<br>append two meshes  |
| MeshType | **[getVTKMesh](../Classes/classshapeworks_1_1Mesh.md#function-getvtkmesh)**() const<br>return the current mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[write](../Classes/classshapeworks_1_1Mesh.md#function-write)**(const std::string & pathname, bool binaryFile =false)<br>writes mesh, format specified by filename extension  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[coverage](../Classes/classshapeworks_1_1Mesh.md#function-coverage)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & otherMesh, bool allowBackIntersections =true, double angleThreshold =0, double backSearchRadius =0)<br>determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[smooth](../Classes/classshapeworks_1_1Mesh.md#function-smooth)**(int iterations =0, double relaxation =0.0)<br>applies laplacian smoothing  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[smoothSinc](../Classes/classshapeworks_1_1Mesh.md#function-smoothsinc)**(int iterations =0, double passband =0.0)<br>applies vtk windowed sinc smoothing  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[remesh](../Classes/classshapeworks_1_1Mesh.md#function-remesh)**(int numVertices, double adaptivity =1.0)<br>applies remeshing using approximated centroidal voronoi diagrams for a given number of vertices and adaptivity  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[remeshPercent](../Classes/classshapeworks_1_1Mesh.md#function-remeshpercent)**(double percentage, double adaptivity =1.0)<br>applies remeshing using approximated centroidal voronoi diagrams for a given percentage of vertices and adaptivity  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[invertNormals](../Classes/classshapeworks_1_1Mesh.md#function-invertnormals)**()<br>handle flipping normals  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[reflect](../Classes/classshapeworks_1_1Mesh.md#function-reflect)**(const Axis & axis, const Vector3 & origin =makeVector({ 0.0, 0.0, 0.0 }))<br>reflect meshes with respect to a specified center and specific axis  |
| MeshTransform | **[createTransform](../Classes/classshapeworks_1_1Mesh.md#function-createtransform)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & target, AlignmentType align =Similarity, unsigned iterations =10)<br>creates transform to target mesh using specified AlignmentType (Mesh::Rigid, Mesh::Similarity, Mesh::Affine) for specified number of iterations  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[applyTransform](../Classes/classshapeworks_1_1Mesh.md#function-applytransform)**(const MeshTransform transform)<br>applies the given transformation to the mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[fillHoles](../Classes/classshapeworks_1_1Mesh.md#function-fillholes)**()<br>finds holes in a mesh and closes them  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[probeVolume](../Classes/classshapeworks_1_1Mesh.md#function-probevolume)**(const [Image](../Classes/classshapeworks_1_1Image.md) & image)<br>samples image data values at point locations specified by image  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[clip](../Classes/classshapeworks_1_1Mesh.md#function-clip)**(const Plane plane)<br>clips a mesh using a cutting plane  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[translate](../Classes/classshapeworks_1_1Mesh.md#function-translate)**(const Vector3 & v)<br>helper to translate mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[scale](../Classes/classshapeworks_1_1Mesh.md#function-scale)**(const Vector3 & v)<br>helper to scale mesh  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1Mesh.md#function-boundingbox)**() const<br>computes bounding box of current mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[fixElement](../Classes/classshapeworks_1_1Mesh.md#function-fixelement)**()<br>fix element winding of mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[distance](../Classes/classshapeworks_1_1Mesh.md#function-distance)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & target, const DistanceMethod method =PointToPoint)<br>computes surface to surface distance, compute method: PointToPoint (default) or PointToCell  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[clipClosedSurface](../Classes/classshapeworks_1_1Mesh.md#function-clipclosedsurface)**(const Plane plane)<br>clips a mesh using a cutting plane resulting in a closed surface  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[computeNormals](../Classes/classshapeworks_1_1Mesh.md#function-computenormals)**()<br>computes and adds oriented point and cell normals  |
| Point3 | **[closestPoint](../Classes/classshapeworks_1_1Mesh.md#function-closestpoint)**(const Point3 point)<br>returns closest point on a face in the mesh to the given point in space  |
| int | **[closestPointId](../Classes/classshapeworks_1_1Mesh.md#function-closestpointid)**(const Point3 point)<br>returns closest point id in this mesh to the given point in space  |
| double | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(int source, int target)<br>computes geodesic distance between two vertices (specified by their indices) on mesh  |
| Field | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(const Point3 landmark)<br>computes geodesic distance between a point (landmark) and each vertex on mesh  |
| Field | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(const std::vector< Point3 > curve)<br>computes geodesic distance between a set of points (curve) and each vertex on mesh  |
| Field | **[curvature](../Classes/classshapeworks_1_1Mesh.md#function-curvature)**(const CurvatureType type =Principal)<br>computes and adds curvature (principal (default) or gaussian or mean)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[applySubdivisionFilter](../Classes/classshapeworks_1_1Mesh.md#function-applysubdivisionfilter)**(const SubdivisionType type =Butterfly, int subdivision =1)<br>applies subdivision filter (butterfly (default) or loop)  |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[toImage](../Classes/classshapeworks_1_1Mesh.md#function-toimage)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region =[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md)(), Point spacing =Point({1., 1., 1.})) const<br>rasterizes specified region to create binary image of desired dims (default: unit spacing)  |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[toDistanceTransform](../Classes/classshapeworks_1_1Mesh.md#function-todistancetransform)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region =[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md)(), Point spacing =Point({1., 1., 1.})) const<br>converts specified region to distance transform image (default: unit spacing)  |
| Point3 | **[center](../Classes/classshapeworks_1_1Mesh.md#function-center)**() const<br>center of mesh  |
| Point3 | **[centerOfMass](../Classes/classshapeworks_1_1Mesh.md#function-centerofmass)**() const<br>center of mass of mesh  |
| int | **[numPoints](../Classes/classshapeworks_1_1Mesh.md#function-numpoints)**() const<br>number of points  |
| int | **[numFaces](../Classes/classshapeworks_1_1Mesh.md#function-numfaces)**() const<br>number of faces  |
| Eigen::MatrixXd | **[points](../Classes/classshapeworks_1_1Mesh.md#function-points)**() const<br>matrix with number of points with (x,y,z) coordinates of each point  |
| Eigen::MatrixXi | **[faces](../Classes/classshapeworks_1_1Mesh.md#function-faces)**() const<br>matrix with number of faces with indices of the three points from which each face is composed  |
| Point3 | **[getPoint](../Classes/classshapeworks_1_1Mesh.md#function-getpoint)**(int id) const<br>(x,y,z) coordinates of vertex at given index  |
| IPoint3 | **[getFace](../Classes/classshapeworks_1_1Mesh.md#function-getface)**(int id) const<br>return indices of the three points with which the face at the given index is composed  |
| std::vector< std::string > | **[getFieldNames](../Classes/classshapeworks_1_1Mesh.md#function-getfieldnames)**() const<br>print all field names in mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[setField](../Classes/classshapeworks_1_1Mesh.md#function-setfield)**(std::string name, Array array)<br>sets the given field for points with array (*does not copy array's values)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[setFieldForFaces](../Classes/classshapeworks_1_1Mesh.md#function-setfieldforfaces)**(std::string name, Array array)<br>sets the given field for faces with array (*does not copy array's values)  |
| template <typename T \> <br>vtkSmartPointer< T > | **[getField](../Classes/classshapeworks_1_1Mesh.md#function-getfield)**(const std::string & name) const<br>gets a pointer to the requested field, null if field doesn't exist  |
| void | **[setFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-setfieldvalue)**(const std::string & name, int idx, double value)<br>sets the given index of field to value  |
| double | **[getFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-getfieldvalue)**(const std::string & name, int idx) const<br>gets the value at the given index of field (NOTE: returns first component of vector fields)  |
| Eigen::VectorXd | **[getMultiFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-getmultifieldvalue)**(const std::string & name, int idx) const<br>gets the multi value at the given index of field  |
| std::vector< double > | **[getFieldRange](../Classes/classshapeworks_1_1Mesh.md#function-getfieldrange)**(const std::string & name) const<br>returns the range of the given field (NOTE: returns range of first component of vector fields)  |
| double | **[getFieldMean](../Classes/classshapeworks_1_1Mesh.md#function-getfieldmean)**(const std::string & name) const<br>returns the mean the given field  |
| double | **[getFieldStd](../Classes/classshapeworks_1_1Mesh.md#function-getfieldstd)**(const std::string & name) const<br>returns the standard deviation of the given field  |
| bool | **[compareAllPoints](../Classes/classshapeworks_1_1Mesh.md#function-compareallpoints)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh) const<br>compare if values of the points in two (corresponding) meshes are (eps)equal  |
| bool | **[compareAllFaces](../Classes/classshapeworks_1_1Mesh.md#function-compareallfaces)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh) const<br>compare if face indices in two (corresponding) meshes are equal  |
| bool | **[compareAllFields](../Classes/classshapeworks_1_1Mesh.md#function-compareallfields)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const double eps =-1.0) const<br>compare if all fields in two meshes are (eps)equal  |
| bool | **[compareField](../Classes/classshapeworks_1_1Mesh.md#function-comparefield)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const std::string & name1, const std::string & name2 ="", const double eps =-1.0) const<br>compare field of meshes to be (eps)equal (same field for both if only one specified)  |
| bool | **[compare](../Classes/classshapeworks_1_1Mesh.md#function-compare)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const double eps =-1.0) const<br>compare meshes  |
| bool | **[operator==](../Classes/classshapeworks_1_1Mesh.md#function-operator==)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other) const<br>compare meshes  |
| bool | **[splitMesh](../Classes/classshapeworks_1_1Mesh.md#function-splitmesh)**(std::vector< std::vector< Eigen::Vector3d > > boundaries, Eigen::Vector3d query, size_t dom, size_t num)<br>Splits the mesh for FFCs by setting scalar and vector fields.  |
| double | **[getFFCValue](../Classes/classshapeworks_1_1Mesh.md#function-getffcvalue)**(Eigen::Vector3d query)<br>Gets values and gradients for FFCs.  |
| Eigen::Vector3d | **[getFFCGradient](../Classes/classshapeworks_1_1Mesh.md#function-getffcgradient)**(Eigen::Vector3d query) |
| MeshPoints | **[getIGLMesh](../Classes/classshapeworks_1_1Mesh.md#function-getiglmesh)**(Eigen::MatrixXd & V, Eigen::MatrixXi & F) const<br>Formats mesh into an IGL format.  |
| std::vector< std::string > | **[getSupportedTypes](../Classes/classshapeworks_1_1Mesh.md#function-getsupportedtypes)**()<br>getSupportedTypes  |

## Friends

|                | Name           |
| -------------- | -------------- |
| struct | **[SharedCommandData](../Classes/classshapeworks_1_1Mesh.md#friend-sharedcommanddata)**  |

## Detailed Description

```cpp
class shapeworks::Mesh;
```


This class encapsulates a [Mesh](../Classes/classshapeworks_1_1Mesh.md) and operations that can be performed on meshes 

## Public Types Documentation

### enum AlignmentType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Rigid | |   |
| Similarity | |   |
| Affine | |   |




### enum DistanceMethod

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| PointToPoint | |   |
| PointToCell | |   |




### enum CurvatureType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Principal | |   |
| Gaussian | |   |
| Mean | |   |




### enum SubdivisionType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Butterfly | |   |
| Loop | |   |




### using MeshType

```cpp
using shapeworks::Mesh::MeshType =  vtkSmartPointer<vtkPolyData>;
```


### using MeshPoints

```cpp
using shapeworks::Mesh::MeshPoints =  vtkSmartPointer<vtkPoints>;
```


## Public Functions Documentation

### function Mesh

```cpp
inline Mesh(
    const std::string & pathname
)
```


### function Mesh

```cpp
inline Mesh(
    MeshType meshPtr
)
```


### function Mesh

```cpp
inline Mesh(
    const Mesh & orig
)
```


### function Mesh

```cpp
inline Mesh(
    Mesh && orig
)
```


### function operator=

```cpp
inline Mesh & operator=(
    const Mesh & orig
)
```


### function operator=

```cpp
inline Mesh & operator=(
    Mesh && orig
)
```


### function operator+=

```cpp
Mesh & operator+=(
    const Mesh & otherMesh
)
```

append two meshes 

### function getVTKMesh

```cpp
inline MeshType getVTKMesh() const
```

return the current mesh 

### function write

```cpp
Mesh & write(
    const std::string & pathname,
    bool binaryFile =false
)
```

writes mesh, format specified by filename extension 

### function coverage

```cpp
Mesh & coverage(
    const Mesh & otherMesh,
    bool allowBackIntersections =true,
    double angleThreshold =0,
    double backSearchRadius =0
)
```

determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head) 

### function smooth

```cpp
Mesh & smooth(
    int iterations =0,
    double relaxation =0.0
)
```

applies laplacian smoothing 

### function smoothSinc

```cpp
Mesh & smoothSinc(
    int iterations =0,
    double passband =0.0
)
```

applies vtk windowed sinc smoothing 

### function remesh

```cpp
Mesh & remesh(
    int numVertices,
    double adaptivity =1.0
)
```

applies remeshing using approximated centroidal voronoi diagrams for a given number of vertices and adaptivity 

### function remeshPercent

```cpp
Mesh & remeshPercent(
    double percentage,
    double adaptivity =1.0
)
```

applies remeshing using approximated centroidal voronoi diagrams for a given percentage of vertices and adaptivity 

### function invertNormals

```cpp
Mesh & invertNormals()
```

handle flipping normals 

### function reflect

```cpp
Mesh & reflect(
    const Axis & axis,
    const Vector3 & origin =makeVector({ 0.0, 0.0, 0.0 })
)
```

reflect meshes with respect to a specified center and specific axis 

### function createTransform

```cpp
MeshTransform createTransform(
    const Mesh & target,
    AlignmentType align =Similarity,
    unsigned iterations =10
)
```

creates transform to target mesh using specified AlignmentType (Mesh::Rigid, Mesh::Similarity, Mesh::Affine) for specified number of iterations 

### function applyTransform

```cpp
Mesh & applyTransform(
    const MeshTransform transform
)
```

applies the given transformation to the mesh 

### function fillHoles

```cpp
Mesh & fillHoles()
```

finds holes in a mesh and closes them 

### function probeVolume

```cpp
Mesh & probeVolume(
    const Image & image
)
```

samples image data values at point locations specified by image 

### function clip

```cpp
Mesh & clip(
    const Plane plane
)
```

clips a mesh using a cutting plane 

### function translate

```cpp
Mesh & translate(
    const Vector3 & v
)
```

helper to translate mesh 

### function scale

```cpp
Mesh & scale(
    const Vector3 & v
)
```

helper to scale mesh 

### function boundingBox

```cpp
PhysicalRegion boundingBox() const
```

computes bounding box of current mesh 

### function fixElement

```cpp
Mesh & fixElement()
```

fix element winding of mesh 

### function distance

```cpp
Mesh & distance(
    const Mesh & target,
    const DistanceMethod method =PointToPoint
)
```

computes surface to surface distance, compute method: PointToPoint (default) or PointToCell 

### function clipClosedSurface

```cpp
Mesh & clipClosedSurface(
    const Plane plane
)
```

clips a mesh using a cutting plane resulting in a closed surface 

### function computeNormals

```cpp
Mesh & computeNormals()
```

computes and adds oriented point and cell normals 

### function closestPoint

```cpp
Point3 closestPoint(
    const Point3 point
)
```

returns closest point on a face in the mesh to the given point in space 

### function closestPointId

```cpp
int closestPointId(
    const Point3 point
)
```

returns closest point id in this mesh to the given point in space 

### function geodesicDistance

```cpp
double geodesicDistance(
    int source,
    int target
)
```

computes geodesic distance between two vertices (specified by their indices) on mesh 

### function geodesicDistance

```cpp
Field geodesicDistance(
    const Point3 landmark
)
```

computes geodesic distance between a point (landmark) and each vertex on mesh 

### function geodesicDistance

```cpp
Field geodesicDistance(
    const std::vector< Point3 > curve
)
```

computes geodesic distance between a set of points (curve) and each vertex on mesh 

### function curvature

```cpp
Field curvature(
    const CurvatureType type =Principal
)
```

computes and adds curvature (principal (default) or gaussian or mean) 

### function applySubdivisionFilter

```cpp
Mesh & applySubdivisionFilter(
    const SubdivisionType type =Butterfly,
    int subdivision =1
)
```

applies subdivision filter (butterfly (default) or loop) 

### function toImage

```cpp
Image toImage(
    PhysicalRegion region =PhysicalRegion(),
    Point spacing =Point({1., 1., 1.})
) const
```

rasterizes specified region to create binary image of desired dims (default: unit spacing) 

### function toDistanceTransform

```cpp
Image toDistanceTransform(
    PhysicalRegion region =PhysicalRegion(),
    Point spacing =Point({1., 1., 1.})
) const
```

converts specified region to distance transform image (default: unit spacing) 

### function center

```cpp
Point3 center() const
```

center of mesh 

### function centerOfMass

```cpp
Point3 centerOfMass() const
```

center of mass of mesh 

### function numPoints

```cpp
inline int numPoints() const
```

number of points 

### function numFaces

```cpp
inline int numFaces() const
```

number of faces 

### function points

```cpp
Eigen::MatrixXd points() const
```

matrix with number of points with (x,y,z) coordinates of each point 

### function faces

```cpp
Eigen::MatrixXi faces() const
```

matrix with number of faces with indices of the three points from which each face is composed 

### function getPoint

```cpp
Point3 getPoint(
    int id
) const
```

(x,y,z) coordinates of vertex at given index 

### function getFace

```cpp
IPoint3 getFace(
    int id
) const
```

return indices of the three points with which the face at the given index is composed 

### function getFieldNames

```cpp
std::vector< std::string > getFieldNames() const
```

print all field names in mesh 

### function setField

```cpp
Mesh & setField(
    std::string name,
    Array array
)
```

sets the given field for points with array (*does not copy array's values) 

### function setFieldForFaces

```cpp
Mesh & setFieldForFaces(
    std::string name,
    Array array
)
```

sets the given field for faces with array (*does not copy array's values) 

### function getField

```cpp
template <typename T >
inline vtkSmartPointer< T > getField(
    const std::string & name
) const
```

gets a pointer to the requested field, null if field doesn't exist 

### function setFieldValue

```cpp
void setFieldValue(
    const std::string & name,
    int idx,
    double value
)
```

sets the given index of field to value 

### function getFieldValue

```cpp
double getFieldValue(
    const std::string & name,
    int idx
) const
```

gets the value at the given index of field (NOTE: returns first component of vector fields) 

### function getMultiFieldValue

```cpp
Eigen::VectorXd getMultiFieldValue(
    const std::string & name,
    int idx
) const
```

gets the multi value at the given index of field 

### function getFieldRange

```cpp
std::vector< double > getFieldRange(
    const std::string & name
) const
```

returns the range of the given field (NOTE: returns range of first component of vector fields) 

### function getFieldMean

```cpp
double getFieldMean(
    const std::string & name
) const
```

returns the mean the given field 

### function getFieldStd

```cpp
double getFieldStd(
    const std::string & name
) const
```

returns the standard deviation of the given field 

### function compareAllPoints

```cpp
bool compareAllPoints(
    const Mesh & other_mesh
) const
```

compare if values of the points in two (corresponding) meshes are (eps)equal 

### function compareAllFaces

```cpp
bool compareAllFaces(
    const Mesh & other_mesh
) const
```

compare if face indices in two (corresponding) meshes are equal 

### function compareAllFields

```cpp
bool compareAllFields(
    const Mesh & other_mesh,
    const double eps =-1.0
) const
```

compare if all fields in two meshes are (eps)equal 

### function compareField

```cpp
bool compareField(
    const Mesh & other_mesh,
    const std::string & name1,
    const std::string & name2 ="",
    const double eps =-1.0
) const
```

compare field of meshes to be (eps)equal (same field for both if only one specified) 

### function compare

```cpp
bool compare(
    const Mesh & other_mesh,
    const double eps =-1.0
) const
```

compare meshes 

### function operator==

```cpp
inline bool operator==(
    const Mesh & other
) const
```

compare meshes 

### function splitMesh

```cpp
bool splitMesh(
    std::vector< std::vector< Eigen::Vector3d > > boundaries,
    Eigen::Vector3d query,
    size_t dom,
    size_t num
)
```

Splits the mesh for FFCs by setting scalar and vector fields. 

### function getFFCValue

```cpp
double getFFCValue(
    Eigen::Vector3d query
)
```

Gets values and gradients for FFCs. 

### function getFFCGradient

```cpp
Eigen::Vector3d getFFCGradient(
    Eigen::Vector3d query
)
```


### function getIGLMesh

```cpp
MeshPoints getIGLMesh(
    Eigen::MatrixXd & V,
    Eigen::MatrixXi & F
) const
```

Formats mesh into an IGL format. 

### function getSupportedTypes

```cpp
static inline std::vector< std::string > getSupportedTypes()
```

getSupportedTypes 

## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData();
```


-------------------------------

Updated on 2022-01-14 at 02:26:01 +0000