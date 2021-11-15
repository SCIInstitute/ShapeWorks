---
title: mat3d

---

# mat3d





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[mat3d](../Classes/classmat3d.md#function-mat3d)**() |
| | **[mat3d](../Classes/classmat3d.md#function-mat3d)**(double a00, double a01, double a02, double a10, double a11, double a12, double a20, double a21, double a22) |
| double * | **[operator[]](../Classes/classmat3d.md#function-operator[])**(int i) |
| double & | **[operator()](../Classes/classmat3d.md#function-operator())**(int i, int j) |
| [mat3d](../Classes/classmat3d.md) | **[operator*](../Classes/classmat3d.md#function-operator*)**([mat3d](../Classes/classmat3d.md) & m) |
| [mat3d](../Classes/classmat3d.md) & | **[operator*=](../Classes/classmat3d.md#function-operator*=)**([mat3d](../Classes/classmat3d.md) & m) |
| [mat3d](../Classes/classmat3d.md) & | **[operator+=](../Classes/classmat3d.md#function-operator+=)**([mat3d](../Classes/classmat3d.md) & m) |
| [mat3d](../Classes/classmat3d.md) & | **[operator-=](../Classes/classmat3d.md#function-operator-=)**([mat3d](../Classes/classmat3d.md) & m) |
| [mat3d](../Classes/classmat3d.md) & | **[operator/=](../Classes/classmat3d.md#function-operator/=)**(const double f) |
| [vec3d](../Classes/classvec3d.md) | **[operator*](../Classes/classmat3d.md#function-operator*)**([vec3d](../Classes/classvec3d.md) b) |
| double | **[det](../Classes/classmat3d.md#function-det)**() const |
| double | **[Invert](../Classes/classmat3d.md#function-invert)**() |
| [mat3d](../Classes/classmat3d.md) | **[inverse](../Classes/classmat3d.md#function-inverse)**() const |
| void | **[zero](../Classes/classmat3d.md#function-zero)**() |
| void | **[unit](../Classes/classmat3d.md#function-unit)**() |
| [mat3d](../Classes/classmat3d.md) | **[transpose](../Classes/classmat3d.md#function-transpose)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_data](../Classes/classmat3d.md#variable-m-data)**  |

## Public Functions Documentation

### function mat3d

```cpp
inline mat3d()
```


### function mat3d

```cpp
mat3d(
    double a00,
    double a01,
    double a02,
    double a10,
    double a11,
    double a12,
    double a20,
    double a21,
    double a22
)
```


### function operator[]

```cpp
inline double * operator[](
    int i
)
```


### function operator()

```cpp
inline double & operator()(
    int i,
    int j
)
```


### function operator*

```cpp
inline mat3d operator*(
    mat3d & m
)
```


### function operator*=

```cpp
inline mat3d & operator*=(
    mat3d & m
)
```


### function operator+=

```cpp
inline mat3d & operator+=(
    mat3d & m
)
```


### function operator-=

```cpp
inline mat3d & operator-=(
    mat3d & m
)
```


### function operator/=

```cpp
inline mat3d & operator/=(
    const double f
)
```


### function operator*

```cpp
inline vec3d operator*(
    vec3d b
)
```


### function det

```cpp
inline double det() const
```


### function Invert

```cpp
double Invert()
```


### function inverse

```cpp
mat3d inverse() const
```


### function zero

```cpp
inline void zero()
```


### function unit

```cpp
inline void unit()
```


### function transpose

```cpp
mat3d transpose()
```


## Protected Attributes Documentation

### variable m_data

```cpp
double m_data;
```


-------------------------------

Updated on 2021-11-15 at 01:30:08 +0000