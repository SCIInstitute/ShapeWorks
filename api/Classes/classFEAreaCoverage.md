---
title: FEAreaCoverage

---

# FEAreaCoverage





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEAreaCoverage](../Classes/classFEAreaCoverage.md#function-feareacoverage)**() |
| void | **[SetSelection1](../Classes/classFEAreaCoverage.md#function-setselection1)**(vector< int > & s) |
| void | **[SetSelection2](../Classes/classFEAreaCoverage.md#function-setselection2)**(vector< int > & s) |
| vector< double > | **[Apply](../Classes/classFEAreaCoverage.md#function-apply)**(std::shared_ptr< [FEMesh](../Classes/classFEMesh.md) > mesh1, std::shared_ptr< [FEMesh](../Classes/classFEMesh.md) > mesh2) |
| void | **[AllowBackIntersection](../Classes/classFEAreaCoverage.md#function-allowbackintersection)**(bool b) |
| bool | **[AllowBackIntersection](../Classes/classFEAreaCoverage.md#function-allowbackintersection)**() const |
| void | **[SetAngleThreshold](../Classes/classFEAreaCoverage.md#function-setanglethreshold)**(double w) |
| double | **[GetAngleThreshold](../Classes/classFEAreaCoverage.md#function-getanglethreshold)**() const |
| void | **[SetBackSearchRadius](../Classes/classFEAreaCoverage.md#function-setbacksearchradius)**(double R) |
| double | **[GetBackSearchRadius](../Classes/classFEAreaCoverage.md#function-getbacksearchradius)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[UpdateSurface](../Classes/classFEAreaCoverage.md#function-updatesurface)**(FEAreaCoverage::Surface & s) |
| bool | **[intersect](../Classes/classFEAreaCoverage.md#function-intersect)**(const [vec3d](../Classes/classvec3d.md) & r, const [vec3d](../Classes/classvec3d.md) & N, FEAreaCoverage::Surface & surf, [Intersection](../Classes/structIntersection.md) & q) |
| bool | **[faceIntersect](../Classes/classFEAreaCoverage.md#function-faceintersect)**(FEAreaCoverage::Surface & surf, const [Ray](../Classes/structRay.md) & ray, int nface, [Intersection](../Classes/structIntersection.md) & q) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| Surface | **[m_surf1](../Classes/classFEAreaCoverage.md#variable-m-surf1)**  |
| Surface | **[m_surf2](../Classes/classFEAreaCoverage.md#variable-m-surf2)**  |
| bool | **[m_ballowBackIntersections](../Classes/classFEAreaCoverage.md#variable-m-ballowbackintersections)**  |
| double | **[m_angleThreshold](../Classes/classFEAreaCoverage.md#variable-m-anglethreshold)**  |
| double | **[m_backSearchRadius](../Classes/classFEAreaCoverage.md#variable-m-backsearchradius)**  |

## Public Functions Documentation

### function FEAreaCoverage

```cpp
FEAreaCoverage()
```


### function SetSelection1

```cpp
inline void SetSelection1(
    vector< int > & s
)
```


### function SetSelection2

```cpp
inline void SetSelection2(
    vector< int > & s
)
```


### function Apply

```cpp
vector< double > Apply(
    std::shared_ptr< FEMesh > mesh1,
    std::shared_ptr< FEMesh > mesh2
)
```


### function AllowBackIntersection

```cpp
void AllowBackIntersection(
    bool b
)
```


### function AllowBackIntersection

```cpp
bool AllowBackIntersection() const
```


### function SetAngleThreshold

```cpp
void SetAngleThreshold(
    double w
)
```


### function GetAngleThreshold

```cpp
double GetAngleThreshold() const
```


### function SetBackSearchRadius

```cpp
void SetBackSearchRadius(
    double R
)
```


### function GetBackSearchRadius

```cpp
double GetBackSearchRadius() const
```


## Protected Functions Documentation

### function UpdateSurface

```cpp
void UpdateSurface(
    FEAreaCoverage::Surface & s
)
```


### function intersect

```cpp
bool intersect(
    const vec3d & r,
    const vec3d & N,
    FEAreaCoverage::Surface & surf,
    Intersection & q
)
```


### function faceIntersect

```cpp
bool faceIntersect(
    FEAreaCoverage::Surface & surf,
    const Ray & ray,
    int nface,
    Intersection & q
)
```


## Protected Attributes Documentation

### variable m_surf1

```cpp
Surface m_surf1;
```


### variable m_surf2

```cpp
Surface m_surf2;
```


### variable m_ballowBackIntersections

```cpp
bool m_ballowBackIntersections;
```


### variable m_angleThreshold

```cpp
double m_angleThreshold;
```


### variable m_backSearchRadius

```cpp
double m_backSearchRadius;
```


-------------------------------

Updated on 2022-01-01 at 17:29:13 +0000