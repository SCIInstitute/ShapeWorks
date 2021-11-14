---
title: itk::ParticleContainer

---

# itk::ParticleContainer



 [More...](#detailed-description)

Inherits from DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef T | **[DataType](../Classes/classitk_1_1ParticleContainer.md#typedef-datatype)**  |
| typedef [ParticleContainer](../Classes/classitk_1_1ParticleContainer.md) | **[Self](../Classes/classitk_1_1ParticleContainer.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticleContainer.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleContainer.md) > | **[Pointer](../Classes/classitk_1_1ParticleContainer.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleContainer.md) > | **[ConstPointer](../Classes/classitk_1_1ParticleContainer.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticleContainer.md) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticleContainer.md#typedef-constweakpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleContainer.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleContainer.md) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleContainer.md#function-itktypemacro)**([ParticleContainer](../Classes/classitk_1_1ParticleContainer.md) , DataObject ) |
| T & | **[operator[]](../Classes/classitk_1_1ParticleContainer.md#function-operator[])**(size_t k) |
| T & | **[Get](../Classes/classitk_1_1ParticleContainer.md#function-get)**(size_t k) |
| unsigned long int | **[GetSize](../Classes/classitk_1_1ParticleContainer.md#function-getsize)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleContainer](../Classes/classitk_1_1ParticleContainer.md#function-particlecontainer)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticleContainer.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticleContainer](../Classes/classitk_1_1ParticleContainer.md#function-~particlecontainer)**() |

## Detailed Description

```cpp
template <class T >
class itk::ParticleContainer;
```

## Public Types Documentation

### typedef DataType

```cpp
typedef T itk::ParticleContainer< T >::DataType;
```


Standard class typedefs 


### typedef Self

```cpp
typedef ParticleContainer itk::ParticleContainer< T >::Self;
```


### typedef Superclass

```cpp
typedef DataObject itk::ParticleContainer< T >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleContainer< T >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleContainer< T >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticleContainer< T >::ConstWeakPointer;
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
    DataObject 
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

### function ParticleContainer

```cpp
inline ParticleContainer()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticleContainer

```cpp
inline virtual ~ParticleContainer()
```


-------------------------------

Updated on 2021-11-14 at 17:52:09 +0000