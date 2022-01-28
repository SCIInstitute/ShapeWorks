---
title: itk::ImageToVTKImageFilter
summary: Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline. 

---

# itk::ImageToVTKImageFilter



Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline.  [More...](#detailed-description)


`#include <itkImageToVTKImageFilter.h>`

Inherits from ProcessObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md) | **[Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self)**  |
| typedef ProcessObject | **[Superclass](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-constpointer)**  |
| typedef TInputImage | **[InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype)**  |
| typedef InputImageType::ConstPointer | **[InputImagePointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagepointer)**  |
| typedef VTKImageExport< [InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype) > | **[ExporterFilterType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-exporterfiltertype)**  |
| typedef ExporterFilterType::Pointer | **[ExporterFilterPointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-exporterfilterpointer)**  |
| typedef [ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md) | **[Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self)**  |
| typedef ProcessObject | **[Superclass](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-constpointer)**  |
| typedef TInputImage | **[InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype)**  |
| typedef InputImageType::ConstPointer | **[InputImagePointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagepointer)**  |
| typedef VTKImageExport< [InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype) > | **[ExporterFilterType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-exporterfiltertype)**  |
| typedef ExporterFilterType::Pointer | **[ExporterFilterPointer](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-exporterfilterpointer)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-itktypemacro)**([ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md) , ProcessObject ) |
| vtkImageData * | **[GetOutput](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getoutput)**() const |
| void | **[SetInput](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-setinput)**(const [InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype) * ) |
| vtkImageImport * | **[GetImporter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getimporter)**() const |
| ExporterFilterType * | **[GetExporter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getexporter)**() const |
| void | **[Update](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-update)**() |
| | **[itkNewMacro](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-itktypemacro)**([ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md) , ProcessObject ) |
| vtkImageData * | **[GetOutput](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getoutput)**() const |
| void | **[SetInput](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-setinput)**(const [InputImageType](../Classes/classitk_1_1ImageToVTKImageFilter.md#typedef-inputimagetype) * ) |
| vtkImageImport * | **[GetImporter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getimporter)**() const |
| ExporterFilterType * | **[GetExporter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-getexporter)**() const |
| void | **[Update](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-update)**() |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-imagetovtkimagefilter)**() |
| virtual | **[~ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-~imagetovtkimagefilter)**() |
| | **[ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-imagetovtkimagefilter)**() |
| virtual | **[~ImageToVTKImageFilter](../Classes/classitk_1_1ImageToVTKImageFilter.md#function-~imagetovtkimagefilter)**() |

## Detailed Description

```cpp
template <class TInputImage >
class itk::ImageToVTKImageFilter;
```

Converts an ITK image into a VTK image and plugs a itk data pipeline to a VTK datapipeline. 

This class puts together an itkVTKImageExporter and a vtkImageImporter. It takes care of the details related to the connection of ITK and VTK pipelines. The User will perceive this filter as an adaptor to which an itk::Image can be plugged as input and a vtkImage is produced as output. 

## Public Types Documentation

### typedef Self

```cpp
typedef ImageToVTKImageFilter itk::ImageToVTKImageFilter< TInputImage >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef ProcessObject itk::ImageToVTKImageFilter< TInputImage >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ImageToVTKImageFilter< TInputImage >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ImageToVTKImageFilter< TInputImage >::ConstPointer;
```


### typedef InputImageType

```cpp
typedef TInputImage itk::ImageToVTKImageFilter< TInputImage >::InputImageType;
```


Some typedefs. 


### typedef InputImagePointer

```cpp
typedef InputImageType::ConstPointer itk::ImageToVTKImageFilter< TInputImage >::InputImagePointer;
```


### typedef ExporterFilterType

```cpp
typedef VTKImageExport< InputImageType> itk::ImageToVTKImageFilter< TInputImage >::ExporterFilterType;
```


### typedef ExporterFilterPointer

```cpp
typedef ExporterFilterType::Pointer itk::ImageToVTKImageFilter< TInputImage >::ExporterFilterPointer;
```


### typedef Self

```cpp
typedef ImageToVTKImageFilter itk::ImageToVTKImageFilter< TInputImage >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef ProcessObject itk::ImageToVTKImageFilter< TInputImage >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ImageToVTKImageFilter< TInputImage >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ImageToVTKImageFilter< TInputImage >::ConstPointer;
```


### typedef InputImageType

```cpp
typedef TInputImage itk::ImageToVTKImageFilter< TInputImage >::InputImageType;
```


Some typedefs. 


### typedef InputImagePointer

```cpp
typedef InputImageType::ConstPointer itk::ImageToVTKImageFilter< TInputImage >::InputImagePointer;
```


### typedef ExporterFilterType

```cpp
typedef VTKImageExport< InputImageType> itk::ImageToVTKImageFilter< TInputImage >::ExporterFilterType;
```


### typedef ExporterFilterPointer

```cpp
typedef ExporterFilterType::Pointer itk::ImageToVTKImageFilter< TInputImage >::ExporterFilterPointer;
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
    ImageToVTKImageFilter ,
    ProcessObject 
)
```


Run-time type information (and related methods). 


### function GetOutput

```cpp
vtkImageData * GetOutput() const
```


Get the output in the form of a vtkImage. This call is delegated to the internal vtkImageImporter filter 


### function SetInput

```cpp
void SetInput(
    const InputImageType * 
)
```


Set the input in the form of an itk::Image 


### function GetImporter

```cpp
vtkImageImport * GetImporter() const
```


Return the internal VTK image importer filter. This is intended to facilitate users the access to methods in the importer 


### function GetExporter

```cpp
ExporterFilterType * GetExporter() const
```


Return the internal ITK image exporter filter. This is intended to facilitate users the access to methods in the exporter 


### function Update

```cpp
void Update()
```


This call delegate the update to the importer 


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
    ImageToVTKImageFilter ,
    ProcessObject 
)
```


Run-time type information (and related methods). 


### function GetOutput

```cpp
vtkImageData * GetOutput() const
```


Get the output in the form of a vtkImage. This call is delegated to the internal vtkImageImporter filter 


### function SetInput

```cpp
void SetInput(
    const InputImageType * 
)
```


Set the input in the form of an itk::Image 


### function GetImporter

```cpp
vtkImageImport * GetImporter() const
```


Return the internal VTK image importer filter. This is intended to facilitate users the access to methods in the importer 


### function GetExporter

```cpp
ExporterFilterType * GetExporter() const
```


Return the internal ITK image exporter filter. This is intended to facilitate users the access to methods in the exporter 


### function Update

```cpp
void Update()
```


This call delegate the update to the importer 


## Protected Functions Documentation

### function ImageToVTKImageFilter

```cpp
ImageToVTKImageFilter()
```


### function ~ImageToVTKImageFilter

```cpp
virtual ~ImageToVTKImageFilter()
```


### function ImageToVTKImageFilter

```cpp
ImageToVTKImageFilter()
```


### function ~ImageToVTKImageFilter

```cpp
virtual ~ImageToVTKImageFilter()
```


-------------------------------

Updated on 2022-01-28 at 07:11:43 +0000