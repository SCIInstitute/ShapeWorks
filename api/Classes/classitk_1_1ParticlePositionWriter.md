---
title: itk::ParticlePositionWriter

---

# itk::ParticlePositionWriter



 [More...](#detailed-description)


`#include <itkParticlePositionWriter.h>`

Inherits from DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticlePositionWriter](../Classes/classitk_1_1ParticlePositionWriter.md) | **[Self](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-constweakpointer)**  |
| typedef Point< double, VDimension > | **[PointType](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticlePositionWriter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticlePositionWriter.md#function-itktypemacro)**([ParticlePositionWriter](../Classes/classitk_1_1ParticlePositionWriter.md) , DataObject ) |
| void | **[SetInput](../Classes/classitk_1_1ParticlePositionWriter.md#function-setinput)**(const std::vector< [PointType](../Classes/classitk_1_1ParticlePositionWriter.md#typedef-pointtype) > & p) |
| | **[itkSetStringMacro](../Classes/classitk_1_1ParticlePositionWriter.md#function-itksetstringmacro)**(FileName ) |
| | **[itkGetStringMacro](../Classes/classitk_1_1ParticlePositionWriter.md#function-itkgetstringmacro)**(FileName ) |
| void | **[Write](../Classes/classitk_1_1ParticlePositionWriter.md#function-write)**() |
| void | **[Update](../Classes/classitk_1_1ParticlePositionWriter.md#function-update)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePositionWriter](../Classes/classitk_1_1ParticlePositionWriter.md#function-particlepositionwriter)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticlePositionWriter.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticlePositionWriter](../Classes/classitk_1_1ParticlePositionWriter.md#function-~particlepositionwriter)**() |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticlePositionWriter;
```


This class writes a set of Points to disk. Its input is a std::vector of points. The file format is simple an ascii list of VDimension-tuples stored one per line (delimited by std::endl). There is no header required for the file, but the file should not contain leading or trailing empty lines.

In 3D, for example, a fragment of a points file looks like this:

1.0 2.0 5.4 2.3 8.7 33.0 4.0 8.21 4.44

etc.. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticlePositionWriter itk::ParticlePositionWriter< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject itk::ParticlePositionWriter< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticlePositionWriter< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticlePositionWriter< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticlePositionWriter< VDimension >::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef Point<double, VDimension> itk::ParticlePositionWriter< VDimension >::PointType;
```


The point type that this class reads. 


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
    ParticlePositionWriter ,
    DataObject 
)
```


Run-time type information (and related methods). 


### function SetInput

```cpp
inline void SetInput(
    const std::vector< PointType > & p
)
```


Set the input vector. The input is a reference to a std::vector of PointType. 


### function itkSetStringMacro

```cpp
itkSetStringMacro(
    FileName 
)
```


Set/Get the filename. 


### function itkGetStringMacro

```cpp
itkGetStringMacro(
    FileName 
)
```


### function Write

```cpp
inline void Write()
```


Write the file. 


### function Update

```cpp
void Update()
```


## Protected Functions Documentation

### function ParticlePositionWriter

```cpp
inline ParticlePositionWriter()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ~ParticlePositionWriter

```cpp
inline virtual ~ParticlePositionWriter()
```


-------------------------------

Updated on 2022-01-28 at 21:13:52 +0000