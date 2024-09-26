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
| enum| **[FieldType](../Classes/classshapeworks_1_1Mesh.md#enum-fieldtype)** { Point, Face} |
| enum| **[AlignmentType](../Classes/classshapeworks_1_1Mesh.md#enum-alignmenttype)** { Rigid, Similarity, Affine} |
| enum| **[DistanceMethod](../Classes/classshapeworks_1_1Mesh.md#enum-distancemethod)** { PointToPoint, PointToCell} |
| enum| **[CurvatureType](../Classes/classshapeworks_1_1Mesh.md#enum-curvaturetype)** { Principal, Gaussian, Mean} |
| enum| **[SubdivisionType](../Classes/classshapeworks_1_1Mesh.md#enum-subdivisiontype)** { Butterfly, Loop} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(const std::string & pathname) |
| void | **[set_id](../Classes/classshapeworks_1_1Mesh.md#function-set-id)**(int id) |
| int | **[get_id](../Classes/classshapeworks_1_1Mesh.md#function-get-id)**() const |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(vtkSmartPointer< vtkPolyData > meshPtr) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & orig) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) && orig) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator=](../Classes/classshapeworks_1_1Mesh.md#function-operator=)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & orig) |
| | **[Mesh](../Classes/classshapeworks_1_1Mesh.md#function-mesh)**(const Eigen::MatrixXd & points, const Eigen::MatrixXi & faces) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator=](../Classes/classshapeworks_1_1Mesh.md#function-operator=)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) && orig) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[operator+=](../Classes/classshapeworks_1_1Mesh.md#function-operator+=)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & otherMesh)<br>append two meshes  |
| vtkSmartPointer< vtkPolyData > | **[getVTKMesh](../Classes/classshapeworks_1_1Mesh.md#function-getvtkmesh)**() const<br>return the current mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[write](../Classes/classshapeworks_1_1Mesh.md#function-write)**(const std::string & pathname, bool binaryFile =false)<br>writes mesh, format specified by filename extension  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[coverage](../Classes/classshapeworks_1_1Mesh.md#function-coverage)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & otherMesh, bool allowBackIntersections =true, double angleThreshold =0, double backSearchRadius =0)<br>determines coverage between current mesh and another mesh (e.g. acetabular cup / femoral head)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[smooth](../Classes/classshapeworks_1_1Mesh.md#function-smooth)**(int iterations =0, double relaxation =0.0)<br>applies laplacian smoothing  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[smoothSinc](../Classes/classshapeworks_1_1Mesh.md#function-smoothsinc)**(int iterations =0, double passband =0.0)<br>applies vtk windowed sinc smoothing  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[remesh](../Classes/classshapeworks_1_1Mesh.md#function-remesh)**(int numVertices, double adaptivity =1.0)<br>applies remeshing using approximated centroidal voronoi diagrams for a given number of vertices and adaptivity  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[remeshPercent](../Classes/classshapeworks_1_1Mesh.md#function-remeshpercent)**(double percentage, double adaptivity =1.0)<br>applies remeshing using approximated centroidal voronoi diagrams for a given percentage of vertices and adaptivity  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[invertNormals](../Classes/classshapeworks_1_1Mesh.md#function-invertnormals)**()<br>handle flipping normals  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[reflect](../Classes/classshapeworks_1_1Mesh.md#function-reflect)**(const [Axis](../Namespaces/namespaceshapeworks.md#enum-axis) & axis, const Vector3 & origin =[makeVector](../Namespaces/namespaceshapeworks.md#function-makevector)({0.0, 0.0, 0.0}))<br>reflect meshes with respect to a specified center and specific axis  |
| [MeshTransform](../Namespaces/namespaceshapeworks.md#using-meshtransform) | **[createTransform](../Classes/classshapeworks_1_1Mesh.md#function-createtransform)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & target, AlignmentType align =Similarity, unsigned iterations =10) |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[applyTransform](../Classes/classshapeworks_1_1Mesh.md#function-applytransform)**(const [MeshTransform](../Namespaces/namespaceshapeworks.md#using-meshtransform) transform)<br>applies the given transformation to the mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[rotate](../Classes/classshapeworks_1_1Mesh.md#function-rotate)**(const double angle, const [Axis](../Namespaces/namespaceshapeworks.md#enum-axis) axis)<br>applies the given rotation to the given axis  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[fillHoles](../Classes/classshapeworks_1_1Mesh.md#function-fillholes)**(double hole_size =1000.0)<br>finds holes in a mesh and closes them  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[clean](../Classes/classshapeworks_1_1Mesh.md#function-clean)**()<br>clean mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[probeVolume](../Classes/classshapeworks_1_1Mesh.md#function-probevolume)**(const [Image](../Classes/classshapeworks_1_1Image.md) & image)<br>samples image data values at point locations specified by image  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[clip](../Classes/classshapeworks_1_1Mesh.md#function-clip)**(const Plane plane)<br>clips a mesh using a cutting plane  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[translate](../Classes/classshapeworks_1_1Mesh.md#function-translate)**(const Vector3 & v)<br>helper to translate mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[scale](../Classes/classshapeworks_1_1Mesh.md#function-scale)**(const Vector3 & v)<br>helper to scale mesh  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1Mesh.md#function-boundingbox)**() const<br>computes bounding box of current mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[fixElement](../Classes/classshapeworks_1_1Mesh.md#function-fixelement)**()<br>fix element winding of mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[fixNonManifold](../Classes/classshapeworks_1_1Mesh.md#function-fixnonmanifold)**()<br>Attempt to fix non-manifold edges.  |
| bool | **[detectNonManifold](../Classes/classshapeworks_1_1Mesh.md#function-detectnonmanifold)**()<br>Detect if mesh contain non-manifold edges.  |
| bool | **[detectTriangular](../Classes/classshapeworks_1_1Mesh.md#function-detecttriangular)**()<br>Detect if mesh is triangular;.  |
| std::vector< Field > | **[distance](../Classes/classshapeworks_1_1Mesh.md#function-distance)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & target, const DistanceMethod method =PointToCell) const |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[clipClosedSurface](../Classes/classshapeworks_1_1Mesh.md#function-clipclosedsurface)**(const Plane plane)<br>clips a mesh using a cutting plane resulting in a closed surface  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[computeNormals](../Classes/classshapeworks_1_1Mesh.md#function-computenormals)**()<br>computes and adds oriented point and cell normals  |
| Point3 | **[closestPoint](../Classes/classshapeworks_1_1Mesh.md#function-closestpoint)**(const Point3 point, double & distance, vtkIdType & face_id) const |
| int | **[closestPointId](../Classes/classshapeworks_1_1Mesh.md#function-closestpointid)**(const Point3 point) const<br>returns closest point id in this mesh to the given point in space  |
| bool | **[isPointInside](../Classes/classshapeworks_1_1Mesh.md#function-ispointinside)**(const Point3 point) const<br>returns if the given point is inside the mesh  |
| double | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(int source, int target) const<br>computes geodesic distance between two vertices (specified by their indices) on mesh  |
| Field | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(const Point3 landmark) const<br>computes geodesic distance between a point (landmark) and each vertex on mesh  |
| Field | **[geodesicDistance](../Classes/classshapeworks_1_1Mesh.md#function-geodesicdistance)**(const std::vector< Point3 > curve) const<br>computes geodesic distance between a set of points (curve) and each vertex on mesh  |
| Field | **[curvature](../Classes/classshapeworks_1_1Mesh.md#function-curvature)**(const CurvatureType type =Principal) const<br>computes curvature using principal (default) or gaussian or mean algorithms  |
| void | **[computeFieldGradient](../Classes/classshapeworks_1_1Mesh.md#function-computefieldgradient)**(const std::string & field) const<br>compute the gradient of a scalar field for all vertices  |
| Eigen::Vector3d | **[computeFieldGradientAtPoint](../Classes/classshapeworks_1_1Mesh.md#function-computefieldgradientatpoint)**(const std::string & field, const Point3 & query) const<br>compute the gradient of a scalar field at a point  |
| double | **[interpolateFieldAtPoint](../Classes/classshapeworks_1_1Mesh.md#function-interpolatefieldatpoint)**(const std::string & field, const Point3 & query) const<br>interpolate a scalar field at a given point  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[applySubdivisionFilter](../Classes/classshapeworks_1_1Mesh.md#function-applysubdivisionfilter)**(const SubdivisionType type =Butterfly, int subdivision =1)<br>applies subdivision filter (butterfly (default) or loop)  |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[toImage](../Classes/classshapeworks_1_1Mesh.md#function-toimage)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region =[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md)(), Point3 spacing =Point3({1., 1., 1.})) const<br>rasterizes specified region to create binary image of desired dims (default: unit spacing)  |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[toDistanceTransform](../Classes/classshapeworks_1_1Mesh.md#function-todistancetransform)**([PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) region =[PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md)(), const Point3 spacing =Point3({1., 1., 1.}), const Dims padding =Dims({1, 1, 1})) const<br>converts specified region to distance transform image (default: unit spacing) with (logical) padding  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[computeThickness](../Classes/classshapeworks_1_1Mesh.md#function-computethickness)**([Image](../Classes/classshapeworks_1_1Image.md) & image, [Image](../Classes/classshapeworks_1_1Image.md) * dt =nullptr, double max_dist =10000, double median_radius =5.0, std::string distance_mesh ="")<br>assign cortical thickness values from mesh points  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[computeLandmarkGeodesics](../Classes/classshapeworks_1_1Mesh.md#function-computelandmarkgeodesics)**(const std::vector< Point3 > & landmarks)<br>compute geodesic distances to landmarks and assign as fields  |
| Point3 | **[center](../Classes/classshapeworks_1_1Mesh.md#function-center)**() const<br>center of mesh  |
| Point3 | **[centerOfMass](../Classes/classshapeworks_1_1Mesh.md#function-centerofmass)**() const<br>center of mass of mesh  |
| int | **[numPoints](../Classes/classshapeworks_1_1Mesh.md#function-numpoints)**() const<br>number of points  |
| int | **[numFaces](../Classes/classshapeworks_1_1Mesh.md#function-numfaces)**() const<br>number of faces  |
| Eigen::MatrixXd | **[points](../Classes/classshapeworks_1_1Mesh.md#function-points)**() const<br>matrix with number of points with (x,y,z) coordinates of each point  |
| Eigen::MatrixXi | **[faces](../Classes/classshapeworks_1_1Mesh.md#function-faces)**() const<br>matrix with number of faces with indices of the three points from which each face is composed  |
| Point3 | **[getPoint](../Classes/classshapeworks_1_1Mesh.md#function-getpoint)**(int id) const<br>(x,y,z) coordinates of vertex at given index  |
| IPoint3 | **[getFace](../Classes/classshapeworks_1_1Mesh.md#function-getface)**(int id) const<br>return indices of the three points with which the face at the given index is composed  |
| std::vector< std::string > | **[getFieldNames](../Classes/classshapeworks_1_1Mesh.md#function-getfieldnames)**() const<br>print all field names in mesh  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) & | **[setField](../Classes/classshapeworks_1_1Mesh.md#function-setfield)**(const std::string name, Array array, const FieldType type)<br>sets the given field for points or faces with array (*does not copy array's values)  |
| Field | **[getField](../Classes/classshapeworks_1_1Mesh.md#function-getfield)**(const std::string & name, const FieldType type) const<br>gets a pointer to the requested field of points or faces, null if field doesn't exist  |
| void | **[setFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-setfieldvalue)**(const std::string & name, int idx, double value)<br>sets the given index of field to value  |
| double | **[getFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-getfieldvalue)**(const std::string & name, int idx) const<br>gets the value at the given index of field (NOTE: returns first component of vector fields)  |
| Eigen::VectorXd | **[getMultiFieldValue](../Classes/classshapeworks_1_1Mesh.md#function-getmultifieldvalue)**(const std::string & name, int idx) const<br>gets the multi value at the given index of [vertex] field  |
| bool | **[compareAllPoints](../Classes/classshapeworks_1_1Mesh.md#function-compareallpoints)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh) const<br>compare if values of the points in two (corresponding) meshes are (eps)equal  |
| bool | **[compareAllFaces](../Classes/classshapeworks_1_1Mesh.md#function-compareallfaces)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh) const<br>compare if face indices in two (corresponding) meshes are equal  |
| bool | **[compareAllFields](../Classes/classshapeworks_1_1Mesh.md#function-compareallfields)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const double eps =1e4) const<br>compare if all fields in two meshes are (eps)equal  |
| bool | **[compareField](../Classes/classshapeworks_1_1Mesh.md#function-comparefield)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const std::string & name1, const std::string & name2 ="", const double eps =-1.0) const<br>compare field of meshes to be (eps)equal (same field for both if only one specified)  |
| bool | **[compare](../Classes/classshapeworks_1_1Mesh.md#function-compare)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other_mesh, const double eps =-1.0) const<br>compare meshes  |
| bool | **[operator==](../Classes/classshapeworks_1_1Mesh.md#function-operator==)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & other) const<br>compare meshes  |
| double | **[getFFCValue](../Classes/classshapeworks_1_1Mesh.md#function-getffcvalue)**(Eigen::Vector3d query) const<br>Gets values for FFCs.  |
| Eigen::Vector3d | **[getFFCGradient](../Classes/classshapeworks_1_1Mesh.md#function-getffcgradient)**(Eigen::Vector3d query) const<br>Gets gradients for FFCs.  |
| vtkSmartPointer< vtkPoints > | **[getIGLMesh](../Classes/classshapeworks_1_1Mesh.md#function-getiglmesh)**(Eigen::MatrixXd & V, Eigen::MatrixXi & F) const<br>Formats mesh into an IGL format.  |
| vtkSmartPointer< vtkPolyData > | **[clipByField](../Classes/classshapeworks_1_1Mesh.md#function-clipbyfield)**(const std::string & name, double value)<br>Clips the mesh according to a field value.  |
| vtkSmartPointer< vtkStaticCellLocator > | **[getCellLocator](../Classes/classshapeworks_1_1Mesh.md#function-getcelllocator)**() const<br>Returns the cell locator.  |
| int | **[getClosestFace](../Classes/classshapeworks_1_1Mesh.md#function-getclosestface)**(const Point3 & point) const |
| Eigen::Vector3d | **[computeBarycentricCoordinates](../Classes/classshapeworks_1_1Mesh.md#function-computebarycentriccoordinates)**(const Eigen::Vector3d & pt, int face) const<br>Computes baricentric coordinates given a query point and a face number.  |
| void | **[interpolate_scalars_to_mesh](../Classes/classshapeworks_1_1Mesh.md#function-interpolate-scalars-to-mesh)**(std::string name, Eigen::VectorXd positions, Eigen::VectorXd scalar_values) |
| std::vector< std::string > | **[getSupportedTypes](../Classes/classshapeworks_1_1Mesh.md#function-getsupportedtypes)**()<br>Return supported file types.  |

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

### enum FieldType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Point | |   |
| Face | |   |




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




## Public Functions Documentation

### function Mesh

```cpp
Mesh(
    const std::string & pathname
)
```


### function set_id

```cpp
inline void set_id(
    int id
)
```


### function get_id

```cpp
inline int get_id() const
```


### function Mesh

```cpp
inline Mesh(
    vtkSmartPointer< vtkPolyData > meshPtr
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


### function Mesh

```cpp
Mesh(
    const Eigen::MatrixXd & points,
    const Eigen::MatrixXi & faces
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
inline vtkSmartPointer< vtkPolyData > getVTKMesh() const
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
    const Vector3 & origin =makeVector({0.0, 0.0, 0.0})
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

### function rotate

```cpp
Mesh & rotate(
    const double angle,
    const Axis axis
)
```

applies the given rotation to the given axis 

### function fillHoles

```cpp
Mesh & fillHoles(
    double hole_size =1000.0
)
```

finds holes in a mesh and closes them 

### function clean

```cpp
Mesh & clean()
```

clean mesh 

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

### function fixNonManifold

```cpp
Mesh & fixNonManifold()
```

Attempt to fix non-manifold edges. 

### function detectNonManifold

```cpp
bool detectNonManifold()
```

Detect if mesh contain non-manifold edges. 

### function detectTriangular

```cpp
bool detectTriangular()
```

Detect if mesh is triangular;. 

### function distance

```cpp
std::vector< Field > distance(
    const Mesh & target,
    const DistanceMethod method =PointToCell
) const
```


Computes distance from each vertex to closest cell or point in target mesh, specified as PointToCell (default) or PointToPoint. Returns Fields containing distance to target and ids of the associated cells or points. 


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
    const Point3 point,
    double & distance,
    vtkIdType & face_id
) const
```


Returns closest point on this mesh to the given point in space. In addition, returns by reference:

* the distance of the point in space from this mesh
* the face_id containing the closest point 


### function closestPointId

```cpp
int closestPointId(
    const Point3 point
) const
```

returns closest point id in this mesh to the given point in space 

### function isPointInside

```cpp
bool isPointInside(
    const Point3 point
) const
```

returns if the given point is inside the mesh 

### function geodesicDistance

```cpp
double geodesicDistance(
    int source,
    int target
) const
```

computes geodesic distance between two vertices (specified by their indices) on mesh 

### function geodesicDistance

```cpp
Field geodesicDistance(
    const Point3 landmark
) const
```

computes geodesic distance between a point (landmark) and each vertex on mesh 

### function geodesicDistance

```cpp
Field geodesicDistance(
    const std::vector< Point3 > curve
) const
```

computes geodesic distance between a set of points (curve) and each vertex on mesh 

### function curvature

```cpp
Field curvature(
    const CurvatureType type =Principal
) const
```

computes curvature using principal (default) or gaussian or mean algorithms 

### function computeFieldGradient

```cpp
void computeFieldGradient(
    const std::string & field
) const
```

compute the gradient of a scalar field for all vertices 

### function computeFieldGradientAtPoint

```cpp
Eigen::Vector3d computeFieldGradientAtPoint(
    const std::string & field,
    const Point3 & query
) const
```

compute the gradient of a scalar field at a point 

### function interpolateFieldAtPoint

```cpp
double interpolateFieldAtPoint(
    const std::string & field,
    const Point3 & query
) const
```

interpolate a scalar field at a given point 

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
    Point3 spacing =Point3({1., 1., 1.})
) const
```

rasterizes specified region to create binary image of desired dims (default: unit spacing) 

### function toDistanceTransform

```cpp
Image toDistanceTransform(
    PhysicalRegion region =PhysicalRegion(),
    const Point3 spacing =Point3({1., 1., 1.}),
    const Dims padding =Dims({1, 1, 1})
) const
```

converts specified region to distance transform image (default: unit spacing) with (logical) padding 

### function computeThickness

```cpp
Mesh & computeThickness(
    Image & image,
    Image * dt =nullptr,
    double max_dist =10000,
    double median_radius =5.0,
    std::string distance_mesh =""
)
```

assign cortical thickness values from mesh points 

### function computeLandmarkGeodesics

```cpp
Mesh & computeLandmarkGeodesics(
    const std::vector< Point3 > & landmarks
)
```

compute geodesic distances to landmarks and assign as fields 

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
    const std::string name,
    Array array,
    const FieldType type
)
```

sets the given field for points or faces with array (*does not copy array's values) 

### function getField

```cpp
Field getField(
    const std::string & name,
    const FieldType type
) const
```

gets a pointer to the requested field of points or faces, null if field doesn't exist 

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

gets the multi value at the given index of [vertex] field 

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
    const double eps =1e4
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

### function getFFCValue

```cpp
double getFFCValue(
    Eigen::Vector3d query
) const
```

Gets values for FFCs. 

### function getFFCGradient

```cpp
Eigen::Vector3d getFFCGradient(
    Eigen::Vector3d query
) const
```

Gets gradients for FFCs. 

### function getIGLMesh

```cpp
vtkSmartPointer< vtkPoints > getIGLMesh(
    Eigen::MatrixXd & V,
    Eigen::MatrixXi & F
) const
```

Formats mesh into an IGL format. 

### function clipByField

```cpp
vtkSmartPointer< vtkPolyData > clipByField(
    const std::string & name,
    double value
)
```

Clips the mesh according to a field value. 

### function getCellLocator

```cpp
inline vtkSmartPointer< vtkStaticCellLocator > getCellLocator() const
```

Returns the cell locator. 

### function getClosestFace

```cpp
int getClosestFace(
    const Point3 & point
) const
```


### function computeBarycentricCoordinates

```cpp
Eigen::Vector3d computeBarycentricCoordinates(
    const Eigen::Vector3d & pt,
    int face
) const
```

Computes baricentric coordinates given a query point and a face number. 

### function interpolate_scalars_to_mesh

```cpp
void interpolate_scalars_to_mesh(
    std::string name,
    Eigen::VectorXd positions,
    Eigen::VectorXd scalar_values
)
```


Interpolates scalar values at points (e.g. correspondence points) to the mesh, assign/create a field with the given name 


### function getSupportedTypes

```cpp
static inline std::vector< std::string > getSupportedTypes()
```

Return supported file types. 

## Friends

### friend SharedCommandData

```cpp
friend struct SharedCommandData(
    SharedCommandData 
);
```


-------------------------------

Updated on 2024-09-26 at 21:49:46 +0000