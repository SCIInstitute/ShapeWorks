---
title: Libs/Image/VectorImage.h

---

# Libs/Image/VectorImage.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::VectorImage](../Classes/classshapeworks_1_1VectorImage.md)** <br>[Image](../Classes/classshapeworks_1_1Image.md) composed of vectors instead of just scalars.  |




## Source code

```cpp
#pragma once

#include "Image.h"

#include <itkVectorLinearInterpolateImageFunction.h>
#include <itkGradientImageFilter.h>

namespace shapeworks {

//
// TODO: generalize Image class instead of this:
//       https://github.com/SCIInstitute/ShapeWorks/issues/1053
class VectorImage
{
public:

  using GradientImageFilter = itk::GradientImageFilter<Image::ImageType>;
  using ImageType = itk::Image<Covariant, 3>;
  using GradientInterpolator = itk::VectorLinearInterpolateImageFunction<
    ImageType, typename Image::PixelType>;
  using ImageIterator = itk::ImageRegionIterator<ImageType>;

  VectorImage(const Image& dt_img);
  VectorImage() = delete;
  ~VectorImage() = default;

  Vector evaluate(Point p) { return toVector(interpolator->Evaluate(p)); }
  ImageIterator setIterator();

private:
  itk::SmartPointer<ImageType> image;
  itk::SmartPointer<GradientInterpolator> interpolator;
};

} // shapeworks
```


-------------------------------

Updated on 2022-04-22 at 07:13:30 +0000
