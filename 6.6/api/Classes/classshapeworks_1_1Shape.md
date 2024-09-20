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
| std::string | **[get_display_name](../Classes/classshapeworks_1_1Shape.md#function-get-display-name)**() |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-meshes)**(DisplayMode display_mode, bool wait =false) |
| void | **[set_annotations](../Classes/classshapeworks_1_1Shape.md#function-set-annotations)**(std::vector< std::string > annotations, bool only_overwrite_blank =true) |
| std::vector< std::string > | **[get_annotations](../Classes/classshapeworks_1_1Shape.md#function-get-annotations)**() |
| void | **[set_mesh_manager](../Classes/classshapeworks_1_1Shape.md#function-set-mesh-manager)**(std::shared_ptr< [MeshManager](../Classes/classshapeworks_1_1MeshManager.md) > mesh_manager) |
| void | **[set_subject](../Classes/classshapeworks_1_1Shape.md#function-set-subject)**(std::shared_ptr< [shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md) > subject) |
| bool | **[is_subject](../Classes/classshapeworks_1_1Shape.md#function-is-subject)**()<br>Is this shape a population subject (e.g. mean/pca constructions are not)  |
| std::shared_ptr< [shapeworks::Subject](../Classes/classshapeworks_1_1Subject.md) > | **[get_subject](../Classes/classshapeworks_1_1Shape.md#function-get-subject)**()<br>Return the pointer to the subject object.  |
| bool | **[is_fixed](../Classes/classshapeworks_1_1Shape.md#function-is-fixed)**()<br>Helper to ask if this shape is fixed or not.  |
| bool | **[is_excluded](../Classes/classshapeworks_1_1Shape.md#function-is-excluded)**()<br>Helper to ask if this shape is excluded.  |
| void | **[import_original_file](../Classes/classshapeworks_1_1Shape.md#function-import-original-file)**(const std::string & filename)<br>Import the original raw mesh or image file.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_original_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-original-meshes)**(bool wait =false)<br>Retrieve the original meshes.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_groomed_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-meshes)**(bool wait =false)<br>Retrieve the groomed meshes.  |
| [MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) | **[get_reconstructed_meshes](../Classes/classshapeworks_1_1Shape.md#function-get-reconstructed-meshes)**(bool wait =false)<br>Retrieve the reconstructed meshes.  |
| void | **[set_reconstructed_meshes](../Classes/classshapeworks_1_1Shape.md#function-set-reconstructed-meshes)**([MeshGroup](../Classes/classshapeworks_1_1MeshGroup.md) meshes)<br>Set the reconstructed meshes.  |
| void | **[reset_groomed_mesh](../Classes/classshapeworks_1_1Shape.md#function-reset-groomed-mesh)**()<br>Reset the groomed mesh so that it will be re-created.  |
| bool | **[import_global_point_files](../Classes/classshapeworks_1_1Shape.md#function-import-global-point-files)**(std::vector< std::string > filenames)<br>Import global correspondence point files.  |
| bool | **[import_local_point_files](../Classes/classshapeworks_1_1Shape.md#function-import-local-point-files)**(std::vector< std::string > filenames)<br>Import local correspondence point files.  |
| bool | **[import_landmarks_files](../Classes/classshapeworks_1_1Shape.md#function-import-landmarks-files)**(std::vector< std::string > filenames)<br>Import landmarks files.  |
| bool | **[store_landmarks](../Classes/classshapeworks_1_1Shape.md#function-store-landmarks)**()<br>Store landmarks.  |
| bool | **[import_constraints](../Classes/classshapeworks_1_1Shape.md#function-import-constraints)**(std::vector< std::string > filenames)<br>import constraints  |
| bool | **[store_constraints](../Classes/classshapeworks_1_1Shape.md#function-store-constraints)**()<br>Store constraints.  |
| void | **[set_particles](../Classes/classshapeworks_1_1Shape.md#function-set-particles)**([Particles](../Classes/classshapeworks_1_1Particles.md) particles)<br>Set particles.  |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_particles](../Classes/classshapeworks_1_1Shape.md#function-get-particles)**()<br>Get particles.  |
| void | **[set_particle_transform](../Classes/classshapeworks_1_1Shape.md#function-set-particle-transform)**(vtkSmartPointer< vtkTransform > transform)<br>Set the particle transform (alignment)  |
| void | **[set_alignment_type](../Classes/classshapeworks_1_1Shape.md#function-set-alignment-type)**(int alignment)<br>Set the alignment type.  |
| Eigen::VectorXd | **[get_global_correspondence_points](../Classes/classshapeworks_1_1Shape.md#function-get-global-correspondence-points)**()<br>Get the global correspondence points.  |
| std::vector< Eigen::VectorXd > | **[get_particles_for_display](../Classes/classshapeworks_1_1Shape.md#function-get-particles-for-display)**()<br>Get the global correspondence points for display.  |
| Eigen::VectorXd | **[get_local_correspondence_points](../Classes/classshapeworks_1_1Shape.md#function-get-local-correspondence-points)**()<br>Get the local correspondence points.  |
| void | **[clear_reconstructed_mesh](../Classes/classshapeworks_1_1Shape.md#function-clear-reconstructed-mesh)**() |
| int | **[get_id](../Classes/classshapeworks_1_1Shape.md#function-get-id)**()<br>Get the id of this shape.  |
| void | **[set_id](../Classes/classshapeworks_1_1Shape.md#function-set-id)**(int id)<br>Set the id of this shape.  |
| void | **[update_annotations](../Classes/classshapeworks_1_1Shape.md#function-update-annotations)**()<br>Update the name of this shape.  |
| std::vector< std::string > | **[get_original_filenames](../Classes/classshapeworks_1_1Shape.md#function-get-original-filenames)**() |
| std::vector< std::string > | **[get_original_filenames_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-original-filenames-with-path)**() |
| std::string | **[get_original_filename](../Classes/classshapeworks_1_1Shape.md#function-get-original-filename)**() |
| std::string | **[get_original_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-original-filename-with-path)**() |
| std::string | **[get_groomed_filename](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-filename)**() |
| std::string | **[get_groomed_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-filename-with-path)**(int domain) |
| std::string | **[get_global_point_filename](../Classes/classshapeworks_1_1Shape.md#function-get-global-point-filename)**() |
| std::string | **[get_global_point_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-global-point-filename-with-path)**() |
| std::string | **[get_local_point_filename](../Classes/classshapeworks_1_1Shape.md#function-get-local-point-filename)**() |
| std::string | **[get_local_point_filename_with_path](../Classes/classshapeworks_1_1Shape.md#function-get-local-point-filename-with-path)**() |
| void | **[set_transform](../Classes/classshapeworks_1_1Shape.md#function-set-transform)**(vtkSmartPointer< vtkTransform > transform) |
| vtkSmartPointer< vtkTransform > | **[get_transform](../Classes/classshapeworks_1_1Shape.md#function-get-transform)**(int domain =0) |
| vtkSmartPointer< vtkTransform > | **[get_inverse_transform](../Classes/classshapeworks_1_1Shape.md#function-get-inverse-transform)**(int domain =0) |
| bool | **[has_alignment](../Classes/classshapeworks_1_1Shape.md#function-has-alignment)**() |
| vtkSmartPointer< vtkTransform > | **[get_original_transform](../Classes/classshapeworks_1_1Shape.md#function-get-original-transform)**(int domain =0) |
| void | **[set_reconstruction_transforms](../Classes/classshapeworks_1_1Shape.md#function-set-reconstruction-transforms)**(std::vector< vtkSmartPointer< vtkTransform > > transforms) |
| vtkSmartPointer< vtkTransform > | **[get_reconstruction_transform](../Classes/classshapeworks_1_1Shape.md#function-get-reconstruction-transform)**(int domain) |
| vtkSmartPointer< vtkTransform > | **[get_groomed_transform](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-transform)**(int domain =0) |
| vtkSmartPointer< vtkTransform > | **[get_procrustes_transform](../Classes/classshapeworks_1_1Shape.md#function-get-procrustes-transform)**(int domain =0) |
| std::vector< vtkSmartPointer< vtkTransform > > | **[get_procrustes_transforms](../Classes/classshapeworks_1_1Shape.md#function-get-procrustes-transforms)**() |
| vtkSmartPointer< vtkTransform > | **[get_alignment](../Classes/classshapeworks_1_1Shape.md#function-get-alignment)**(int domain =0) |
| void | **[load_feature](../Classes/classshapeworks_1_1Shape.md#function-load-feature)**(DisplayMode display_mode, std::string feature) |
| std::shared_ptr< [Image](../Classes/classshapeworks_1_1Image.md) > | **[get_image_volume](../Classes/classshapeworks_1_1Shape.md#function-get-image-volume)**(std::string image_volume_name) |
| Eigen::VectorXd | **[get_point_features](../Classes/classshapeworks_1_1Shape.md#function-get-point-features)**(std::string feature) |
| void | **[set_point_features](../Classes/classshapeworks_1_1Shape.md#function-set-point-features)**(std::string feature, Eigen::VectorXd values) |
| void | **[load_feature_from_scalar_file](../Classes/classshapeworks_1_1Shape.md#function-load-feature-from-scalar-file)**(std::string filename, std::string feature_name) |
| void | **[set_override_feature](../Classes/classshapeworks_1_1Shape.md#function-set-override-feature)**(std::string feature) |
| std::string | **[get_override_feature](../Classes/classshapeworks_1_1Shape.md#function-get-override-feature)**() |
| Eigen::MatrixXd & | **[landmarks](../Classes/classshapeworks_1_1Shape.md#function-landmarks)**() |
| std::vector< [Constraints](../Classes/classshapeworks_1_1Constraints.md) > & | **[constraints](../Classes/classshapeworks_1_1Shape.md#function-constraints)**() |
| [Constraints](../Classes/classshapeworks_1_1Constraints.md) & | **[get_constraints](../Classes/classshapeworks_1_1Shape.md#function-get-constraints)**(int domain_id) |
| bool | **[has_planes](../Classes/classshapeworks_1_1Shape.md#function-has-planes)**() |
| std::vector< std::shared_ptr< [MeshWrapper](../Classes/classshapeworks_1_1MeshWrapper.md) > > | **[get_groomed_mesh_wrappers](../Classes/classshapeworks_1_1Shape.md#function-get-groomed-mesh-wrappers)**() |

## Public Functions Documentation

### function Shape

```cpp
Shape()
```


### function ~Shape

```cpp
~Shape()
```


### function get_display_name

```cpp
std::string get_display_name()
```


### function get_meshes

```cpp
MeshGroup get_meshes(
    DisplayMode display_mode,
    bool wait =false
)
```


### function set_annotations

```cpp
void set_annotations(
    std::vector< std::string > annotations,
    bool only_overwrite_blank =true
)
```


### function get_annotations

```cpp
std::vector< std::string > get_annotations()
```


### function set_mesh_manager

```cpp
void set_mesh_manager(
    std::shared_ptr< MeshManager > mesh_manager
)
```


### function set_subject

```cpp
void set_subject(
    std::shared_ptr< shapeworks::Subject > subject
)
```


### function is_subject

```cpp
bool is_subject()
```

Is this shape a population subject (e.g. mean/pca constructions are not) 

### function get_subject

```cpp
std::shared_ptr< shapeworks::Subject > get_subject()
```

Return the pointer to the subject object. 

### function is_fixed

```cpp
bool is_fixed()
```

Helper to ask if this shape is fixed or not. 

### function is_excluded

```cpp
bool is_excluded()
```

Helper to ask if this shape is excluded. 

### function import_original_file

```cpp
void import_original_file(
    const std::string & filename
)
```

Import the original raw mesh or image file. 

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

### function set_reconstructed_meshes

```cpp
void set_reconstructed_meshes(
    MeshGroup meshes
)
```

Set the reconstructed meshes. 

### function reset_groomed_mesh

```cpp
void reset_groomed_mesh()
```

Reset the groomed mesh so that it will be re-created. 

### function import_global_point_files

```cpp
bool import_global_point_files(
    std::vector< std::string > filenames
)
```

Import global correspondence point files. 

### function import_local_point_files

```cpp
bool import_local_point_files(
    std::vector< std::string > filenames
)
```

Import local correspondence point files. 

### function import_landmarks_files

```cpp
bool import_landmarks_files(
    std::vector< std::string > filenames
)
```

Import landmarks files. 

### function store_landmarks

```cpp
bool store_landmarks()
```

Store landmarks. 

### function import_constraints

```cpp
bool import_constraints(
    std::vector< std::string > filenames
)
```

import constraints 

### function store_constraints

```cpp
bool store_constraints()
```

Store constraints. 

### function set_particles

```cpp
void set_particles(
    Particles particles
)
```

Set particles. 

### function get_particles

```cpp
Particles get_particles()
```

Get particles. 

### function set_particle_transform

```cpp
void set_particle_transform(
    vtkSmartPointer< vtkTransform > transform
)
```

Set the particle transform (alignment) 

### function set_alignment_type

```cpp
void set_alignment_type(
    int alignment
)
```

Set the alignment type. 

### function get_global_correspondence_points

```cpp
Eigen::VectorXd get_global_correspondence_points()
```

Get the global correspondence points. 

### function get_particles_for_display

```cpp
std::vector< Eigen::VectorXd > get_particles_for_display()
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

### function update_annotations

```cpp
void update_annotations()
```

Update the name of this shape. 

### function get_original_filenames

```cpp
std::vector< std::string > get_original_filenames()
```


### function get_original_filenames_with_path

```cpp
std::vector< std::string > get_original_filenames_with_path()
```


### function get_original_filename

```cpp
std::string get_original_filename()
```


### function get_original_filename_with_path

```cpp
std::string get_original_filename_with_path()
```


### function get_groomed_filename

```cpp
std::string get_groomed_filename()
```


### function get_groomed_filename_with_path

```cpp
std::string get_groomed_filename_with_path(
    int domain
)
```


### function get_global_point_filename

```cpp
std::string get_global_point_filename()
```


### function get_global_point_filename_with_path

```cpp
std::string get_global_point_filename_with_path()
```


### function get_local_point_filename

```cpp
std::string get_local_point_filename()
```


### function get_local_point_filename_with_path

```cpp
std::string get_local_point_filename_with_path()
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


### function get_inverse_transform

```cpp
vtkSmartPointer< vtkTransform > get_inverse_transform(
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
    std::vector< vtkSmartPointer< vtkTransform > > transforms
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


### function get_procrustes_transform

```cpp
vtkSmartPointer< vtkTransform > get_procrustes_transform(
    int domain =0
)
```


### function get_procrustes_transforms

```cpp
std::vector< vtkSmartPointer< vtkTransform > > get_procrustes_transforms()
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
    DisplayMode display_mode,
    std::string feature
)
```


### function get_image_volume

```cpp
std::shared_ptr< Image > get_image_volume(
    std::string image_volume_name
)
```


### function get_point_features

```cpp
Eigen::VectorXd get_point_features(
    std::string feature
)
```


### function set_point_features

```cpp
void set_point_features(
    std::string feature,
    Eigen::VectorXd values
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


### function constraints

```cpp
std::vector< Constraints > & constraints()
```


### function get_constraints

```cpp
Constraints & get_constraints(
    int domain_id
)
```


### function has_planes

```cpp
bool has_planes()
```


### function get_groomed_mesh_wrappers

```cpp
std::vector< std::shared_ptr< MeshWrapper > > get_groomed_mesh_wrappers()
```


-------------------------------

Updated on 2024-09-20 at 07:18:40 +0000