---
title: Studio/src/Data/StudioMesh.h

---

# Studio/src/Data/StudioMesh.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <itkImage.h>

#include <vnl/vnl_vector.h>

using PixelType = float;
using ImageType = itk::Image<PixelType, 3>;

namespace shapeworks {

class StudioMesh;
using MeshHandle = std::shared_ptr<StudioMesh>;
using MeshList = std::vector<MeshHandle>;



class StudioMesh {
public:

  StudioMesh();

  ~StudioMesh();

  QString get_dimension_string();

  vtkSmartPointer<vtkPolyData> get_poly_data();

  vnl_vector<double> get_center_transform();

  void set_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

  void set_error_message(std::string error_message);

  std::string get_error_message();

  void apply_feature_map(std::string name, ImageType::Pointer image);

  void apply_scalars(MeshHandle mesh);

  void interpolate_scalars_to_mesh(std::string name,
                                   vnl_vector<double> positions, Eigen::VectorXf scalar_values);

private:

  // metadata
  int dimensions_[3];
  vnl_vector<double> center_transform_;

  // the polydata
  vtkSmartPointer<vtkPolyData> poly_data_;

  // error message if the polydata didn't load
  std::string error_message_;

};
}
```


-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000
