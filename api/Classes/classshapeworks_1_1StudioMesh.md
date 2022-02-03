---
title: shapeworks::StudioMesh
summary: Representation of a single mesh. 

---

# shapeworks::StudioMesh



Representation of a single mesh.  [More...](#detailed-description)


`#include <StudioMesh.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StudioMesh](../Classes/classshapeworks_1_1StudioMesh.md#function-studiomesh)**()<br>Constructor.  |
| | **[~StudioMesh](../Classes/classshapeworks_1_1StudioMesh.md#function-~studiomesh)**()<br>Destructor.  |
| QString | **[get_dimension_string](../Classes/classshapeworks_1_1StudioMesh.md#function-get-dimension-string)**()<br>Get the dimensions as a string for display (if loaded from an image)  |
| vtkSmartPointer< vtkPolyData > | **[get_poly_data](../Classes/classshapeworks_1_1StudioMesh.md#function-get-poly-data)**()<br>Get the mesh polydata.  |
| vnl_vector< double > | **[get_center_transform](../Classes/classshapeworks_1_1StudioMesh.md#function-get-center-transform)**()<br>Get the center transform.  |
| void | **[set_poly_data](../Classes/classshapeworks_1_1StudioMesh.md#function-set-poly-data)**(vtkSmartPointer< vtkPolyData > poly_data)<br>Set the poly data directly.  |
| void | **[set_error_message](../Classes/classshapeworks_1_1StudioMesh.md#function-set-error-message)**(std::string error_message)<br>Set the error message.  |
| std::string | **[get_error_message](../Classes/classshapeworks_1_1StudioMesh.md#function-get-error-message)**()<br>Return the error message.  |
| void | **[apply_feature_map](../Classes/classshapeworks_1_1StudioMesh.md#function-apply-feature-map)**(std::string name, ImageType::Pointer image)<br>Apply a feature map.  |
| void | **[apply_scalars](../Classes/classshapeworks_1_1StudioMesh.md#function-apply-scalars)**(MeshHandle mesh)<br>Apply scalars from another mesh, with a transform.  |
| void | **[interpolate_scalars_to_mesh](../Classes/classshapeworks_1_1StudioMesh.md#function-interpolate-scalars-to-mesh)**(std::string name, Eigen::VectorXd positions, Eigen::VectorXf scalar_values)<br>Interpolation scalars at positions to this mesh.  |

## Detailed Description

```cpp
class shapeworks::StudioMesh;
```

Representation of a single mesh. 

The [Mesh](../Classes/classshapeworks_1_1Mesh.md) class represents a single mesh generated from an image file or set of particles. It is responsible for loading the image and generating a mesh from it. 

## Public Functions Documentation

### function StudioMesh

```cpp
StudioMesh()
```

Constructor. 

### function ~StudioMesh

```cpp
~StudioMesh()
```

Destructor. 

### function get_dimension_string

```cpp
QString get_dimension_string()
```

Get the dimensions as a string for display (if loaded from an image) 

### function get_poly_data

```cpp
vtkSmartPointer< vtkPolyData > get_poly_data()
```

Get the mesh polydata. 

### function get_center_transform

```cpp
vnl_vector< double > get_center_transform()
```

Get the center transform. 

### function set_poly_data

```cpp
void set_poly_data(
    vtkSmartPointer< vtkPolyData > poly_data
)
```

Set the poly data directly. 

### function set_error_message

```cpp
void set_error_message(
    std::string error_message
)
```

Set the error message. 

### function get_error_message

```cpp
std::string get_error_message()
```

Return the error message. 

### function apply_feature_map

```cpp
void apply_feature_map(
    std::string name,
    ImageType::Pointer image
)
```

Apply a feature map. 

### function apply_scalars

```cpp
void apply_scalars(
    MeshHandle mesh
)
```

Apply scalars from another mesh, with a transform. 

### function interpolate_scalars_to_mesh

```cpp
void interpolate_scalars_to_mesh(
    std::string name,
    Eigen::VectorXd positions,
    Eigen::VectorXf scalar_values
)
```

Interpolation scalars at positions to this mesh. 

-------------------------------

Updated on 2022-02-03 at 23:25:52 +0000