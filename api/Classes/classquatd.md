---
title: quatd

---

# quatd





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[quatd](../Classes/classquatd.md#function-quatd)**() |
| | **[quatd](../Classes/classquatd.md#function-quatd)**(const double angle, [vec3d](../Classes/classvec3d.md) v) |
| | **[quatd](../Classes/classquatd.md#function-quatd)**([vec3d](../Classes/classvec3d.md) v1, [vec3d](../Classes/classvec3d.md) v2) |
| | **[quatd](../Classes/classquatd.md#function-quatd)**(const double qx, const double qy, const double qz, const double qw =1.0) |
| bool | **[operator!=](../Classes/classquatd.md#function-operator!=)**(const [quatd](../Classes/classquatd.md) & q) |
| [quatd](../Classes/classquatd.md) | **[operator-](../Classes/classquatd.md#function-operator-)**() |
| [quatd](../Classes/classquatd.md) | **[operator+](../Classes/classquatd.md#function-operator+)**(const [quatd](../Classes/classquatd.md) & q) const |
| [quatd](../Classes/classquatd.md) | **[operator-](../Classes/classquatd.md#function-operator-)**(const [quatd](../Classes/classquatd.md) & q) const |
| [quatd](../Classes/classquatd.md) & | **[operator+=](../Classes/classquatd.md#function-operator+=)**(const [quatd](../Classes/classquatd.md) & q) |
| [quatd](../Classes/classquatd.md) & | **[operator-=](../Classes/classquatd.md#function-operator-=)**(const [quatd](../Classes/classquatd.md) & q) |
| [quatd](../Classes/classquatd.md) | **[operator*](../Classes/classquatd.md#function-operator*)**(const [quatd](../Classes/classquatd.md) & q) const |
| [quatd](../Classes/classquatd.md) & | **[operator*=](../Classes/classquatd.md#function-operator*=)**(const [quatd](../Classes/classquatd.md) & q) |
| [quatd](../Classes/classquatd.md) | **[operator*](../Classes/classquatd.md#function-operator*)**(const double a) const |
| [quatd](../Classes/classquatd.md) | **[operator/](../Classes/classquatd.md#function-operator/)**(const double a) const |
| [quatd](../Classes/classquatd.md) & | **[operator/=](../Classes/classquatd.md#function-operator/=)**(const double a) |
| [quatd](../Classes/classquatd.md) | **[Conjugate](../Classes/classquatd.md#function-conjugate)**() const |
| double | **[Norm](../Classes/classquatd.md#function-norm)**() const |
| void | **[MakeUnit](../Classes/classquatd.md#function-makeunit)**() |
| [quatd](../Classes/classquatd.md) | **[Inverse](../Classes/classquatd.md#function-inverse)**() const |
| double | **[DotProduct](../Classes/classquatd.md#function-dotproduct)**(const [quatd](../Classes/classquatd.md) & q) const |
| [vec3d](../Classes/classvec3d.md) | **[GetVector](../Classes/classquatd.md#function-getvector)**() const |
| double | **[GetAngle](../Classes/classquatd.md#function-getangle)**() const |
| void | **[RotateVector](../Classes/classquatd.md#function-rotatevector)**([vec3d](../Classes/classvec3d.md) & v) const |
| [vec3d](../Classes/classvec3d.md) | **[operator*](../Classes/classquatd.md#function-operator*)**(const [vec3d](../Classes/classvec3d.md) & r) |
| [mat3d](../Classes/classmat3d.md) | **[operator*](../Classes/classquatd.md#function-operator*)**([mat3d](../Classes/classmat3d.md) m) |
| void | **[RotateVectorP](../Classes/classquatd.md#function-rotatevectorp)**(double * v, double * r) const |
| double | **[dot](../Classes/classquatd.md#function-dot)**([quatd](../Classes/classquatd.md) & q1, [quatd](../Classes/classquatd.md) & q2) |
| [quatd](../Classes/classquatd.md) | **[lerp](../Classes/classquatd.md#function-lerp)**([quatd](../Classes/classquatd.md) & q1, [quatd](../Classes/classquatd.md) & q2, double t) |
| [quatd](../Classes/classquatd.md) | **[slerp](../Classes/classquatd.md#function-slerp)**([quatd](../Classes/classquatd.md) & q1, [quatd](../Classes/classquatd.md) & q2, double t) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[x](../Classes/classquatd.md#variable-x)**  |
| double | **[y](../Classes/classquatd.md#variable-y)**  |
| double | **[z](../Classes/classquatd.md#variable-z)**  |
| double | **[w](../Classes/classquatd.md#variable-w)**  |

## Public Functions Documentation

### function quatd

```cpp
inline quatd()
```


### function quatd

```cpp
inline quatd(
    const double angle,
    vec3d v
)
```


### function quatd

```cpp
inline quatd(
    vec3d v1,
    vec3d v2
)
```


### function quatd

```cpp
inline quatd(
    const double qx,
    const double qy,
    const double qz,
    const double qw =1.0
)
```


### function operator!=

```cpp
inline bool operator!=(
    const quatd & q
)
```


### function operator-

```cpp
inline quatd operator-()
```


### function operator+

```cpp
inline quatd operator+(
    const quatd & q
) const
```


### function operator-

```cpp
inline quatd operator-(
    const quatd & q
) const
```


### function operator+=

```cpp
inline quatd & operator+=(
    const quatd & q
)
```


### function operator-=

```cpp
inline quatd & operator-=(
    const quatd & q
)
```


### function operator*

```cpp
inline quatd operator*(
    const quatd & q
) const
```


### function operator*=

```cpp
inline quatd & operator*=(
    const quatd & q
)
```


### function operator*

```cpp
inline quatd operator*(
    const double a
) const
```


### function operator/

```cpp
inline quatd operator/(
    const double a
) const
```


### function operator/=

```cpp
inline quatd & operator/=(
    const double a
)
```


### function Conjugate

```cpp
inline quatd Conjugate() const
```


### function Norm

```cpp
inline double Norm() const
```


### function MakeUnit

```cpp
inline void MakeUnit()
```


### function Inverse

```cpp
inline quatd Inverse() const
```


### function DotProduct

```cpp
inline double DotProduct(
    const quatd & q
) const
```


### function GetVector

```cpp
inline vec3d GetVector() const
```


### function GetAngle

```cpp
inline double GetAngle() const
```


### function RotateVector

```cpp
inline void RotateVector(
    vec3d & v
) const
```


### function operator*

```cpp
inline vec3d operator*(
    const vec3d & r
)
```


### function operator*

```cpp
inline mat3d operator*(
    mat3d m
)
```


### function RotateVectorP

```cpp
inline void RotateVectorP(
    double * v,
    double * r
) const
```


### function dot

```cpp
static inline double dot(
    quatd & q1,
    quatd & q2
)
```


### function lerp

```cpp
static inline quatd lerp(
    quatd & q1,
    quatd & q2,
    double t
)
```


### function slerp

```cpp
static quatd slerp(
    quatd & q1,
    quatd & q2,
    double t
)
```


## Public Attributes Documentation

### variable x

```cpp
double x;
```


### variable y

```cpp
double y;
```


### variable z

```cpp
double z;
```


### variable w

```cpp
double w;
```


-------------------------------

Updated on 2022-01-10 at 16:27:27 +0000