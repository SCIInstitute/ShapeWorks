---
title: vec3d

---

# vec3d





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[vec3d](../Classes/classvec3d.md#function-vec3d)**() |
| | **[vec3d](../Classes/classvec3d.md#function-vec3d)**(double rx, double ry, double rz) |
| | **[vec3d](../Classes/classvec3d.md#function-vec3d)**(const [vec2d](../Classes/classvec2d.md) & r) |
| [vec3d](../Classes/classvec3d.md) | **[operator+](../Classes/classvec3d.md#function-operator+)**(const [vec3d](../Classes/classvec3d.md) & v) const |
| [vec3d](../Classes/classvec3d.md) | **[operator-](../Classes/classvec3d.md#function-operator-)**(const [vec3d](../Classes/classvec3d.md) & v) const |
| [vec3d](../Classes/classvec3d.md) | **[operator^](../Classes/classvec3d.md#function-operator^)**(const [vec3d](../Classes/classvec3d.md) & v) const |
| double | **[operator*](../Classes/classvec3d.md#function-operator*)**(const [vec3d](../Classes/classvec3d.md) & v) const |
| [vec3d](../Classes/classvec3d.md) | **[operator*](../Classes/classvec3d.md#function-operator*)**(const double g) const |
| [vec3d](../Classes/classvec3d.md) | **[operator/](../Classes/classvec3d.md#function-operator/)**(const double g) const |
| const [vec3d](../Classes/classvec3d.md) & | **[operator+=](../Classes/classvec3d.md#function-operator+=)**(const [vec3d](../Classes/classvec3d.md) & v) |
| const [vec3d](../Classes/classvec3d.md) & | **[operator-=](../Classes/classvec3d.md#function-operator-=)**(const [vec3d](../Classes/classvec3d.md) & v) |
| const [vec3d](../Classes/classvec3d.md) & | **[operator/=](../Classes/classvec3d.md#function-operator/=)**(const double f) |
| const [vec3d](../Classes/classvec3d.md) & | **[operator/=](../Classes/classvec3d.md#function-operator/=)**(const int n) |
| const [vec3d](../Classes/classvec3d.md) & | **[operator*=](../Classes/classvec3d.md#function-operator*=)**(const double f) |
| [vec3d](../Classes/classvec3d.md) | **[operator-](../Classes/classvec3d.md#function-operator-)**() const |
| double | **[Length](../Classes/classvec3d.md#function-length)**() const |
| double | **[SqrLength](../Classes/classvec3d.md#function-sqrlength)**() const |
| [vec3d](../Classes/classvec3d.md) & | **[Normalize](../Classes/classvec3d.md#function-normalize)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[x](../Classes/classvec3d.md#variable-x)**  |
| double | **[y](../Classes/classvec3d.md#variable-y)**  |
| double | **[z](../Classes/classvec3d.md#variable-z)**  |

## Public Functions Documentation

### function vec3d

```cpp
inline vec3d()
```


### function vec3d

```cpp
inline vec3d(
    double rx,
    double ry,
    double rz
)
```


### function vec3d

```cpp
inline vec3d(
    const vec2d & r
)
```


### function operator+

```cpp
inline vec3d operator+(
    const vec3d & v
) const
```


### function operator-

```cpp
inline vec3d operator-(
    const vec3d & v
) const
```


### function operator^

```cpp
inline vec3d operator^(
    const vec3d & v
) const
```


### function operator*

```cpp
inline double operator*(
    const vec3d & v
) const
```


### function operator*

```cpp
inline vec3d operator*(
    const double g
) const
```


### function operator/

```cpp
inline vec3d operator/(
    const double g
) const
```


### function operator+=

```cpp
inline const vec3d & operator+=(
    const vec3d & v
)
```


### function operator-=

```cpp
inline const vec3d & operator-=(
    const vec3d & v
)
```


### function operator/=

```cpp
inline const vec3d & operator/=(
    const double f
)
```


### function operator/=

```cpp
inline const vec3d & operator/=(
    const int n
)
```


### function operator*=

```cpp
inline const vec3d & operator*=(
    const double f
)
```


### function operator-

```cpp
inline vec3d operator-() const
```


### function Length

```cpp
inline double Length() const
```


### function SqrLength

```cpp
inline double SqrLength() const
```


### function Normalize

```cpp
inline vec3d & Normalize()
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


-------------------------------

Updated on 2022-01-28 at 07:11:44 +0000