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
class vtkCleanPolyData;

class MeshGenerator
{
public:
  MeshGenerator();
  ~MeshGenerator();

  void setNeighborhoodSize( int size );
  void setSampleSpacing( double spacing );
  void setUsePowerCrust( bool enabled );

  vtkSmartPointer<vtkPolyData> buildMesh( const vnl_vector<double>& shape );

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
  vtkSmartPointer<vtkCleanPolyData>        cleanPolyData;

  bool usePowerCrust;
};

#endif // ifndef MESH_GENERATOR_H
