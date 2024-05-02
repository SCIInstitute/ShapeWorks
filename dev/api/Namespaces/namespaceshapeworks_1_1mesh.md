---
title: shapeworks::mesh

---

# shapeworks::mesh



## Functions

|                | Name           |
| -------------- | -------------- |
| void | **[compute_thickness](../Namespaces/namespaceshapeworks_1_1mesh.md#function-compute-thickness)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh, [Image](../Classes/classshapeworks_1_1Image.md) & image, [Image](../Classes/classshapeworks_1_1Image.md) * dt, double max_dist, double median_radius, std::string distance_mesh)<br>Compute the cortical thickness of a mesh and image (e.g. CT)  |
| [Mesh](../Classes/classshapeworks_1_1Mesh.md) | **[compute_inner_mesh](../Namespaces/namespaceshapeworks_1_1mesh.md#function-compute-inner-mesh)**(const [Mesh](../Classes/classshapeworks_1_1Mesh.md) & mesh, std::string array_name)<br>Compute an internal mesh based on the thickness values.  |
| void | **[summarize_internal_intensities](../Namespaces/namespaceshapeworks_1_1mesh.md#function-summarize-internal-intensities)**([Mesh](../Classes/classshapeworks_1_1Mesh.md) & outer_mesh, [Mesh](../Classes/classshapeworks_1_1Mesh.md) & inner_mesh, [Image](../Classes/classshapeworks_1_1Image.md) & image)<br>Summarize the internal intensities values of the area inside the inner mesh on the outer mesh.  |


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

Compute an internal mesh based on the thickness values. 

### function summarize_internal_intensities

```cpp
void summarize_internal_intensities(
    Mesh & outer_mesh,
    Mesh & inner_mesh,
    Image & image
)
```

Summarize the internal intensities values of the area inside the inner mesh on the outer mesh. 





-------------------------------

Updated on 2024-05-02 at 20:57:31 +0000