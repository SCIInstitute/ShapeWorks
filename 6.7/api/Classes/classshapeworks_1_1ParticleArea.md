---
title: shapeworks::ParticleArea

---

# shapeworks::ParticleArea





## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[assign_vertex_particles](../Classes/classshapeworks_1_1ParticleArea.md#function-assign-vertex-particles)**(vtkSmartPointer< vtkPolyData > poly_data, std::vector< itk::Point< double > > particles)<br>assign particle ids for each vertex based on closest geodesic distance  |
| void | **[assign_vertex_colors](../Classes/classshapeworks_1_1ParticleArea.md#function-assign-vertex-colors)**(vtkSmartPointer< vtkPolyData > poly_data, std::vector< QColor > colors)<br>assign vertex colors based on particle ids  |
| void | **[assign_vertex_areas](../Classes/classshapeworks_1_1ParticleArea.md#function-assign-vertex-areas)**(vtkSmartPointer< vtkPolyData > poly_data, Eigen::VectorXd areas)<br>assign vertex areas based on particle ids  |
| std::vector< QColor > | **[colors_from_lut](../Classes/classshapeworks_1_1ParticleArea.md#function-colors-from-lut)**(vtkSmartPointer< vtkLookupTable > lut)<br>convert lut to array of colors  |
| Eigen::VectorXd | **[compute_particle_triangle_areas](../Classes/classshapeworks_1_1ParticleArea.md#function-compute-particle-triangle-areas)**(vtkSmartPointer< vtkPolyData > poly_data, std::vector< itk::Point< double > > particles)<br>compute the area assigned to each particle  |

## Public Functions Documentation

### function assign_vertex_particles

```cpp
static void assign_vertex_particles(
    vtkSmartPointer< vtkPolyData > poly_data,
    std::vector< itk::Point< double > > particles
)
```

assign particle ids for each vertex based on closest geodesic distance 

### function assign_vertex_colors

```cpp
static void assign_vertex_colors(
    vtkSmartPointer< vtkPolyData > poly_data,
    std::vector< QColor > colors
)
```

assign vertex colors based on particle ids 

### function assign_vertex_areas

```cpp
static void assign_vertex_areas(
    vtkSmartPointer< vtkPolyData > poly_data,
    Eigen::VectorXd areas
)
```

assign vertex areas based on particle ids 

### function colors_from_lut

```cpp
static std::vector< QColor > colors_from_lut(
    vtkSmartPointer< vtkLookupTable > lut
)
```

convert lut to array of colors 

### function compute_particle_triangle_areas

```cpp
static Eigen::VectorXd compute_particle_triangle_areas(
    vtkSmartPointer< vtkPolyData > poly_data,
    std::vector< itk::Point< double > > particles
)
```

compute the area assigned to each particle 

-------------------------------

Updated on 2025-10-10 at 18:16:00 +0000