---
title: shapeworks::VectorImage
summary: Image composed of vectors instead of just scalars. 

---

# shapeworks::VectorImage



[Image](../Classes/classshapeworks_1_1Image.md) composed of vectors instead of just scalars. 


`#include <VectorImage.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::GradientImageFilter< Image::ImageType > | **[GradientImageFilter](../Classes/classshapeworks_1_1VectorImage.md#using-gradientimagefilter)**  |
| using GradientImageFilter::OutputImageType | **[ImageType](../Classes/classshapeworks_1_1VectorImage.md#using-imagetype)**  |
| using itk::VectorLinearInterpolateImageFunction< ImageType, typename Image::PixelType > | **[GradientInterpolator](../Classes/classshapeworks_1_1VectorImage.md#using-gradientinterpolator)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-vectorimage)**(const [Image](../Classes/classshapeworks_1_1Image.md) & dt_img)<br>Creates a gradient vector image of image (presumably a distance transform)  |
| | **[VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-vectorimage)**() =delete |
| | **[~VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-~vectorimage)**() =default |
| Vector | **[evaluate](../Classes/classshapeworks_1_1VectorImage.md#function-evaluate)**(Point p)<br>Returns a Vector (which can be normalized using `v.Normalize()`).  |

## Public Types Documentation

### using GradientImageFilter

```cpp
using shapeworks::VectorImage::GradientImageFilter =  itk::GradientImageFilter<Image::ImageType>;
```


### using ImageType

```cpp
using shapeworks::VectorImage::ImageType =  GradientImageFilter::OutputImageType;
```


### using GradientInterpolator

```cpp
using shapeworks::VectorImage::GradientInterpolator =  itk::VectorLinearInterpolateImageFunction< ImageType, typename Image::PixelType>;
```


## Public Functions Documentation

### function VectorImage

```cpp
VectorImage(
    const Image & dt_img
)
```

Creates a gradient vector image of image (presumably a distance transform) 

### function VectorImage

```cpp
VectorImage() =delete
```


### function ~VectorImage

```cpp
~VectorImage() =default
```


### function evaluate

```cpp
inline Vector evaluate(
    Point p
)
```

Returns a Vector (which can be normalized using `v.Normalize()`). 

-------------------------------

Updated on 2022-01-12 at 19:06:14 +0000