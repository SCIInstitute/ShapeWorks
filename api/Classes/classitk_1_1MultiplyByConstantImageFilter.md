---
title: itk::MultiplyByConstantImageFilter
summary: Multiply input pixels by a constant. 

---

# itk::MultiplyByConstantImageFilter



Multiply input pixels by a constant.  [More...](#detailed-description)


`#include <itkMultiplyByConstantImageFilter.h>`

Inherits from UnaryFunctorImageFilter< TInputImage, TOutputImage, Functor::MultiplyByConstant< TInputImage::PixelType, TConstant, TOutputImage::PixelType > >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [MultiplyByConstantImageFilter](../Classes/classitk_1_1MultiplyByConstantImageFilter.md) | **[Self](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-self)**  |
| typedef UnaryFunctorImageFilter< TInputImage, TOutputImage, [Functor::MultiplyByConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md)< typename TInputImage::PixelType, TConstant, typename TOutputImage::PixelType > > | **[Superclass](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-constpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-itktypemacro)**([MultiplyByConstantImageFilter](../Classes/classitk_1_1MultiplyByConstantImageFilter.md) , UnaryFunctorImageFilter ) |
| void | **[SetConstant](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-setconstant)**(TConstant ct) |
| const TConstant & | **[GetConstant](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-getconstant)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[MultiplyByConstantImageFilter](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-multiplybyconstantimagefilter)**() |
| virtual | **[~MultiplyByConstantImageFilter](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-~multiplybyconstantimagefilter)**() |
| void | **[PrintSelf](../Classes/classitk_1_1MultiplyByConstantImageFilter.md#function-printself)**(std::ostream & os, Indent indent) const |

## Detailed Description

```cpp
template <class TInputImage ,
class TConstant ,
class TOutputImage >
class itk::MultiplyByConstantImageFilter;
```

Multiply input pixels by a constant. 

**See**: UnaryFunctorImageFilter 

**Author**: Tom Vercauteren, INRIA & Mauna Kea Technologies

This filter is templated over the input image type and the output image type.


This implementation was taken from the Insight Journal paper: [http://hdl.handle.net/1926/510](http://hdl.handle.net/1926/510)

## Public Types Documentation

### typedef Self

```cpp
typedef MultiplyByConstantImageFilter itk::MultiplyByConstantImageFilter< TInputImage, TConstant, TOutputImage >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef UnaryFunctorImageFilter< TInputImage,TOutputImage, Functor::MultiplyByConstant< typename TInputImage::PixelType, TConstant, typename TOutputImage::PixelType> > itk::MultiplyByConstantImageFilter< TInputImage, TConstant, TOutputImage >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::MultiplyByConstantImageFilter< TInputImage, TConstant, TOutputImage >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::MultiplyByConstantImageFilter< TInputImage, TConstant, TOutputImage >::ConstPointer;
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
    MultiplyByConstantImageFilter ,
    UnaryFunctorImageFilter 
)
```


Run-time type information (and related methods). 


### function SetConstant

```cpp
inline void SetConstant(
    TConstant ct
)
```


Set the constant that will be used to multiply all the image pixels 


### function GetConstant

```cpp
inline const TConstant & GetConstant() const
```


## Protected Functions Documentation

### function MultiplyByConstantImageFilter

```cpp
inline MultiplyByConstantImageFilter()
```


### function ~MultiplyByConstantImageFilter

```cpp
inline virtual ~MultiplyByConstantImageFilter()
```


### function PrintSelf

```cpp
inline void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


-------------------------------

Updated on 2022-01-01 at 17:59:57 +0000