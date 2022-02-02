---
title: Studio/src/Data/LegacyMeshGenerator.h
summary: Mesh generation. 

---

# Studio/src/Data/LegacyMeshGenerator.h

Mesh generation.  [More...](#detailed-description)

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[LegacyMeshGenerator](../Classes/classLegacyMeshGenerator.md)**  |

## Detailed Description

Mesh generation. 

The [LegacyMeshGenerator](../Classes/classLegacyMeshGenerator.md) performs the actual work of reconstructing a mesh from a shape (list of points). 




## Source code

```cpp
/*
 * Shapeworks license
 */

#pragma once

#include "vnl/vnl_vector.h"

#include <vtkSmartPointer.h>
#include <QSharedPointer>
#include <Eigen/Eigen>

//#include <SurfaceReconstructor.h>

//#define SW_USE_POWERCRUST

class CustomSurfaceReconstructionFilter;
class vtkPowerCrustSurfaceReconstruction;
class vtkContourFilter;
class vtkReverseSense;
class vtkSmoothPolyDataFilter;
class vtkPolyDataNormals;
class vtkPoints;
class vtkPolyData;
class vtkTriangleFilter;
class vtkWindowedSincPolyDataFilter;
class vtkPolyDataToImageData;

class LegacyMeshGenerator
{
public:
  LegacyMeshGenerator();
  ~LegacyMeshGenerator();

  void setNeighborhoodSize( int size );
  void setSampleSpacing( double spacing );
  void setUsePowerCrust( bool enabled );
  void setSmoothingAmount( float amount ); // 0-100

  vtkSmartPointer<vtkPolyData> buildMesh( const Eigen::VectorXd& shape );

  //void set_surface_reconstructor(QSharedPointer<SurfaceReconstructor> reconstructor);

private:

  void updatePipeline();

  vtkSmartPointer<CustomSurfaceReconstructionFilter>  surfaceReconstruction;
  vtkSmartPointer<vtkPowerCrustSurfaceReconstruction> powercrust;

  vtkSmartPointer<vtkPoints>               points;
  vtkSmartPointer<vtkPolyData>             pointSet;
  vtkSmartPointer<vtkContourFilter>        contourFilter;
  vtkSmartPointer<vtkReverseSense>         reverseSense;
  vtkSmartPointer<vtkPolyDataNormals>      polydataNormals;
  vtkSmartPointer<vtkTriangleFilter>       triangleFilter;
  vtkSmartPointer<vtkWindowedSincPolyDataFilter> windowSincFilter;
  vtkSmartPointer<vtkPolyDataToImageData>  polydataToImageData;

  bool usePowerCrust;
  bool smoothingEnabled;

  //QSharedPointer<SurfaceReconstructor> surface_reconstructor_;
};
```


-------------------------------

Updated on 2022-02-02 at 18:18:10 +0000
