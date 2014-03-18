#include <Visualization/Viewer.h>

#include <vtkRenderer.h>
#include <vtkImageActor.h>
//#include <vtkImageViewer2.h>
#include <vtkContourFilter.h>
#include <vtkImageData.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindow.h>

Viewer::Viewer()
{

  this->renderer = vtkSmartPointer<vtkRenderer>::New();
  count = 0;
}

void Viewer::setInteractor( vtkRenderWindowInteractor* interactor )
{
  this->interactor = interactor;
}

void Viewer::addInput(vtkSmartPointer<vtkPolyData> polyData)
{
  std::cerr << polyData->GetNumberOfVerts() << "\n";


  this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->actor = vtkSmartPointer<vtkActor>::New();

  vtkSmartPointer<vtkRenderer> ren = vtkSmartPointer<vtkRenderer>::New();

  if (this->count == 0)
  {
    this->camera_ = ren->GetActiveCamera();
  }
  else
  {
    ren->SetActiveCamera(this->camera_);
  }


  this->renderers.push_back(ren);



  this->mapper->SetInputData( polyData );
  this->actor->SetMapper( this->mapper );
  ren->AddActor( this->actor );
  ren->ResetCamera();
  //double viewport[4] = {0.0, 0.0, 0.5, 1.0};
  double viewport[4] = {0.0, 0.0, 0.0, 0.0};

  viewport[0] = ((this->count % 4)) * 0.25;
  viewport[2] = viewport[0] + 0.25;

  viewport[1] = (3-(this->count) / 4) * 0.25;
  viewport[3] = viewport[1] + 0.25;

  // margins
  viewport[0] += 0.01;
  viewport[1] += 0.01;
  viewport[2] -= 0.01;
  viewport[3] -= 0.01;


  std::cerr << "viewport = {" << viewport[0] << ", " << viewport[1] << ", " << viewport[2] << ", " << viewport[3] << "}\n";

  ren->SetViewport( viewport );
  //ren->SetBackground( .2, .2 * count, .1 * count );

  float color = 0.2 + (0.1 * (count % 3));

  ren->SetBackground( color, color, color );

  this->renderWindow_->AddRenderer(ren);

  this->count++;

}

vtkRenderer* Viewer::getRenderer()
{
  return this->renderer;
}

void Viewer::setRenderWindow(vtkRenderWindow *renderWindow)
{
  this->renderWindow_ = renderWindow;
}
