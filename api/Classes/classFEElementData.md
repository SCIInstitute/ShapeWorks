---
title: FEElementData

---

# FEElementData





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FEElementData](../Classes/classFEElementData.md#function-feelementdata)**() |
| | **[FEElementData](../Classes/classFEElementData.md#function-feelementdata)**(const [FEElementData](../Classes/classFEElementData.md) & d) |
| [FEElementData](../Classes/classFEElementData.md) & | **[operator=](../Classes/classFEElementData.md#function-operator=)**(const [FEElementData](../Classes/classFEElementData.md) & d) |
| void | **[Create](../Classes/classFEElementData.md#function-create)**([FEMesh](../Classes/classFEMesh.md) * pm, double v =0.0) |
| int | **[Size](../Classes/classFEElementData.md#function-size)**() |
| double | **[get](../Classes/classFEElementData.md#function-get)**(int i) |
| void | **[set](../Classes/classFEElementData.md#function-set)**(int i, double v) |
| double & | **[operator[]](../Classes/classFEElementData.md#function-operator[])**(int i) |
| void | **[SetName](../Classes/classFEElementData.md#function-setname)**(const char * sz) |
| const char * | **[GetName](../Classes/classFEElementData.md#function-getname)**() |
| void | **[FillRandomBox](../Classes/classFEElementData.md#function-fillrandombox)**(double fmin, double fmax) |

## Public Functions Documentation

### function FEElementData

```cpp
FEElementData()
```


### function FEElementData

```cpp
FEElementData(
    const FEElementData & d
)
```


### function operator=

```cpp
FEElementData & operator=(
    const FEElementData & d
)
```


### function Create

```cpp
void Create(
    FEMesh * pm,
    double v =0.0
)
```


### function Size

```cpp
inline int Size()
```


### function get

```cpp
inline double get(
    int i
)
```


### function set

```cpp
inline void set(
    int i,
    double v
)
```


### function operator[]

```cpp
inline double & operator[](
    int i
)
```


### function SetName

```cpp
void SetName(
    const char * sz
)
```


### function GetName

```cpp
inline const char * GetName()
```


### function FillRandomBox

```cpp
void FillRandomBox(
    double fmin,
    double fmax
)
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000