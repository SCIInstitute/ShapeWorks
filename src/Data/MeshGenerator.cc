/*
 * Shapeworks license
 */

#include <Data/MeshGenerator.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <limits>

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

  this->surfaceReconstruction->SetSampleSpacing(prefs.get_spacing());
  this->surfaceReconstruction->SetNeighborhoodSize(prefs.get_neighborhood());

  this->contourFilter = vtkSmartPointer<vtkContourFilter>::New();
  this->contourFilter->SetInputConnection(this->surfaceReconstruction->GetOutputPort());
  this->contourFilter->SetValue(0, 0.0);
  this->contourFilter->ComputeNormalsOn();

  this->smoothFilter = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
  this->smoothFilter->SetNumberOfIterations(prefs.get_smoothing_amount() * 100);
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
  float spacing = this->prefs_.get_spacing();
  int neighborhood = this->prefs_.get_neighborhood();
  int smoothing = this->prefs_.get_smoothing_amount() * 100;
  this->surfaceReconstruction->SetSampleSpacing(spacing);
  this->surfaceReconstruction->SetNeighborhoodSize(neighborhood);
  this->surfaceReconstruction->Modified();
  this->surfaceReconstruction->Update();
  this->contourFilter->Update();
  this->smoothFilter->SetNumberOfIterations(smoothing);
  this->smoothFilter->Update();
  this->reverseSense->Update();
  return this->transform_back(this->points,this->reverseSense->GetOutput());
}

vtkSmartPointer<vtkPolyData> MeshGenerator::transform_back(
    vtkSmartPointer<vtkPoints> pt,
    vtkSmartPointer<vtkPolyData> pd)
{
  double pt_bounds[6];
  double pd_bounds[6];
  pt->GetBounds(pt_bounds);
  pd->GetBounds(pd_bounds);

  double scaleX = (pt_bounds[1] - pt_bounds[0])/(pd_bounds[1] - pd_bounds[0]);
  double scaleY = (pt_bounds[3] - pt_bounds[2])/(pd_bounds[3] - pd_bounds[2]);
  double scaleZ = (pt_bounds[5] - pt_bounds[4])/(pd_bounds[5] - pd_bounds[4]);

  vtkSmartPointer<vtkTransform> transp = vtkSmartPointer<vtkTransform>::New();

  transp->Translate(pt_bounds[0], pt_bounds[2], pt_bounds[4]);
  transp->Scale(scaleX, scaleY, scaleZ);
  transp->Translate(- pd_bounds[0], - pd_bounds[2], - pd_bounds[4]);

  vtkSmartPointer<vtkTransformPolyDataFilter> tpd =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
#if VTK_MAJOR_VERSION <= 5
  tpd->SetInput(pd);
#else
  tpd->SetInputData(pd);
#endif
  tpd->SetTransform(transp);
  tpd->Update();
  return tpd->GetOutput();
}
