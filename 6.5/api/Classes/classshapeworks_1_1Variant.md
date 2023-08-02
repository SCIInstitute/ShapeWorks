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
| | **[Variant](../Classes/classshapeworks_1_1Variant.md#function-variant)**(std::vector< std::string > v) |
| | **[operator std::string](../Classes/classshapeworks_1_1Variant.md#function-operator-stdstring)**() const |
| | **[operator bool](../Classes/classshapeworks_1_1Variant.md#function-operator-bool)**() const |
| | **[operator int](../Classes/classshapeworks_1_1Variant.md#function-operator-int)**() const |
| | **[operator unsigned int](../Classes/classshapeworks_1_1Variant.md#function-operator-unsigned-int)**() const |
| | **[operator long](../Classes/classshapeworks_1_1Variant.md#function-operator-long)**() const |
| | **[operator unsigned long](../Classes/classshapeworks_1_1Variant.md#function-operator-unsigned-long)**() const |
| | **[operator float](../Classes/classshapeworks_1_1Variant.md#function-operator-float)**() const |
| | **[operator double](../Classes/classshapeworks_1_1Variant.md#function-operator-double)**() const |
| | **[operator std::vector< double >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-double->)**() const |
| | **[operator std::vector< int >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-int->)**() const |
| | **[operator std::vector< bool >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-bool->)**() const |
| | **[operator std::vector< std::string >](../Classes/classshapeworks_1_1Variant.md#function-operator-stdvector<-stdstring->)**() const |

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


### function Variant

```cpp
inline Variant(
    std::vector< std::string > v
)
```


### function operator std::string

```cpp
operator std::string() const
```


### function operator bool

```cpp
operator bool() const
```


### function operator int

```cpp
operator int() const
```


### function operator unsigned int

```cpp
operator unsigned int() const
```


### function operator long

```cpp
operator long() const
```


### function operator unsigned long

```cpp
operator unsigned long() const
```


### function operator float

```cpp
operator float() const
```


### function operator double

```cpp
operator double() const
```


### function operator std::vector< double >

```cpp
operator std::vector< double >() const
```


### function operator std::vector< int >

```cpp
operator std::vector< int >() const
```


### function operator std::vector< bool >

```cpp
operator std::vector< bool >() const
```


### function operator std::vector< std::string >

```cpp
operator std::vector< std::string >() const
```


-------------------------------

Updated on 2023-08-02 at 17:10:00 +0000