---
title: shapeworks::VectorImage
summary: Gradient (vector) image. 

---

# shapeworks::VectorImage



Gradient (vector) image. 


`#include <VectorImage.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::GradientImageFilter< Image::ImageType > | **[GradientImageFilter](../Classes/classshapeworks_1_1VectorImage.md#using-gradientimagefilter)**  |
| using itk::Image< Covariant, 3 > | **[ImageType](../Classes/classshapeworks_1_1VectorImage.md#using-imagetype)**  |
| using itk::VectorLinearInterpolateImageFunction< ImageType, Image::PixelType > | **[GradientInterpolatorType](../Classes/classshapeworks_1_1VectorImage.md#using-gradientinterpolatortype)**  |
| using itk::ImageRegionIterator< ImageType > | **[ImageIterator](../Classes/classshapeworks_1_1VectorImage.md#using-imageiterator)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-vectorimage)**(const [Image](../Classes/classshapeworks_1_1Image.md) & dt_img)<br>Creates a gradient vector image of image (presumably a distance transform)  |
| | **[VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-vectorimage)**() =delete |
| | **[~VectorImage](../Classes/classshapeworks_1_1VectorImage.md#function-~vectorimage)**() =default |
| Vector | **[evaluate](../Classes/classshapeworks_1_1VectorImage.md#function-evaluate)**(Point p)<br>Returns a Vector (which can be normalized using `v.Normalize()`).  |
| ImageIterator | **[iterator](../Classes/classshapeworks_1_1VectorImage.md#function-iterator)**() |

## Public Types Documentation

### using GradientImageFilter

```cpp
using shapeworks::VectorImage::GradientImageFilter =  itk::GradientImageFilter<Image::ImageType>;
```


### using ImageType

```cpp
using shapeworks::VectorImage::ImageType =  itk::Image<Covariant, 3>;
```


### using GradientInterpolatorType

```cpp
using shapeworks::VectorImage::GradientInterpolatorType =  itk::VectorLinearInterpolateImageFunction<ImageType, Image::PixelType>;
```


### using ImageIterator

```cpp
using shapeworks::VectorImage::ImageIterator =  itk::ImageRegionIterator<ImageType>;
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
Vector evaluate(
    Point p
)
```

Returns a Vector (which can be normalized using `v.Normalize()`). 

### function iterator

```cpp
ImageIterator iterator()
```


-------------------------------

Updated on 2022-09-13 at 16:52:35 +0000