/*
 * Shapeworks license
 */

/**
 * @file MeshGenerator.h
 * @brief Mesh generation
 *
 * The MeshGenerator performs the actual work of reconstructing
 * a mesh from a shape (list of points).
 */

#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include "vnl/vnl_vector.h"

#include <vtkSmartPointer.h>
#include <Application/Preferences.h>

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

class MeshGenerator
{
public:
  MeshGenerator(Preferences& prefs);
  ~MeshGenerator();

  vtkSmartPointer<vtkPolyData> buildMesh( const vnl_vector<double>& shape );
  void updatePipeline();

private:


  vtkSmartPointer<CustomSurfaceReconstructionFilter>  surfaceReconstruction;
  vtkSmartPointer<vtkPowerCrustSurfaceReconstruction> powercrust;

  vtkSmartPointer<vtkPoints>               points;
  vtkSmartPointer<vtkPolyData>             pointSet;
  vtkSmartPointer<vtkContourFilter>        contourFilter;
  vtkSmartPointer<vtkReverseSense>         reverseSense;
  vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  vtkSmartPointer<vtkPolyDataNormals>      polydataNormals;
  vtkSmartPointer<vtkTriangleFilter>       triangleFilter;
  vtkSmartPointer<vtkWindowedSincPolyDataFilter> windowSincFilter;
  vtkSmartPointer<vtkPolyDataToImageData>  polydataToImageData;

  Preferences& prefs_;
};

#endif // ifndef MESH_GENERATOR_H
