#include <MeshGenerator.h>

#include "CustomSurfaceReconstructionFilter.h"

#define SW_USE_POWERCRUST

// local files
#ifdef SW_USE_POWERCRUST
#include "vtkPowerCrustSurfaceReconstruction.h"
#endif

MeshGenerator::MeshGenerator()
{
  this->usePowerCrust = false;

  this->scalars = vtkSmartPointer<vtkUnsignedLongArray>::New();

  this->glyphPoints = vtkSmartPointer<vtkPoints>::New();
  this->glyphPoints->SetDataTypeToDouble();
  this->glyphPointSet = vtkSmartPointer<vtkPolyData>::New();
  this->glyphPointSet->SetPoints( this->glyphPoints );
  this->glyphPointSet->GetPointData()->SetScalars( this->scalars );

  this->surface = vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
  this->surface->SetInput( this->glyphPointSet );

#ifdef SW_USE_POWERCRUST
  this->powercrust = vtkSmartPointer<vtkPowerCrustSurfaceReconstruction>::New();
  this->powercrust->SetInput( this->glyphPointSet );
#endif

  this->surfaceContourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->surfaceContourFilter->SetInputConnection( this->surface->GetOutputPort() );
  this->surfaceContourFilter->SetValue( 0, 0.0 );
  this->surfaceContourFilter->ComputeNormalsOn();

  this->surfaceReverseSense = vtkSmartPointer<vtkReverseSense>::New();
  this->surfaceReverseSense->SetInputConnection( this->surfaceContourFilter->GetOutputPort() );
  this->surfaceReverseSense->ReverseCellsOn();
  this->surfaceReverseSense->ReverseNormalsOn();

  this->surfaceSmoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  this->surfaceSmoothFilter->SetInputConnection( this->surfaceReverseSense->GetOutputPort() );
  this->surfaceSmoothFilter->SetNumberOfIterations( 0 );

  this->polydataNormals = vtkSmartPointer<vtkPolyDataNormals>::New();
  this->polydataNormals->SplittingOff();
  this->polydataNormals->SetInputConnection( this->surfaceSmoothFilter->GetOutputPort() );
}

MeshGenerator::~MeshGenerator()
{}

void MeshGenerator::setNeighborhoodSize( int size )
{
  this->surface->SetNeighborhoodSize( size );
  this->meshCache.clear();
}

void MeshGenerator::setSampleSpacing( double spacing )
{
  this->surface->SetSampleSpacing( spacing );
  this->meshCache.clear();
}

vtkSmartPointer<vtkPolyData> MeshGenerator::buildMesh( const vnl_vector<double>& shape )
{
  // check cache....

  vtkSmartPointer<vtkPolyData> polyData;

  if ( this->meshCache.getMesh( shape ) )
  {
    polyData = this->meshCache.getMesh( shape );
  }
  else
  {
    int numPoints = shape.size() / 3;

    this->glyphPoints->SetNumberOfPoints( numPoints );
    // copy shape into point set
    unsigned int k = 0;
    for ( unsigned int i = 0; i < numPoints; i++ )
    {
      double x = shape[k++];
      double y = shape[k++];
      double z = shape[k++];
      this->glyphPoints->SetPoint( i, x, y, z );
    }
    this->glyphPoints->Modified();

    if ( !this->usePowerCrust )
    {
      this->surface->Modified();
      this->surface->Update();
      this->surfaceContourFilter->Update();
    }

    this->polydataNormals->Update();

    // make a copy of the vtkPolyData output and place it in the cache
    polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy( this->polydataNormals->GetOutput() );
    this->meshCache.insertMesh( shape, polyData );
  }

  return polyData;
}

void MeshGenerator::setUsePowerCrust( bool enabled )
{
  this->usePowerCrust = enabled;
  this->meshCache.clear();

  if ( this->usePowerCrust )
  {
#ifdef SW_USE_POWERCRUST
    this->surfaceReverseSense->SetInputConnection( this->powercrust->GetOutputPort() );
#endif
  }
  else
  {
    this->surfaceReverseSense->SetInputConnection( this->surfaceContourFilter->GetOutputPort() );
  }
}
