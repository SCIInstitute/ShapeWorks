---
title: Libs/Image/ImageUtils.h

---

# Libs/Image/ImageUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ImageUtils](../Classes/classshapeworks_1_1ImageUtils.md)** <br>Helper functions for image.  |




## Source code

```cpp
#pragma once

#include <itkThinPlateSplineKernelTransform.h>

#include "Image.h"
//#include "ShapeworksUtils.h"

namespace shapeworks {

class ImageUtils {
 public:
  static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, Image::PixelType isoValue = 1.0);

  static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Image>>& images,
                                    Image::PixelType isoValue = 1.0);

  using TPSTransform = itk::ThinPlateSplineKernelTransform<double, 3>;
  static TPSTransform::Pointer createWarpTransform(const std::string& source_landmarks_file,
                                                   const std::string& target_landmarks_file, const int stride = 1);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-01-28 at 22:52:41 +0000
