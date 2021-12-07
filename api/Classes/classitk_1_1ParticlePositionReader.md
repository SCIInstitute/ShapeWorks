---
title: itk::ParticlePositionReader

---

# itk::ParticlePositionReader



 [More...](#detailed-description)


`#include <itkParticlePositionReader.h>`

Inherits from DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticlePositionReader](../Classes/classitk_1_1ParticlePositionReader.md) | **[Self](../Classes/classitk_1_1ParticlePositionReader.md#typedef-self)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticlePositionReader.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticlePositionReader.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticlePositionReader.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticlePositionReader.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticlePositionReader.md#typedef-constpointer)**  |
| typedef WeakPointer< const [Self](../Classes/classitk_1_1ParticlePositionReader.md#typedef-self) > | **[ConstWeakPointer](../Classes/classitk_1_1ParticlePositionReader.md#typedef-constweakpointer)**  |
| typedef Point< double, VDimension > | **[PointType](../Classes/classitk_1_1ParticlePositionReader.md#typedef-pointtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ParticlePositionReader.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticlePositionReader.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticlePositionReader.md#function-itktypemacro)**([ParticlePositionReader](../Classes/classitk_1_1ParticlePositionReader.md) , DataObject ) |
| const std::vector< [PointType](../Classes/classitk_1_1ParticlePositionReader.md#typedef-pointtype) > & | **[GetOutput](../Classes/classitk_1_1ParticlePositionReader.md#function-getoutput)**() const |
| | **[itkSetStringMacro](../Classes/classitk_1_1ParticlePositionReader.md#function-itksetstringmacro)**(FileName ) |
| | **[itkGetStringMacro](../Classes/classitk_1_1ParticlePositionReader.md#function-itkgetstringmacro)**(FileName ) |
| void | **[Read](../Classes/classitk_1_1ParticlePositionReader.md#function-read)**() |
| void | **[Update](../Classes/classitk_1_1ParticlePositionReader.md#function-update)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticlePositionReader](../Classes/classitk_1_1ParticlePositionReader.md#function-particlepositionreader)**() |
| void | **[PrintSelf](../Classes/classitk_1_1ParticlePositionReader.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual | **[~ParticlePositionReader](../Classes/classitk_1_1ParticlePositionReader.md#function-~particlepositionreader)**() |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticlePositionReader;
```


This class reads a set of Points from disk and stores them in a vector. The file format is simple an ascii list of VDimension-tuples stored one per line (delimited by std::endl). There is no header required for the file, but the file should not contain leading or trailing empty lines.

In 3D, for example, a fragment of a points file looks like this:

1.0 2.0 5.4 2.3 8.7 33.0 4.0 8.21 4.44

etc.. 

## Public Types Documentation

### typedef Self

```cpp
typedef ParticlePositionReader itk::ParticlePositionReader< VDimension >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef DataObject itk::ParticlePositionReader< VDimension >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticlePositionReader< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticlePositionReader< VDimension >::ConstPointer;
```


### typedef ConstWeakPointer

```cpp
typedef WeakPointer<const Self> itk::ParticlePositionReader< VDimension >::ConstWeakPointer;
```


### typedef PointType

```cpp
typedef Point<double, VDimension> itk::ParticlePositionReader< VDimension >::PointType;
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
    DataObject 
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
void Update()
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
    Indent indent
) const
```


### function ~ParticlePositionReader

```cpp
inline virtual ~ParticlePositionReader()
```


-------------------------------

Updated on 2021-12-07 at 17:10:23 +0000