---
title: Libs/Mesh/PreviewMeshQC/FEMeshSmoothingModifier.h

---

# Libs/Mesh/PreviewMeshQC/FEMeshSmoothingModifier.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[FEMeshSmoothingModifier](../Classes/classFEMeshSmoothingModifier.md)**  |




## Source code

```cpp
#pragma once
#include "math3d.h"

class FEMesh;

//-----------------------------------------------------------------------------
class FEMeshSmoothingModifier
{

public:
    FEMeshSmoothingModifier();

    FEMesh* Apply(FEMesh* pm);

protected:
    double area_triangle(vec3d r[3]);
    double distance(vec3d x,vec3d y );
    double random();
    void Laplacian_Smoothing(FEMesh* pm,vector<int> hashmap);
    void Laplacian_Smoothing2(FEMesh* pm,vector<int> hashmap);
    void Taubin_Smoothing(FEMesh* pm,vector<int> hashmap);
    void Crease_Enhancing_Diffusion(FEMesh* pm,vector<int> hashmap);
    void Add_Noise(FEMesh* pm,vector<int> hashmap);

public:
    double  m_threshold1;
    double  m_threshold2;
    double m_iteration;
    int m_method;
};
```


-------------------------------

Updated on 2021-12-13 at 17:42:54 +0000
