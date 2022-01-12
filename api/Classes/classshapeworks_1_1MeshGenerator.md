---
title: shapeworks::MeshGenerator

---

# shapeworks::MeshGenerator





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MeshGenerator](../Classes/classshapeworks_1_1MeshGenerator.md#function-meshgenerator)**() |
| | **[~MeshGenerator](../Classes/classshapeworks_1_1MeshGenerator.md#function-~meshgenerator)**() |
| MeshHandle | **[build_mesh](../Classes/classshapeworks_1_1MeshGenerator.md#function-build-mesh)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & item) |
| MeshHandle | **[build_mesh_from_points](../Classes/classshapeworks_1_1MeshGenerator.md#function-build-mesh-from-points)**(const vnl_vector< double > & shape, int domain) |
| MeshHandle | **[build_mesh_from_image](../Classes/classshapeworks_1_1MeshGenerator.md#function-build-mesh-from-image)**(ImageType::Pointer image, float iso_value =0.0001) |
| MeshHandle | **[build_mesh_from_file](../Classes/classshapeworks_1_1MeshGenerator.md#function-build-mesh-from-file)**(std::string filename, float iso_value =0.0001) |
| void | **[set_mesh_reconstructors](../Classes/classshapeworks_1_1MeshGenerator.md#function-set-mesh-reconstructors)**(std::shared_ptr< [MeshReconstructors](../Classes/classshapeworks_1_1MeshReconstructors.md) > reconstructors) |
| void | **[set_reconstruction_method](../Classes/classshapeworks_1_1MeshGenerator.md#function-set-reconstruction-method)**(std::string method) |
| std::string | **[get_reconstruction_method](../Classes/classshapeworks_1_1MeshGenerator.md#function-get-reconstruction-method)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[RECONSTRUCTION_LEGACY_C](../Classes/classshapeworks_1_1MeshGenerator.md#variable-reconstruction-legacy-c)**  |
| const std::string | **[RECONSTRUCTION_DISTANCE_TRANSFORM_C](../Classes/classshapeworks_1_1MeshGenerator.md#variable-reconstruction-distance-transform-c)**  |
| const std::string | **[RECONSTRUCTION_MESH_WARPER_C](../Classes/classshapeworks_1_1MeshGenerator.md#variable-reconstruction-mesh-warper-c)**  |

## Public Functions Documentation

### function MeshGenerator

```cpp
MeshGenerator()
```


### function ~MeshGenerator

```cpp
~MeshGenerator()
```


### function build_mesh

```cpp
MeshHandle build_mesh(
    const MeshWorkItem & item
)
```


### function build_mesh_from_points

```cpp
MeshHandle build_mesh_from_points(
    const vnl_vector< double > & shape,
    int domain
)
```


### function build_mesh_from_image

```cpp
MeshHandle build_mesh_from_image(
    ImageType::Pointer image,
    float iso_value =0.0001
)
```


### function build_mesh_from_file

```cpp
MeshHandle build_mesh_from_file(
    std::string filename,
    float iso_value =0.0001
)
```


### function set_mesh_reconstructors

```cpp
void set_mesh_reconstructors(
    std::shared_ptr< MeshReconstructors > reconstructors
)
```


### function set_reconstruction_method

```cpp
void set_reconstruction_method(
    std::string method
)
```


### function get_reconstruction_method

```cpp
std::string get_reconstruction_method()
```


## Public Attributes Documentation

### variable RECONSTRUCTION_LEGACY_C

```cpp
static const std::string RECONSTRUCTION_LEGACY_C;
```


### variable RECONSTRUCTION_DISTANCE_TRANSFORM_C

```cpp
static const std::string RECONSTRUCTION_DISTANCE_TRANSFORM_C;
```


### variable RECONSTRUCTION_MESH_WARPER_C

```cpp
static const std::string RECONSTRUCTION_MESH_WARPER_C;
```


-------------------------------

Updated on 2022-01-12 at 19:06:13 +0000