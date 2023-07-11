---
title: shapeworks::ParticlePositionReader

---

# shapeworks::ParticlePositionReader



 [More...](#detailed-description)


`#include <itkParticlePositionReader.h>`

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticlePositionReader](../Classes/classshapeworks_1_1ParticlePositionReader.md) | **[Self](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-superclass)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-constpointer)**  |
| typedef itk::WeakPointer< const [Self](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-self) > | **[ConstWeakPointer](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-constweakpointer)**  |
| typedef itk::Point< double, 3 > | **[PointType](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-itktypemacro)**([ParticlePositionReader](../Classes/classshapeworks_1_1ParticlePositionReader.md) , itk::DataObject ) |
| const std::vector< [PointType](../Classes/classshapeworks_1_1ParticlePositionReader.md#typedef-pointtype) > & | **[GetOutput](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-getoutput)**() const |
| | **[itkSetStringMacro](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-itksetstringmacro)**(FileName ) |
| | **[itkGetStringMacro](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-itkgetstringmacro)**(FileName ) |
| void | **[Read](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-read)**() |
| void | **[Update](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-update)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePositionReader](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-particlepositionreader)**() |
| void | **[PrintSelf](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-printself)**(std::ostream & os, itk::Indent indent) const |
| virtual | **[~ParticlePositionReader](../Classes/classshapeworks_1_1ParticlePositionReader.md#function-~particlepositionreader)**() |

## Detailed Description

```cpp
class shapeworks::ParticlePositionReader;
```


This class reads a set of Points from disk and stores them in a vector. The file format is simple an ascii list of VDimension-tuples stored one per line (delimited by std::endl). There is no header required for the file, but the file should not contain leading or trailing empty lines.

In 3D, for example, a fragment of a points file looks like this:

1.0 2.0 5.4 2.3 8.7 33.0 4.0 8.21 4.44

etc.. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticlePositionReader shapeworks::ParticlePositionReader::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject shapeworks::ParticlePositionReader::Superclass;
```


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticlePositionReader::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticlePositionReader::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef itk::WeakPointer<const Self> shapeworks::ParticlePositionReader::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef itk::Point<double, 3> shapeworks::ParticlePositionReader::PointType;
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
    ParticlePositionReader ,
    itk::DataObject 
)
```


Run-time type information (and related methods). 


### function GetOutput

```cpp
inline const std::vector< PointType > & GetOutput() const
```


Get the output of the reader. The output is a std::vector of PointType. 


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


### function Read

```cpp
inline void Read()
```


Read the file. 


### function Update

```cpp
inline void Update()
```


## Protected Functions Documentation

### function ParticlePositionReader

```cpp
inline ParticlePositionReader()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    itk::Indent indent
) const
```


### function ~ParticlePositionReader

```cpp
inline virtual ~ParticlePositionReader()
```


-------------------------------

Updated on 2023-07-11 at 16:40:30 +0000