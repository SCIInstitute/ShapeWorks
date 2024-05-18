---
title: Libs/Image/VectorImage.h

---

# Libs/Image/VectorImage.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::VectorImage](../Classes/classshapeworks_1_1VectorImage.md)** <br>Gradient (vector) image.  |




## Source code

```cpp
#pragma once

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>

#include "Image.h"

namespace shapeworks {

class VectorImage {
 public:
  using GradientImageFilter = itk::GradientImageFilter<Image::ImageType>;
  using ImageType = itk::Image<Covariant, 3>;
  using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<ImageType, Image::PixelType>;
  using ImageIterator = itk::ImageRegionIterator<ImageType>;

  VectorImage(const Image& dt_img);
  VectorImage() = delete;
  ~VectorImage() = default;

  Vector evaluate(Point p);
  ImageIterator iterator();

 private:
  itk::SmartPointer<ImageType> itk_image_;
  itk::SmartPointer<GradientInterpolatorType> interpolator_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-18 at 07:02:36 +0000
