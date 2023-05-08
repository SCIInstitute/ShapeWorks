---
title: shapeworks::ParticlePositionWriter

---

# shapeworks::ParticlePositionWriter



 [More...](#detailed-description)


`#include <itkParticlePositionWriter.h>`

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticlePositionWriter](../Classes/classshapeworks_1_1ParticlePositionWriter.md) | **[Self](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-constweakpointer)**  |
| typedef itk::Point< double, 3 > | **[PointType](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-itktypemacro)**([ParticlePositionWriter](../Classes/classshapeworks_1_1ParticlePositionWriter.md) , itk::DataObject ) |
| void | **[SetInput](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-setinput)**(const std::vector< [PointType](../Classes/classshapeworks_1_1ParticlePositionWriter.md#typedef-pointtype) > & p) |
| | **[itkSetStringMacro](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-itksetstringmacro)**(FileName ) |
| | **[itkGetStringMacro](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-itkgetstringmacro)**(FileName ) |
| void | **[Write](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-write)**() |
| void | **[Update](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-update)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePositionWriter](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-particlepositionwriter)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ParticlePositionWriter](../Classes/classshapeworks_1_1ParticlePositionWriter.md#function-~particlepositionwriter)**() |

## Detailed Description

```cpp
class shapeworks::ParticlePositionWriter;
```


This class writes a set of Points to disk. Its input is a std::vector of points. The file format is simple an ascii list of VDimension-tuples stored one per line (delimited by std::endl). There is no header required for the file, but the file should not contain leading or trailing empty lines.

In 3D, for example, a fragment of a points file looks like this:

1.0 2.0 5.4 2.3 8.7 33.0 4.0 8.21 4.44

etc.. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticlePositionWriter shapeworks::ParticlePositionWriter::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::ParticlePositionWriter::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticlePositionWriter::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticlePositionWriter::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticlePositionWriter::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef itk::Point<double, 3> shapeworks::ParticlePositionWriter::PointType;
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
    itk::DataObject 
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
inline void Update()
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
    itk::Indent indent
) const
```


### function ~ParticlePositionWriter

```cpp
inline virtual ~ParticlePositionWriter()
```


-------------------------------

Updated on 2023-05-08 at 20:57:58 +0000