---
title: FEFillHole::EdgeRing

---

# FEFillHole::EdgeRing





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md#function-edgering)**() |
| | **[EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md#function-edgering)**(const [EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ring) |
| void | **[operator=](../Classes/classFEFillHole_1_1EdgeRing.md#function-operator=)**(const [EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ring) |
| bool | **[empty](../Classes/classFEFillHole_1_1EdgeRing.md#function-empty)**() |
| void | **[add](../Classes/classFEFillHole_1_1EdgeRing.md#function-add)**(int n, const [vec3d](../Classes/classvec3d.md) & r, const [vec3d](../Classes/classvec3d.md) & nn) |
| void | **[clear](../Classes/classFEFillHole_1_1EdgeRing.md#function-clear)**() |
| int | **[size](../Classes/classFEFillHole_1_1EdgeRing.md#function-size)**() |
| int | **[operator[]](../Classes/classFEFillHole_1_1EdgeRing.md#function-operator[])**(int i) |
| void | **[GetLeftEar](../Classes/classFEFillHole_1_1EdgeRing.md#function-getleftear)**(int n0, int n1, [EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ear) |
| void | **[GetRightEar](../Classes/classFEFillHole_1_1EdgeRing.md#function-getrightear)**(int n0, int n1, [EdgeRing](../Classes/classFEFillHole_1_1EdgeRing.md) & ear) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| vector< [vec3d](../Classes/classvec3d.md) > | **[m_r](../Classes/classFEFillHole_1_1EdgeRing.md#variable-m-r)**  |
| vector< int > | **[m_node](../Classes/classFEFillHole_1_1EdgeRing.md#variable-m-node)**  |
| int | **[m_winding](../Classes/classFEFillHole_1_1EdgeRing.md#variable-m-winding)**  |
| vector< [vec3d](../Classes/classvec3d.md) > | **[m_normal](../Classes/classFEFillHole_1_1EdgeRing.md#variable-m-normal)**  |

## Public Functions Documentation

### function EdgeRing

```cpp
inline EdgeRing()
```


### function EdgeRing

```cpp
inline EdgeRing(
    const EdgeRing & ring
)
```


### function operator=

```cpp
inline void operator=(
    const EdgeRing & ring
)
```


### function empty

```cpp
inline bool empty()
```


### function add

```cpp
inline void add(
    int n,
    const vec3d & r,
    const vec3d & nn
)
```


### function clear

```cpp
inline void clear()
```


### function size

```cpp
inline int size()
```


### function operator[]

```cpp
inline int operator[](
    int i
)
```


### function GetLeftEar

```cpp
void GetLeftEar(
    int n0,
    int n1,
    EdgeRing & ear
)
```


### function GetRightEar

```cpp
void GetRightEar(
    int n0,
    int n1,
    EdgeRing & ear
)
```


## Public Attributes Documentation

### variable m_r

```cpp
vector< vec3d > m_r;
```


### variable m_node

```cpp
vector< int > m_node;
```


### variable m_winding

```cpp
int m_winding;
```


### variable m_normal

```cpp
vector< vec3d > m_normal;
```


-------------------------------

Updated on 2021-12-13 at 17:42:53 +0000