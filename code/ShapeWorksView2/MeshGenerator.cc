/*
 * Shapeworks license
 */

#include <MeshGenerator.h>

// vtk
#include <vtkContourFilter.h>
#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>
#include <vtkReverseSense.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkUnsignedLongArray.h>
#include <vtkTriangleFilter.h>
#include <vtkCleanPolyData.h>

#include "CustomSurfaceReconstructionFilter.h"

// local files
#ifdef SW_USE_POWERCRUST
#include "vtkPowerCrustSurfaceReconstruction.h"
#endif

MeshGenerator::MeshGenerator()
{
  this->usePowerCrust = false;

  this->points = vtkSmartPointer<vtkPoints>::New();
  this->points->SetDataTypeToDouble();
  this->pointSet = vtkSmartPointer<vtkPolyData>::New();
  this->pointSet->SetPoints( this->points );

  this->surfaceReconstruction = vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
  this->surfaceReconstruction->SetInput( this->pointSet );

#ifdef SW_USE_POWERCRUST
  this->powercrust = vtkSmartPointer<vtkPowerCrustSurfaceReconstruction>::New();
  this->powercrust->SetInput( this->pointSet );

/*
  this->triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  this->triangleFilter->SetInputConnection( this->powercrust->GetOutputPort() );
  //this->triangleFilter->SetInputConnection( this->cleanPolyData->GetOutputPort() );

  this->cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
  this->cleanPolyData->SetInputConnection( this->triangleFilter->GetOutputPort() );
*/
#endif

  this->contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->contourFilter->SetInputConnection( this->surfaceReconstruction->GetOutputPort() );
  this->contourFilter->SetValue( 0, 0.0 );
  this->contourFilter->ComputeNormalsOn();

  this->reverseSense = vtkSmartPointer<vtkReverseSense>::New();
  this->reverseSense->SetInputConnection( this->contourFilter->GetOutputPort() );
  this->reverseSense->ReverseCellsOn();
  this->reverseSense->ReverseNormalsOn();


  this->smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  this->smoothFilter->SetInputConnection( this->reverseSense->GetOutputPort() );
  this->smoothFilter->SetNumberOfIterations( 50 );
  this->smoothFilter->SetRelaxationFactor( 0.05 );

  this->polydataNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  this->polydataNormals->SplittingOff();
  //this->polydataNormals->SetInputConnection( this->smoothFilter->GetOutputPort() );
  this->polydataNormals->SetInputConnection( this->reverseSense->GetOutputPort() );
}

MeshGenerator::~MeshGenerator()
{}

void MeshGenerator::setNeighborhoodSize( int size )
{
  this->surfaceReconstruction->SetNeighborhoodSize( size );
}

void MeshGenerator::setSampleSpacing( double spacing )
{
  this->surfaceReconstruction->SetSampleSpacing( spacing );
}

void MeshGenerator::setUsePowerCrust( bool enabled )
{
  this->usePowerCrust = enabled;

  if ( this->usePowerCrust )
  {
#ifdef SW_USE_POWERCRUST
    this->reverseSense->SetInputConnection( this->powercrust->GetOutputPort() );
    //this->reverseSense->SetInputConnection( this->triangleFilter->GetOutputPort() );
    //this->reverseSense->SetInputConnection( this->cleanPolyData->GetOutputPort() );
#endif
  }
  else
  {
    this->reverseSense->SetInputConnection( this->contourFilter->GetOutputPort() );
  }
}

vtkSmartPointer<vtkPolyData> MeshGenerator::buildMesh( const vnl_vector<double>& shape )
{
  // copy shape points into point set
  int numPoints = shape.size() / 3;
  this->points->SetNumberOfPoints( numPoints );
  unsigned int k = 0;
  for ( unsigned int i = 0; i < numPoints; i++ )
  {
    double x = shape[k++];
    double y = shape[k++];
    double z = shape[k++];
    this->points->SetPoint( i, x, y, z );
  }
  this->points->Modified();

  if ( !this->usePowerCrust )
  {
    this->surfaceReconstruction->Modified();
    this->surfaceReconstruction->Update();
    this->contourFilter->Update();
  }

  this->polydataNormals->Update();

  // make a copy of the vtkPolyData output to return
  vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
  polyData->DeepCopy( this->polydataNormals->GetOutput() );
  return polyData;
}
