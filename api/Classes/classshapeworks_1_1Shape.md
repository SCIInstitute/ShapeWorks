---
title: shapeworks::Shape
summary: Representation of a single shape/patient/subject. 

---

# shapeworks::Shape



Representation of a single shape/patient/subject. 


`#include <Shape.h>`

## Public Classes

|                | Name           |
| -------------- | -------------- |
| class | **[Point](../Classes/classshapeworks_1_1Shape_1_1Point.md)** <br>TODO: replace this wherever it is used.  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Shape](../Classes/classshapeworks_1_1Shape.md#function-shape)**() |
| | **[~Shape](../Classes/classshapeworks_1_1Shape.md#function-~shape)**() |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-meshes)**(const string & display_mode) |
| void | **[set_annotations](../Classes/classshapeworks_1_1Shape.md#function-set-annotations)**(QStringList annotations, bool only_overwrite_blank =true) |
| QStringList | **[get_annotations](../Classes/classshapeworks_1_1Shape.md#function-get-annotations)**() |
| void | **[set_mesh_manager](../Classes/classshapeworks_1_1Shape.md#function-set-mesh-manager)**(QSharedPointer< [MeshManager](../Classes/classshapeworks_1_1MeshManager.md) > mesh_manager) |
| void | **[set_subject](../Classes/classshapeworks_1_1Shape.md#function-set-subject)**(std::shared_ptr< [shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md) > subject) |
| std::shared_ptr< [shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md) > | **[get_subject](../Classes/classshapeworks_1_1Shape.md#function-get-subject)**() |
| void | **[import_original_image](../Classes/classshapeworks_1_1Shape.md#function-import-original-image)**(const std::string & filename)<br>Import the original raw image file.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_original_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-original-meshes)**(bool wait =false)<br>Retrieve the original meshes.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_groomed_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-meshes)**(bool wait =false)<br>Retrieve the groomed meshes.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_reconstructed_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-reconstructed-meshes)**(bool wait =false)<br>Retrieve the reconstructed meshes.  |
| void | **[reset_groomed_mesh](../Classes/classshapeworks_1_1Shape.md#function-reset-groomed-mesh)**()<br>Reset the groomed mesh so that it will be re-created.  |
| bool | **[import_global_point_files](../Classes/classshapeworks_1_1Shape.md#function-import-global-point-files)**(QStringList filenames)<br>Import global correspondence point files.  |
| bool | **[import_local_point_files](../Classes/classshapeworks_1_1Shape.md#function-import-local-point-files)**(QStringList filenames)<br>Import local correspondence point files.  |
| bool | **[import_landmarks_files](../Classes/classshapeworks_1_1Shape.md#function-import-landmarks-files)**(QStringList filenames)<br>Import landmarks files.  |
| bool | **[store_landmarks](../Classes/classshapeworks_1_1Shape.md#function-store-landmarks)**()<br>Store landmarks.  |
| void | **[set_particles](../Classes/classshapeworks_1_1Shape.md#function-set-particles)**([StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) particles) |
| [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) | **[get_particles](../Classes/classshapeworks_1_1Shape.md#function-get-particles)**() |
| void | **[set_particle_transform](../Classes/classshapeworks_1_1Shape.md#function-set-particle-transform)**(vtkSmartPointer< vtkTransform > transform) |
| Eigen::VectorXd | **[get_global_correspondence_points](../Classes/classshapeworks_1_1Shape.md#function-get-global-correspondence-points)**()<br>Get the global correspondence points.  |
| Eigen::VectorXd | **[get_global_correspondence_points_for_display](../Classes/classshapeworks_1_1Shape.md#function-get-global-correspondence-points-for-display)**()<br>Get the global correspondence points for display.  |
| Eigen::VectorXd | **[get_local_correspondence_points](../Classes/classshapeworks_1_1Shape.md#function-get-local-correspondence-points)**()<br>Get the local correspondence points.  |
| void | **[clear_reconstructed_mesh](../Classes/classshapeworks_1_1Shape.md#function-clear-reconstructed-mesh)**() |
| int | **[get_id](../Classes/classshapeworks_1_1Shape.md#function-get-id)**()<br>Get the id of this shape.  |
| void | **[set_id](../Classes/classshapeworks_1_1Shape.md#function-set-id)**(int id)<br>Set the id of this shape.  |
| std::vector< QString > | **[get_original_filenames](../Classes/classshapeworks_1_1Shape.md#function-get-original-filenames)**() |
| std::vector< QString > | **[get_original_filenames_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-original-filenames-with-path)**() |
| QString | **[get_original_filename](../Classes/classshapeworks_1_1Shape.md#function-get-original-filename)**() |
| QString | **[get_original_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-original-filename-with-path)**() |
| QString | **[get_groomed_filename](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-filename)**() |
| QString | **[get_groomed_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-filename-with-path)**(int domain) |
| QString | **[get_global_point_filename](../Classes/classshapeworks_1_1Shape.md#function-get-global-point-filename)**() |
| QString | **[get_global_point_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-global-point-filename-with-path)**() |
| QString | **[get_local_point_filename](../Classes/classshapeworks_1_1Shape.md#function-get-local-point-filename)**() |
| QString | **[get_local_point_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-local-point-filename-with-path)**() |
| QList< [Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > | **[get_exclusion_sphere_centers](../Classes/classshapeworks_1_1Shape.md#function-get-exclusion-sphere-centers)**() |
| void | **[set_exclusion_sphere_centers](../Classes/classshapeworks_1_1Shape.md#function-set-exclusion-sphere-centers)**(QList< [Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > centers) |
| QList< double > | **[get_exclusion_sphere_radii](../Classes/classshapeworks_1_1Shape.md#function-get-exclusion-sphere-radii)**() |
| void | **[set_exclusion_sphere_radii](../Classes/classshapeworks_1_1Shape.md#function-set-exclusion-sphere-radii)**(QList< double > radii) |
| int | **[get_group_id](../Classes/classshapeworks_1_1Shape.md#function-get-group-id)**() |
| void | **[set_group_id](../Classes/classshapeworks_1_1Shape.md#function-set-group-id)**(int id) |
| std::vector< [Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > | **[get_vectors](../Classes/classshapeworks_1_1Shape.md#function-get-vectors)**() |
| void | **[set_vectors](../Classes/classshapeworks_1_1Shape.md#function-set-vectors)**(std::vector< [Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > vectors) |
| void | **[set_transform](../Classes/classshapeworks_1_1Shape.md#function-set-transform)**(vtkSmartPointer< vtkTransform > transform) |
| vtkSmartPointer< vtkTransform > | **[get_transform](../Classes/classshapeworks_1_1Shape.md#function-get-transform)**(int domain =0) |
| bool | **[has_alignment](../Classes/classshapeworks_1_1Shape.md#function-has-alignment)**() |
| vtkSmartPointer< vtkTransform > | **[get_original_transform](../Classes/classshapeworks_1_1Shape.md#function-get-original-transform)**(int domain =0) |
| void | **[set_reconstruction_transforms](../Classes/classshapeworks_1_1Shape.md#function-set-reconstruction-transforms)**(std::vector< vtkSmartPointer< vtkTransform >> transforms) |
| vtkSmartPointer< vtkTransform > | **[get_reconstruction_transform](../Classes/classshapeworks_1_1Shape.md#function-get-reconstruction-transform)**(int domain) |
| vtkSmartPointer< vtkTransform > | **[get_groomed_transform](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-transform)**(int domain =0) |
| vtkSmartPointer< vtkTransform > | **[get_procrustest_transform](../Classes/classshapeworks_1_1Shape.md#function-get-procrustest-transform)**(int domain =0) |
| std::vector< vtkSmartPointer< vtkTransform > > | **[get_procrustest_transforms](../Classes/classshapeworks_1_1Shape.md#function-get-procrustest-transforms)**() |
| vtkSmartPointer< vtkTransform > | **[get_alignment](../Classes/classshapeworks_1_1Shape.md#function-get-alignment)**(int domain =0) |
| void | **[load_feature](../Classes/classshapeworks_1_1Shape.md#function-load-feature)**(std::string display_mode, std::string feature) |
| vtkSmartPointer< vtkImageData > | **[get_image_volume](../Classes/classshapeworks_1_1Shape.md#function-get-image-volume)**(std::string image_volume_name) |
| Eigen::VectorXf | **[get_point_features](../Classes/classshapeworks_1_1Shape.md#function-get-point-features)**(std::string feature) |
| void | **[set_point_features](../Classes/classshapeworks_1_1Shape.md#function-set-point-features)**(std::string feature, Eigen::VectorXf values) |
| void | **[load_feature_from_scalar_file](../Classes/classshapeworks_1_1Shape.md#function-load-feature-from-scalar-file)**(std::string filename, std::string feature_name) |
| void | **[set_override_feature](../Classes/classshapeworks_1_1Shape.md#function-set-override-feature)**(std::string feature) |
| std::string | **[get_override_feature](../Classes/classshapeworks_1_1Shape.md#function-get-override-feature)**() |
| Eigen::MatrixXd & | **[landmarks](../Classes/classshapeworks_1_1Shape.md#function-landmarks)**() |

## Public Functions Documentation

### function Shape

```cpp
Shape()
```


### function ~Shape

```cpp
~Shape()
```


### function get_meshes

```cpp
MeshGroup get_meshes(
    const string & display_mode
)
```


### function set_annotations

```cpp
void set_annotations(
    QStringList annotations,
    bool only_overwrite_blank =true
)
```


### function get_annotations

```cpp
QStringList get_annotations()
```


### function set_mesh_manager

```cpp
void set_mesh_manager(
    QSharedPointer< MeshManager > mesh_manager
)
```


### function set_subject

```cpp
void set_subject(
    std::shared_ptr< shapeworks::Subject > subject
)
```


### function get_subject

```cpp
std::shared_ptr< shapeworks::Subject > get_subject()
```


### function import_original_image

```cpp
void import_original_image(
    const std::string & filename
)
```

Import the original raw image file. 

### function get_original_meshes

```cpp
MeshGroup get_original_meshes(
    bool wait =false
)
```

Retrieve the original meshes. 

### function get_groomed_meshes

```cpp
MeshGroup get_groomed_meshes(
    bool wait =false
)
```

Retrieve the groomed meshes. 

### function get_reconstructed_meshes

```cpp
MeshGroup get_reconstructed_meshes(
    bool wait =false
)
```

Retrieve the reconstructed meshes. 

### function reset_groomed_mesh

```cpp
void reset_groomed_mesh()
```

Reset the groomed mesh so that it will be re-created. 

### function import_global_point_files

```cpp
bool import_global_point_files(
    QStringList filenames
)
```

Import global correspondence point files. 

### function import_local_point_files

```cpp
bool import_local_point_files(
    QStringList filenames
)
```

Import local correspondence point files. 

### function import_landmarks_files

```cpp
bool import_landmarks_files(
    QStringList filenames
)
```

Import landmarks files. 

### function store_landmarks

```cpp
bool store_landmarks()
```

Store landmarks. 

### function set_particles

```cpp
void set_particles(
    StudioParticles particles
)
```


### function get_particles

```cpp
StudioParticles get_particles()
```


### function set_particle_transform

```cpp
void set_particle_transform(
    vtkSmartPointer< vtkTransform > transform
)
```


### function get_global_correspondence_points

```cpp
Eigen::VectorXd get_global_correspondence_points()
```

Get the global correspondence points. 

### function get_global_correspondence_points_for_display

```cpp
Eigen::VectorXd get_global_correspondence_points_for_display()
```

Get the global correspondence points for display. 

### function get_local_correspondence_points

```cpp
Eigen::VectorXd get_local_correspondence_points()
```

Get the local correspondence points. 

### function clear_reconstructed_mesh

```cpp
void clear_reconstructed_mesh()
```


### function get_id

```cpp
int get_id()
```

Get the id of this shape. 

### function set_id

```cpp
void set_id(
    int id
)
```

Set the id of this shape. 

### function get_original_filenames

```cpp
std::vector< QString > get_original_filenames()
```


### function get_original_filenames_with_path

```cpp
std::vector< QString > get_original_filenames_with_path()
```


### function get_original_filename

```cpp
QString get_original_filename()
```


### function get_original_filename_with_path

```cpp
QString get_original_filename_with_path()
```


### function get_groomed_filename

```cpp
QString get_groomed_filename()
```


### function get_groomed_filename_with_path

```cpp
QString get_groomed_filename_with_path(
    int domain
)
```


### function get_global_point_filename

```cpp
QString get_global_point_filename()
```


### function get_global_point_filename_with_path

```cpp
QString get_global_point_filename_with_path()
```


### function get_local_point_filename

```cpp
QString get_local_point_filename()
```


### function get_local_point_filename_with_path

```cpp
QString get_local_point_filename_with_path()
```


### function get_exclusion_sphere_centers

```cpp
QList< Point > get_exclusion_sphere_centers()
```


### function set_exclusion_sphere_centers

```cpp
void set_exclusion_sphere_centers(
    QList< Point > centers
)
```


### function get_exclusion_sphere_radii

```cpp
QList< double > get_exclusion_sphere_radii()
```


### function set_exclusion_sphere_radii

```cpp
void set_exclusion_sphere_radii(
    QList< double > radii
)
```


### function get_group_id

```cpp
int get_group_id()
```


### function set_group_id

```cpp
void set_group_id(
    int id
)
```


### function get_vectors

```cpp
std::vector< Point > get_vectors()
```


### function set_vectors

```cpp
void set_vectors(
    std::vector< Point > vectors
)
```


### function set_transform

```cpp
void set_transform(
    vtkSmartPointer< vtkTransform > transform
)
```


### function get_transform

```cpp
vtkSmartPointer< vtkTransform > get_transform(
    int domain =0
)
```


### function has_alignment

```cpp
bool has_alignment()
```


### function get_original_transform

```cpp
vtkSmartPointer< vtkTransform > get_original_transform(
    int domain =0
)
```


### function set_reconstruction_transforms

```cpp
void set_reconstruction_transforms(
    std::vector< vtkSmartPointer< vtkTransform >> transforms
)
```


### function get_reconstruction_transform

```cpp
vtkSmartPointer< vtkTransform > get_reconstruction_transform(
    int domain
)
```


### function get_groomed_transform

```cpp
vtkSmartPointer< vtkTransform > get_groomed_transform(
    int domain =0
)
```


### function get_procrustest_transform

```cpp
vtkSmartPointer< vtkTransform > get_procrustest_transform(
    int domain =0
)
```


### function get_procrustest_transforms

```cpp
std::vector< vtkSmartPointer< vtkTransform > > get_procrustest_transforms()
```


### function get_alignment

```cpp
vtkSmartPointer< vtkTransform > get_alignment(
    int domain =0
)
```


### function load_feature

```cpp
void load_feature(
    std::string display_mode,
    std::string feature
)
```


### function get_image_volume

```cpp
vtkSmartPointer< vtkImageData > get_image_volume(
    std::string image_volume_name
)
```


### function get_point_features

```cpp
Eigen::VectorXf get_point_features(
    std::string feature
)
```


### function set_point_features

```cpp
void set_point_features(
    std::string feature,
    Eigen::VectorXf values
)
```


### function load_feature_from_scalar_file

```cpp
void load_feature_from_scalar_file(
    std::string filename,
    std::string feature_name
)
```


### function set_override_feature

```cpp
void set_override_feature(
    std::string feature
)
```


### function get_override_feature

```cpp
std::string get_override_feature()
```


### function landmarks

```cpp
Eigen::MatrixXd & landmarks()
```


-------------------------------

Updated on 2022-03-05 at 23:20:34 +0000