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
  using ImageType = GradientImageFilter::OutputImageType;
  using GradientInterpolator = itk::VectorLinearInterpolateImageFunction<
    ImageType, typename Image::PixelType>;

  VectorImage(const Image& dt_img);
  VectorImage() = delete;
  ~VectorImage() = default;

  Vector evaluate(Point p) { return toVector(interpolator->Evaluate(p)); }

private:
  itk::SmartPointer<ImageType> image;
  itk::SmartPointer<GradientInterpolator> interpolator;
};

} // shapeworks
```


-------------------------------

Updated on 2022-02-04 at 06:14:24 +0000
