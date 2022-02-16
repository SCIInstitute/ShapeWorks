---
title: shapeworks::Variant
summary: Variant class to represent multiple types. 

---

# shapeworks::Variant



[Variant]() class to represent multiple types.  [More...](#detailed-description)


`#include <Variant.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**() |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(const std::string & v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(int v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(double v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(const char * v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(bool v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(std::vector< double > v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(std::vector< int > v) |
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(std::vector< bool > v) |
| | **[operator std::string](../Classes/classshapeworks_1_1Variant.md#function-operator-stdstring)**() |
| | **[operator bool](../Classes/classshapeworks_1_1Variant.md#function-operator-bool)**() |
| | **[operator int](../Classes/classshapeworks_1_1Variant.md#function-operator-int)**() |
| | **[operator unsigned int](../Classes/classshapeworks_1_1Variant.md#function-operator-unsigned-int)**() |
| | **[operator long](../Classes/classshapeworks_1_1Variant.md#function-operator-long)**() |
| | **[operator unsigned long](../Classes/classshapeworks_1_1Variant.md#function-operator-unsigned-long)**() |
| | **[operator float](../Classes/classshapeworks_1_1Variant.md#function-operator-float)**() |
| | **[operator double](../Classes/classshapeworks_1_1Variant.md#function-operator-double)**() |
| | **[operator std::vector< double >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-double->)**() |
| | **[operator std::vector< int >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-int->)**() |
| | **[operator std::vector< bool >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-bool->)**() |

## Detailed Description

```cpp
class shapeworks::Variant;
```

[Variant]() class to represent multiple types. 

This class was adapted from optparse to contain multiple types as a string This is used for storing parameters in a spreadsheet 

## Public Functions Documentation

### function Variant

```cpp
inline Variant()
```


### function Variant

```cpp
inline Variant(
    const std::string & v
)
```


### function Variant

```cpp
inline Variant(
    int v
)
```


### function Variant

```cpp
inline Variant(
    double v
)
```


### function Variant

```cpp
inline Variant(
    const char * v
)
```


### function Variant

```cpp
inline Variant(
    bool v
)
```


### function Variant

```cpp
inline Variant(
    std::vector< double > v
)
```


### function Variant

```cpp
inline Variant(
    std::vector< int > v
)
```


### function Variant

```cpp
inline Variant(
    std::vector< bool > v
)
```


### function operator std::string

```cpp
operator std::string()
```


### function operator bool

```cpp
operator bool()
```


### function operator int

```cpp
operator int()
```


### function operator unsigned int

```cpp
operator unsigned int()
```


### function operator long

```cpp
operator long()
```


### function operator unsigned long

```cpp
operator unsigned long()
```


### function operator float

```cpp
operator float()
```


### function operator double

```cpp
operator double()
```


### function operator std::vector< double >

```cpp
operator std::vector< double >()
```


### function operator std::vector< int >

```cpp
operator std::vector< int >()
```


### function operator std::vector< bool >

```cpp
operator std::vector< bool >()
```


-------------------------------

Updated on 2022-02-16 at 06:47:28 +0000