---
title: itk::TPGACLevelSetImageFilter

---

# itk::TPGACLevelSetImageFilter



 [More...](#detailed-description)

Inherits from GeodesicActiveContourLevelSetImageFilter< TInputImage, TFeatureImage, float >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md) | **[Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self)**  |
| typedef GeodesicActiveContourLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType > | **[Superclass](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-constpointer)**  |
| typedef TInputImage | **[ImageType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-imagetype)**  |
| typedef ImageType::IndexType | **[IndexType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-indextype)**  |
| typedef Superclass::TimeStepType | **[TimeStepType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-timesteptype)**  |
| typedef Superclass::ValueType | **[ValueType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-valuetype)**  |
| typedef Superclass::OutputImageType | **[OutputImageType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-outputimagetype)**  |
| typedef Superclass::FeatureImageType | **[FeatureImageType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-featureimagetype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-itktypemacro)**([TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md) , GeodesicActiveContourLevelSetImageFilter ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[~TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-~tpgaclevelsetimagefilter)**() |
| | **[TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-tpgaclevelsetimagefilter)**() |
| virtual void | **[PrintSelf](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-printself)**(std::ostream & os, Indent indent) const |
| | **[TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-tpgaclevelsetimagefilter)**(const [Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self) & ) |
| void | **[operator=](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-operator=)**(const [Self](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-self) & ) |
| virtual [ValueType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-valuetype) | **[CalculateUpdateValue](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#function-calculateupdatevalue)**(const IndexType & idx, const TimeStepType & dt, const [ValueType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-valuetype) & value, const [ValueType](../Classes/classitk_1_1TPGACLevelSetImageFilter.md#typedef-valuetype) & change) |

## Detailed Description

```cpp
template <class TInputImage ,
class TFeatureImage ,
class TOutputPixelType  =float>
class itk::TPGACLevelSetImageFilter;
```

## Public Types Documentation

### typedef Self

```cpp
typedef TPGACLevelSetImageFilter itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::Self;
```


Standard class typedefs 


### typedef Superclass

```cpp
typedef GeodesicActiveContourLevelSetImageFilter<TInputImage, TFeatureImage, TOutputPixelType> itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::ConstPointer;
```


### typedef ImageType

```cpp
typedef TInputImage itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::ImageType;
```


### typedef IndexType

```cpp
typedef ImageType::IndexType itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::IndexType;
```


### typedef TimeStepType

```cpp
typedef Superclass::TimeStepType itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::TimeStepType;
```


### typedef ValueType

```cpp
typedef Superclass::ValueType itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::ValueType;
```


Inherited typedef from the superclass. 


### typedef OutputImageType

```cpp
typedef Superclass::OutputImageType itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::OutputImageType;
```


### typedef FeatureImageType

```cpp
typedef Superclass::FeatureImageType itk::TPGACLevelSetImageFilter< TInputImage, TFeatureImage, TOutputPixelType >::FeatureImageType;
```


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory 


### function itkTypeMacro

```cpp
itkTypeMacro(
    TPGACLevelSetImageFilter ,
    GeodesicActiveContourLevelSetImageFilter 
)
```


Run-time type information (and related methods). 


## Protected Functions Documentation

### function ~TPGACLevelSetImageFilter

```cpp
inline ~TPGACLevelSetImageFilter()
```


### function TPGACLevelSetImageFilter

```cpp
TPGACLevelSetImageFilter()
```


### function PrintSelf

```cpp
virtual void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function TPGACLevelSetImageFilter

```cpp
TPGACLevelSetImageFilter(
    const Self & 
)
```


### function operator=

```cpp
void operator=(
    const Self & 
)
```


### function CalculateUpdateValue

```cpp
inline virtual ValueType CalculateUpdateValue(
    const IndexType & idx,
    const TimeStepType & dt,
    const ValueType & value,
    const ValueType & change
)
```


Overridden from the parent class to indroduce a constraint on

* surface flow under certain conditions. 


-------------------------------

Updated on 2022-01-14 at 02:28:13 +0000