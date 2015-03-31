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

#define SW_USE_POWERCRUST

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
  MeshGenerator();
  ~MeshGenerator();

  void setNeighborhoodSize( int size );
  void setSampleSpacing( double spacing );
  void setUsePowerCrust( bool enabled );
  void setSmoothingAmount( float amount ); // 0-100

  vtkSmartPointer<vtkPolyData> buildMesh( const vnl_vector<double>& shape );

private:

  void updatePipeline();

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

  bool usePowerCrust;
  bool smoothingEnabled;
};

#endif // ifndef MESH_GENERATOR_H
