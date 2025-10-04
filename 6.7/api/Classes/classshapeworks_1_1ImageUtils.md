---
title: shapeworks::ImageUtils
summary: Helper functions for image. 

---

# shapeworks::ImageUtils



Helper functions for image. 


`#include <ImageUtils.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using itk::ThinPlateSplineKernelTransform< double, 3 > | **[TPSTransform](../Classes/classshapeworks_1_1ImageUtils.md#using-tpstransform)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1ImageUtils.md#function-boundingbox)**(const std::vector< std::string > & filenames, Image::PixelType isoValue =1.0)<br>calculate bounding box for images using the region of data <= the given isoValue  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[boundingBox](../Classes/classshapeworks_1_1ImageUtils.md#function-boundingbox)**(const std::vector< std::reference_wrapper< const [Image](../Classes/classshapeworks_1_1Image.md) > > & images, Image::PixelType isoValue =1.0)<br>calculate bounding box for shapework images using the region of data <= the given isoValue  |
| TPSTransform::Pointer | **[createWarpTransform](../Classes/classshapeworks_1_1ImageUtils.md#function-createwarptransform)**(const std::string & source_landmarks_file, const std::string & target_landmarks_file, const int stride =1) |
| void | **[register_itk_factories](../Classes/classshapeworks_1_1ImageUtils.md#function-register-itk-factories)**() |
| ImageType::Pointer | **[make_axis_aligned](../Classes/classshapeworks_1_1ImageUtils.md#function-make-axis-aligned)**(ImageType::Pointer input) |

## Public Types Documentation

### using TPSTransform

```cpp
using shapeworks::ImageUtils::TPSTransform =  itk::ThinPlateSplineKernelTransform<double, 3>;
```


computes a thin plate spline (TSP) transform from the source to the target landmarks (in the given files) using every stride points 


## Public Functions Documentation

### function boundingBox

```cpp
static PhysicalRegion boundingBox(
    const std::vector< std::string > & filenames,
    Image::PixelType isoValue =1.0
)
```

calculate bounding box for images using the region of data <= the given isoValue 

### function boundingBox

```cpp
static PhysicalRegion boundingBox(
    const std::vector< std::reference_wrapper< const Image > > & images,
    Image::PixelType isoValue =1.0
)
```

calculate bounding box for shapework images using the region of data <= the given isoValue 

### function createWarpTransform

```cpp
static TPSTransform::Pointer createWarpTransform(
    const std::string & source_landmarks_file,
    const std::string & target_landmarks_file,
    const int stride =1
)
```


### function register_itk_factories

```cpp
static void register_itk_factories()
```


### function make_axis_aligned

```cpp
static ImageType::Pointer make_axis_aligned(
    ImageType::Pointer input
)
```


-------------------------------

Updated on 2025-10-04 at 01:42:57 +0000