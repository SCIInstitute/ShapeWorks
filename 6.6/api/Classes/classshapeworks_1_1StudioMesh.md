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
| vtkSmartPointer< vtkPolyData > | **[get_poly_data](../Classes/classshapeworks_1_1StudioMesh.md#function-get-poly-data)**()<br>Get the mesh polydata.  |
| void | **[set_poly_data](../Classes/classshapeworks_1_1StudioMesh.md#function-set-poly-data)**(vtkSmartPointer< vtkPolyData > poly_data)<br>Set the poly data directly.  |
| void | **[set_error_message](../Classes/classshapeworks_1_1StudioMesh.md#function-set-error-message)**(std::string error_message)<br>Set the error message.  |
| std::string | **[get_error_message](../Classes/classshapeworks_1_1StudioMesh.md#function-get-error-message)**()<br>Return the error message.  |
| void | **[apply_feature_map](../Classes/classshapeworks_1_1StudioMesh.md#function-apply-feature-map)**(std::string name, ImageType::Pointer image)<br>Apply a feature map.  |
| void | **[apply_scalars](../Classes/classshapeworks_1_1StudioMesh.md#function-apply-scalars)**(MeshHandle mesh)<br>Apply scalars from another mesh, with a transform.  |
| void | **[interpolate_scalars_to_mesh](../Classes/classshapeworks_1_1StudioMesh.md#function-interpolate-scalars-to-mesh)**(std::string name, Eigen::VectorXd positions, Eigen::VectorXd scalar_values)<br>Interpolation scalars at positions to this mesh.  |
| double | **[get_largest_dimension_size](../Classes/classshapeworks_1_1StudioMesh.md#function-get-largest-dimension-size)**()<br>Return the range of largest axis (e.g. 200 for an object that sits in 100x200x100)  |
| vtkFloatArray * | **[get_or_create_array](../Classes/classshapeworks_1_1StudioMesh.md#function-get-or-create-array)**(std::string name, float default_value)<br>Get or create and return an array with a given name.  |
| void | **[paint_ffc](../Classes/classshapeworks_1_1StudioMesh.md#function-paint-ffc)**(double world_pos[], double radius, bool inclusive)<br>Paint free form constraint.  |
| bool | **[has_ffc_paint](../Classes/classshapeworks_1_1StudioMesh.md#function-has-ffc-paint)**()<br>Does this mesh have free form constraint paint?  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[get_sw_mesh](../Classes/classshapeworks_1_1StudioMesh.md#function-get-sw-mesh)**()<br>Return a shapeworks [Mesh](../Classes/classshapeworks_1_1Mesh.md) library class mesh.  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr const char *const | **[FFC_PAINT](../Classes/classshapeworks_1_1StudioMesh.md#variable-ffc-paint)**  |

## Detailed Description

```cpp
class shapeworks::StudioMesh;
```

Representation of a single mesh. 

The [Mesh](../Classes/classshapeworks_1_1Mesh.md) class represents a single mesh generated from an image file or set of particles. It is responsible for loading the image and generating a mesh from it.

TODO: Merge this functionality with Libs/Mesh/Mesh class 

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

### function get_poly_data

```cpp
vtkSmartPointer< vtkPolyData > get_poly_data()
```

Get the mesh polydata. 

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
    Eigen::VectorXd scalar_values
)
```

Interpolation scalars at positions to this mesh. 

### function get_largest_dimension_size

```cpp
double get_largest_dimension_size()
```

Return the range of largest axis (e.g. 200 for an object that sits in 100x200x100) 

### function get_or_create_array

```cpp
vtkFloatArray * get_or_create_array(
    std::string name,
    float default_value
)
```

Get or create and return an array with a given name. 

### function paint_ffc

```cpp
void paint_ffc(
    double world_pos[],
    double radius,
    bool inclusive
)
```

Paint free form constraint. 

### function has_ffc_paint

```cpp
bool has_ffc_paint()
```

Does this mesh have free form constraint paint? 

### function get_sw_mesh

```cpp
Mesh get_sw_mesh()
```

Return a shapeworks [Mesh](../Classes/classshapeworks_1_1Mesh.md) library class mesh. 

## Public Attributes Documentation

### variable FFC_PAINT

```cpp
static constexpr const char *const FFC_PAINT = "ffc_paint";
```


-------------------------------

Updated on 2024-09-10 at 03:33:07 +0000