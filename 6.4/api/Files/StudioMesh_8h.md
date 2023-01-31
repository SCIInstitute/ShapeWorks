---
title: Libs/Analyze/StudioMesh.h

---

# Libs/Analyze/StudioMesh.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioMesh](../Classes/classshapeworks_1_1StudioMesh.md)** <br>Representation of a single mesh.  |

## Types

|                | Name           |
| -------------- | -------------- |
| using float | **[PixelType](../Files/StudioMesh_8h.md#using-pixeltype)**  |
| using itk::Image< PixelType, 3 > | **[ImageType](../Files/StudioMesh_8h.md#using-imagetype)**  |

## Types Documentation

### using PixelType

```cpp
using PixelType =  float;
```


### using ImageType

```cpp
using ImageType =  itk::Image<PixelType, 3>;
```





## Source code

```cpp
#pragma once

#include <itkImage.h>
#include <vnl/vnl_vector.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#include <Eigen/Dense>
#include <Eigen/Sparse>

using PixelType = float;
using ImageType = itk::Image<PixelType, 3>;
class vtkFloatArray;
class vtkStaticPointLocator;

namespace shapeworks {

class StudioMesh;
using MeshHandle = std::shared_ptr<StudioMesh>;
using MeshList = std::vector<MeshHandle>;


class StudioMesh {
 public:
  StudioMesh();

  ~StudioMesh();

  vtkSmartPointer<vtkPolyData> get_poly_data();

  void set_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

  void set_error_message(std::string error_message);

  std::string get_error_message();

  void apply_feature_map(std::string name, ImageType::Pointer image);

  void apply_scalars(MeshHandle mesh);

  void interpolate_scalars_to_mesh(std::string name, Eigen::VectorXd positions, Eigen::VectorXf scalar_values);

  double get_largest_dimension_size();

  vtkFloatArray* get_or_create_array(std::string name, float default_value);

  void paint_ffc(double world_pos[], double radius, bool inclusive);

  bool has_ffc_paint();

  static constexpr const char* const FFC_PAINT = "ffc_paint";

 private:
  vtkSmartPointer<vtkPolyData> poly_data_;

  vtkSmartPointer<vtkStaticPointLocator> locator_;

  // error message if the polydata didn't load
  std::string error_message_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-01-31 at 20:06:22 +0000
