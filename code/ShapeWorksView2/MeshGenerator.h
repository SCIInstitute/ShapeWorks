#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include <vtkSmartPointer.h>

#include <MeshCache.h>

//#define SW_USE_POWERCRUST

class CustomSurfaceReconstructionFilter;
class vtkPowerCrustSurfaceReconstruction;
class vtkContourFilter;
class vtkReverseSense;
class vtkSmoothPolyDataFilter;
class vtkPolyDataNormals;
class vtkUnsignedLongArray;
class vtkPoints;
class vtkPolyData;

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

  vtkSmartPointer<CustomSurfaceReconstructionFilter>  surface;
  vtkSmartPointer<vtkPowerCrustSurfaceReconstruction> powercrust;

  vtkSmartPointer<vtkContourFilter>        surfaceContourFilter;
  vtkSmartPointer<vtkReverseSense>         surfaceReverseSense;
  vtkSmartPointer<vtkSmoothPolyDataFilter> surfaceSmoothFilter;
  vtkSmartPointer<vtkPolyDataNormals>      polydataNormals;
  vtkSmartPointer<vtkUnsignedLongArray>    scalars;
  vtkSmartPointer<vtkPoints>               glyphPoints;
  vtkSmartPointer<vtkPolyData>             glyphPointSet;

  // cache of shape meshes
  MeshCache meshCache;

  bool usePowerCrust;
};

#endif // ifndef MESH_GENERATOR_H
