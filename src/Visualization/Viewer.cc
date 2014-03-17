#include <Visualization/Viewer.h>

#include <vtkRenderer.h>
#include <vtkImageActor.h>
#include <vtkImageViewer2.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkSphereSource.h>

Viewer::Viewer()
{

  this->renderer = vtkSmartPointer<vtkRenderer>::New();
}

void Viewer::setInteractor( vtkRenderWindowInteractor* interactor )
{
  this->interactor = interactor;
}

void Viewer::addInput( vtkSmartPointer<vtkImageData> imageData )
{

/*

   vtkSmartPointer<vtkImageActor> actor =
    vtkSmartPointer<vtkImageActor>::New();

   actor->SetInputData( imageData );

   this->renderer->AddActor(actor);
   this->renderer->ResetCamera();
 */

/*
   vtkSmartPointer<vtkImageViewer2> imageViewer = vtkSmartPointer<vtkImageViewer2>::New();
   imageViewer->SetInputData( imageData );
   imageViewer->SetupInteractor( this->interactor );
   imageViewer->Render();
   imageViewer->GetRenderer()->ResetCamera();
   imageViewer->Render();
 */

  vtkSmartPointer<vtkContourFilter> contour = vtkSmartPointer<vtkContourFilter>::New();
  contour->SetInputData( imageData );
  contour->SetNumberOfContours( 1 );
  contour->SetValue( 0, 0.5 );

  contour->Update();

  vtkSmartPointer<vtkPolyData> polyData = contour->GetOutput();

  this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->actor = vtkSmartPointer<vtkActor>::New();

  vtkSphereSource* sphere = vtkSphereSource::New();
  sphere->SetRadius( 1.0 );
  sphere->SetThetaResolution( 18 );
  sphere->SetPhiResolution( 18 );

  //this->mapper->SetInputConnection( sphere->GetOutputPort() );

  this->mapper->SetInputConnection( contour->GetOutputPort() );
  this->actor->SetMapper( this->mapper );
  this->renderer->AddActor( this->actor );
  this->renderer->ResetCamera();
  double leftViewport[4] = {0.0, 0.0, 0.5, 2.0};
  this->renderer->SetViewport( leftViewport );
  this->renderer->SetBackground( .6, .5, .4 );
}

vtkRenderer* Viewer::getRenderer()
{
  return this->renderer;
}
