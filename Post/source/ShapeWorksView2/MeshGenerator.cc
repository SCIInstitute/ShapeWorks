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
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkDecimatePro.h>
#include <vtkButterflySubdivisionFilter.h>
#include <vtkPolyDataToImageData.h>

#include <vtkMetaImageWriter.h>
#include <vtkPolyDataWriter.h>

#include "CustomSurfaceReconstructionFilter.h"

// local files
#ifdef SW_USE_POWERCRUST
#include "vtkPowerCrustSurfaceReconstruction.h"
#endif

MeshGenerator::MeshGenerator()
{
  this->usePowerCrust = false;
  this->smoothingEnabled = false;

  this->points = vtkSmartPointer<vtkPoints>::New();
  this->points->SetDataTypeToDouble();
  this->pointSet = vtkSmartPointer<vtkPolyData>::New();
  this->pointSet->SetPoints( this->points );

  this->surfaceReconstruction = vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
  this->surfaceReconstruction->SetInputData( this->pointSet );

#ifdef SW_USE_POWERCRUST
  this->polydataToImageData = vtkSmartPointer<vtkPolyDataToImageData>::New();
  this->triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
  this->powercrust = vtkSmartPointer<vtkPowerCrustSurfaceReconstruction>::New();
  this->powercrust->SetInput( this->pointSet );
  this->triangleFilter->SetInputConnection( this->powercrust->GetOutputPort() );
  this->polydataToImageData->SetInputConnection( this->triangleFilter->GetOutputPort() );
#endif // ifdef SW_USE_POWERCRUST

  this->contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->contourFilter->SetInputConnection( this->surfaceReconstruction->GetOutputPort() );
  this->contourFilter->SetValue( 0, 0.0 );
  this->contourFilter->ComputeNormalsOn();

  this->windowSincFilter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  this->windowSincFilter->SetInputConnection( this->contourFilter->GetOutputPort() );

  this->smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  this->smoothFilter->SetInputConnection( this->contourFilter->GetOutputPort() );

  this->polydataNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  this->polydataNormals->SplittingOff();

  this->updatePipeline();
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
  this->updatePipeline();
}

void MeshGenerator::setSmoothingAmount( float amount )
{
  if ( amount <= 0 )
  {
    this->smoothingEnabled = false;
  }
  else
  {
    this->smoothingEnabled = true;
    this->windowSincFilter->SetNumberOfIterations( amount );
    this->windowSincFilter->SetPassBand( 0.05 );
  }

  this->updatePipeline();
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

  if ( this->usePowerCrust )
  {
    if ( this->smoothingEnabled )
    {
      this->polydataToImageData->Update();
      this->contourFilter->Update();
    }
  }
  else
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

void MeshGenerator::updatePipeline()
{
  if ( this->usePowerCrust && this->smoothingEnabled )
  {
    this->contourFilter->SetInputConnection( this->polydataToImageData->GetOutputPort() );
    this->contourFilter->SetValue( 0, 0.5 );
    this->polydataNormals->SetInputConnection( this->windowSincFilter->GetOutputPort() );
  }
  else if ( this->usePowerCrust && !this->smoothingEnabled )
  {
#ifdef SW_USE_POWERCRUST
    this->polydataNormals->SetInputConnection( this->powercrust->GetOutputPort() );
#endif
  }
  else if ( !this->usePowerCrust && this->smoothingEnabled )
  {
    this->contourFilter->SetInputConnection( this->surfaceReconstruction->GetOutputPort() );
    this->contourFilter->SetValue( 0, 0.0 );
    this->polydataNormals->SetInputConnection( this->windowSincFilter->GetOutputPort() );
  }
  else if ( !this->usePowerCrust && !this->smoothingEnabled )
  {
    this->polydataNormals->SetInputConnection( this->contourFilter->GetOutputPort() );
  }
}
