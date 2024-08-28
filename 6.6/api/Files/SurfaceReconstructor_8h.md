---
title: Libs/Analyze/SurfaceReconstructor.h
summary: Surface Reconstruction Layer. 

---

# Libs/Analyze/SurfaceReconstructor.h

Surface [Reconstruction](../Classes/classReconstruction.md) Layer.  [More...](#detailed-description)

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[SurfaceReconstructor](../Classes/classSurfaceReconstructor.md)**  |

## Detailed Description

Surface [Reconstruction](../Classes/classReconstruction.md) Layer. 

The [SurfaceReconstructor](../Classes/classSurfaceReconstructor.md) wraps the surface reconstruction method 




## Source code

```cpp
#pragma once

#include <vector>
#include <string>

#include <itkPoint.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <Eigen/Core>

class SurfaceReconstructorPrivate;

class SurfaceReconstructor {

 public:

  SurfaceReconstructor();
  ~SurfaceReconstructor();

  //**********************************************//
  //************Imported From Studio *************//

  void initializeReconstruction(std::vector<std::vector<itk::Point<double>>> local_pts,
                                std::vector<std::vector<itk::Point<double>>> global_pts,
                                std::vector<std::string> distance_transforms,
                                double maxAngle,
                                float decimationPercent,
                                int numClusters);
  bool hasDenseMean();
  void setMean(vtkSmartPointer<vtkPoints> sparseMean,
               vtkSmartPointer<vtkPolyData> denseMean,
               std::vector<bool> goodPoints);

  void writeMeanInfo(std::string baseName);
  void readMeanInfo(std::string dense,
                    std::string sparse, std::string goodPoints);
  void resetReconstruct();
  //**********************************************//

  void set_number_of_clusters(int num_clusters);
  void set_normal_angle(double angle);
  void set_decimation_percent(double decimation);

  bool get_surface_reconstruction_available();

  vtkSmartPointer<vtkPolyData> build_mesh(const Eigen::VectorXd& shape);

 private:

  std::unique_ptr<SurfaceReconstructorPrivate> private_;

  bool surface_reconstruction_available_ = false;

  int num_clusters_ = -1;
  double decimation_percent_ = 0.3f;
  double normal_angle_ = 45.0f;
};
```


-------------------------------

Updated on 2024-08-28 at 22:41:21 +0000
