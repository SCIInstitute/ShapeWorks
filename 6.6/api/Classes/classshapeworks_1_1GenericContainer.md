---
title: shapeworks::GenericContainer
summary: This class seems to be only a wrapper around std::vector implementing ITK DataObject (for smart pointer?) 

---

# shapeworks::GenericContainer



This class seems to be only a wrapper around std::vector implementing ITK DataObject (for smart pointer?)  [More...](#detailed-description)


`#include <GenericContainer.h>`

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classshapeworks_1_1GenericContainer.md#typedef-datatype)**  |
| typedef [GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md) | **[Self](../Classes/classshapeworks_1_1GenericContainer.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1GenericContainer.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1GenericContainer.md) > | **[Pointer](../Classes/classshapeworks_1_1GenericContainer.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1GenericContainer.md) > | **[ConstPointer](../Classes/classshapeworks_1_1GenericContainer.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1GenericContainer.md) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1GenericContainer.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1GenericContainer.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1GenericContainer.md) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1GenericContainer.md#function-itktypemacro)**(ParticleContainer , itk::DataObject ) |
| T & | **[operator[]](../Classes/classshapeworks_1_1GenericContainer.md#function-operator[])**(size_t k) |
| T & | **[Get](../Classes/classshapeworks_1_1GenericContainer.md#function-get)**(size_t k) |
| unsigned long int | **[GetSize](../Classes/classshapeworks_1_1GenericContainer.md#function-getsize)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md#function-genericcontainer)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1GenericContainer.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~GenericContainer](../Classes/classshapeworks_1_1GenericContainer.md#function-~genericcontainer)**() |

## Detailed Description

```cpp
template <class T >
class shapeworks::GenericContainer;
```

This class seems to be only a wrapper around std::vector implementing ITK DataObject (for smart pointer?) 

One difference being that the set operator will expand the vector if the index is out of bounds. 

## Public Types Documentation

### typedef DataType

```cpp
typedef T shapeworks::GenericContainer< T >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef GenericContainer shapeworks::GenericContainer< T >::Self;
```


### typedef Superclass

```cpp
typedef DataObject shapeworks::GenericContainer< T >::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::GenericContainer< T >::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::GenericContainer< T >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::GenericContainer< T >::ConstWeakPointer;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleContainer ,
    itk::DataObject 
)
```


Run-time type information (and related methods). 


### function operator[]

```cpp
inline T & operator[](
    size_t k
)
```


Returns a reference to the object associated with index k. If the index k does not already exist, this method inserts a new entry for k. 


### function Get

```cpp
inline T & Get(
    size_t k
)
```


Convenience method since all usage of this function is via a pointer, leading ugly syntax like ->operator[](k) 


### function GetSize

```cpp
inline unsigned long int GetSize() const
```


Number of objects in the container. 


## Protected Functions Documentation

### function GenericContainer

```cpp
inline GenericContainer()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~GenericContainer

```cpp
inline virtual ~GenericContainer()
```


-------------------------------

Updated on 2024-04-20 at 15:37:52 +0000