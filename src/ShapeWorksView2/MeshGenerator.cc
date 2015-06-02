/*
 * Shapeworks license
 */

#include <MeshGenerator.h>

MeshGenerator::MeshGenerator(Preferences& prefs) : prefs_(prefs)
{
  this->points = vtkSmartPointer<vtkPoints>::New();
  this->points->SetDataTypeToDouble();
  this->pointSet = vtkSmartPointer<vtkPolyData>::New();
  this->pointSet->SetPoints( this->points );
  this->surfaceReconstruction = vtkSmartPointer<vtkSurfaceReconstructionFilter>::New();
 
#if VTK_MAJOR_VERSION <= 5
  surf->SetInput(polydata);
#else
  this->surfaceReconstruction->SetInputData(this->pointSet);
#endif

  this->surfaceReconstruction->SetSampleSpacing(prefs.getSpacing());
  this->surfaceReconstruction->SetNeighborhoodSize(prefs.getNeighborhood());
 
  this->contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->contourFilter->SetInputConnection(this->surfaceReconstruction->GetOutputPort());
  this->contourFilter->SetValue(0, 0.0);
  this->contourFilter->ComputeNormalsOn();

  this->smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  this->smoothFilter->SetNumberOfIterations(prefs.getSmoothingAmount());
  this->smoothFilter->SetInputConnection( this->contourFilter->GetOutputPort() );
 
  this->reverseSense = vtkSmartPointer<vtkReverseSense>::New();
  this->reverseSense->SetInputConnection(this->smoothFilter->GetOutputPort());
  this->reverseSense->ReverseCellsOn();
  this->reverseSense->ReverseNormalsOn();
}
MeshGenerator::~MeshGenerator() {}

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
  float spacing = this->prefs_.getSpacing();
  int neighborhood = this->prefs_.getNeighborhood();
  int smoothing = this->prefs_.getSmoothingAmount();
  this->surfaceReconstruction->SetSampleSpacing(spacing);
  this->surfaceReconstruction->SetNeighborhoodSize(neighborhood);
  this->surfaceReconstruction->Modified();
  this->surfaceReconstruction->Update();
  this->contourFilter->Update();
  this->smoothFilter->SetNumberOfIterations(smoothing);
  this->smoothFilter->Update();
  this->reverseSense->Update();
  return this->reverseSense->GetOutput();
}
