---
title: Libs/Mesh/PreviewMeshQC/Intersect.h

---

# Libs/Mesh/PreviewMeshQC/Intersect.h



## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[Ray](../Classes/structRay.md)**  |
| struct | **[Intersection](../Classes/structIntersection.md)**  |
| struct | **[Triangle](../Classes/structTriangle.md)**  |
| struct | **[Quad](../Classes/structQuad.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[IntersectTriangle](../Files/Intersect_8h.md#function-intersecttriangle)**(const [Ray](../Classes/structRay.md) & ray, const [Triangle](../Classes/structTriangle.md) & tri, [Intersection](../Classes/structIntersection.md) & q, bool evalNormal =true) |
| bool | **[IntersectQuad](../Files/Intersect_8h.md#function-intersectquad)**(const [Ray](../Classes/structRay.md) & ray, const [Quad](../Classes/structQuad.md) & quad, [Intersection](../Classes/structIntersection.md) & q) |
| bool | **[FastIntersectQuad](../Files/Intersect_8h.md#function-fastintersectquad)**(const [Ray](../Classes/structRay.md) & ray, const [Quad](../Classes/structQuad.md) & quad, [Intersection](../Classes/structIntersection.md) & q) |


## Functions Documentation

### function IntersectTriangle

```cpp
bool IntersectTriangle(
    const Ray & ray,
    const Triangle & tri,
    Intersection & q,
    bool evalNormal =true
)
```


### function IntersectQuad

```cpp
bool IntersectQuad(
    const Ray & ray,
    const Quad & quad,
    Intersection & q
)
```


### function FastIntersectQuad

```cpp
bool FastIntersectQuad(
    const Ray & ray,
    const Quad & quad,
    Intersection & q
)
```




## Source code

```cpp
#pragma once
#include "math3d.h"

// tools for finding intersections
struct Ray
{
    vec3d   origin;         // origin of ray
    vec3d   direction;      // direction of ray (must be unit vector!)
};

//-----------------------------------------------------------------------------
struct Intersection
{
    vec3d   point;          // point of intersection
    float   r[2];           // natural coordinates
    int     m_index;        // index of item that was intersected (context dependent)
    int     m_faceIndex;    // index of face that was intersected (context dependent)
};

//-----------------------------------------------------------------------------
struct Triangle
{
    vec3d   r0;
    vec3d   r1;
    vec3d   r2;
    vec3d   fn; // face normal
};

//-----------------------------------------------------------------------------
struct Quad
{
    vec3d   r0;
    vec3d   r1;
    vec3d   r2;
    vec3d   r3;
};

//-----------------------------------------------------------------------------
// Find intersection of a ray with a triangle
// To evaluate the normal automatically, set evalNormal to true. Otherwise, the normal in Triangle is used
bool IntersectTriangle(const Ray& ray, const Triangle& tri, Intersection& q, bool evalNormal = true);

//-----------------------------------------------------------------------------
// Find intersection of a ray with a quad
bool IntersectQuad(const Ray& ray, const Quad& quad, Intersection& q);
bool FastIntersectQuad(const Ray& ray, const Quad& quad, Intersection& q);
```


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000
