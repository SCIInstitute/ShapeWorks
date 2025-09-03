---
title: Libs/Optimize/Utils/OptimizationVisualizer.h

---

# Libs/Optimize/Utils/OptimizationVisualizer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::OptimizationVisualizer](../Classes/classshapeworks_1_1OptimizationVisualizer.md)**  |




## Source code

```cpp
#pragma once

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <string>
#include <vector>

#include "TriMesh.h"
#include "ParticleSystem.h"

namespace shapeworks {
class OptimizationVisualizer {
 public:
  void AddMesh(vtkPolyData* mesh, std::shared_ptr<trimesh::TriMesh> tmesh);
  void IterationCallback(ParticleSystem* particleSystem);

  void SetWireFrame(bool enabled);
  void SetSaveScreenshots(bool enabled, std::string path);

  OptimizationVisualizer() {}
  ~OptimizationVisualizer() {}

 private:
  void initialize();

  std::string screenshotDirectory;
  bool saveScreenshots = false;
  bool wireFrame = false;
  bool colorNormals = false;
  bool initialized = false;

  double* focalPoint;
  int iteration = 0;
  double radius;

  std::vector<vtkSmartPointer<vtkPolyData>> meshes;
  std::vector<std::shared_ptr<trimesh::TriMesh>> tmeshes;

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkRenderer> mainRenderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
  vtkSmartPointer<vtkSphereSource> cubeSource = vtkSmartPointer<vtkSphereSource>::New();

  vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
  vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

  std::vector<vtkSmartPointer<vtkRenderer>> sampleRenderers;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> sampleMappers;
  std::vector<vtkSmartPointer<vtkPoints>> samplePoints;
  std::vector<vtkSmartPointer<vtkPolyData>> samplePolyData;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-03 at 17:42:44 +0000
