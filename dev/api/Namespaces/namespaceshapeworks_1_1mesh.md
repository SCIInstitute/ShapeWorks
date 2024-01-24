---
title: shapeworks::mesh

---

# shapeworks::mesh



## Functions

|                | Name           |
| -------------- | -------------- |
| void | **[compute_thickness](../Namespaces/namespaceshapeworks_1_1mesh.md#function-compute-thickness)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh, [Image](../Classes/classshapeworks_1_1Image.md) & image, [Image](../Classes/classshapeworks_1_1Image.md) * dt, double max_dist, double median_radius, std::string distance_mesh)<br>Compute the cortical thickness of a mesh and image (e.g. CT)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[compute_inner_mesh](../Namespaces/namespaceshapeworks_1_1mesh.md#function-compute-inner-mesh)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh, std::string array_name) |


## Functions Documentation

### function compute_thickness

```cpp
void compute_thickness(
    Mesh & mesh,
    Image & image,
    Image * dt,
    double max_dist,
    double median_radius,
    std::string distance_mesh
)
```

Compute the cortical thickness of a mesh and image (e.g. CT) 

### function compute_inner_mesh

```cpp
Mesh compute_inner_mesh(
    const Mesh & mesh,
    std::string array_name
)
```






-------------------------------

Updated on 2024-01-24 at 10:55:24 +0000