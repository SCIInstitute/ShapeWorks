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
  this->surfaceReconstruction->SetInput( this->pointSet );

#ifdef SW_USE_POWERCRUST
  this->powercrust = vtkSmartPointer<vtkPowerCrustSurfaceReconstruction>::New();
  this->powercrust->SetInput( this->pointSet );

  //this->cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
  //this->cleanPolyData->SetInputConnection( this->powercrust->GetOutputPort() );
/*
   this->triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
   this->triangleFilter->SetInputConnection( this->powercrust->GetOutputPort() );
   //this->triangleFilter->SetInputConnection( this->cleanPolyData->GetOutputPort() );

   this->cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
   this->cleanPolyData->SetInputConnection( this->triangleFilter->GetOutputPort() );
   //  this->cleanPolyData->SetTolerance(0.5);



   this->decimate = vtkSmartPointer<vtkDecimatePro>::New();
   //this->decimate->SetInputConnection(this->butterfly->GetOutputPort());
   //this->decimate->SetInputConnection(this->cleanPolyData->GetOutputPort());
   this->decimate->SetInputConnection(this->triangleFilter->GetOutputPort());
   this->decimate->SetTargetReduction(0.90);
   this->decimate->PreserveTopologyOff();
   this->decimate->BoundaryVertexDeletionOff();
   this->decimate->SplittingOn();

   this->butterfly = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
   this->butterfly->SetInputConnection(this->decimate->GetOutputPort());
   this->butterfly->SetNumberOfSubdivisions(1);


   this->powerPolydataNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
   this->powerPolydataNormals->SplittingOff();
   this->powerPolydataNormals->SetInputConnection( this->butterfly->GetOutputPort() );
   //this->powerPolydataNormals->SetInputConnection( this->triangleFilter->GetOutputPort() );
   //this->powerPolydataNormals->SetInputConnection( this->cleanPolyData->GetOutputPort() );
   
 */

  //this->powerCrustTail = this->powercrust->GetOutputPort();
#endif // ifdef SW_USE_POWERCRUST

  this->contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->contourFilter->SetInputConnection( this->surfaceReconstruction->GetOutputPort() );
  this->contourFilter->SetValue( 0, 0.0 );
  this->contourFilter->ComputeNormalsOn();

  this->windowSincFilter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
  this->windowSincFilter->SetInputConnection( this->contourFilter->GetOutputPort() );


  //this->regularTail = this->contourFilter->GetOutputPort();

  //this->windowSincFilter->BoundarySmoothingOff();
  //this->windowSincFilter->FeatureEdgeSmoothingOff();
  //this->windowSincFilter->SetFeatureAngle(120.0);
  //this->windowSincFilter->NonManifoldSmoothingOn();
  //this->windowSincFilter->NormalizeCoordinatesOn();

  //this->reverseSense = vtkSmartPointer<vtkReverseSense>::New();
//  this->reverseSense->SetInputConnection( this->contourFilter->GetOutputPort() );
//this->reverseSense->ReverseCellsOn();
//this->reverseSense->ReverseNormalsOn();

  //this->smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  //this->smoothFilter->SetInputConnection( this->reverseSense->GetOutputPort() );
  //this->smoothFilter->SetNumberOfIterations( 50 );
  //this->smoothFilter->SetRelaxationFactor( 0.05 );

  this->polydataNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  this->polydataNormals->SplittingOff();
  //this->polydataNormals->SetInputConnection( this->smoothFilter->GetOutputPort() );
  //this->polydataNormals->SetInputConnection( this->reverseSense->GetOutputPort() );
  // 
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
  if (amount <= 0)
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


void MeshGenerator::updatePipeline()
{
  if ( this->usePowerCrust )
  {
#ifdef SW_USE_POWERCRUST
    this->polydataNormals->SetInputConnection( this->powercrust->GetOutputPort() );
#endif
  }
  else
  {
    if (smoothingEnabled)
    {
      this->polydataNormals->SetInputConnection( this->windowSincFilter->GetOutputPort());
    }
    else
    {
      this->polydataNormals->SetInputConnection( this->contourFilter->GetOutputPort() );
    }
  }
}
