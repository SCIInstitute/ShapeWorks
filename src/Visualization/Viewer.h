#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>


class vtkRenderer;
class CustomImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;

class Viewer
{
public:
  Viewer();
  ~Viewer();

  vtkRenderer *getRenderer();
  void setInteractor(vtkRenderWindowInteractor *interactor);

  void addInput(vtkSmartPointer<vtkPolyData> polyData);


  void setRenderWindow(vtkRenderWindow *renderWindow);

private:

  bool initialized;
  vtkSmartPointer<vtkRenderer> renderer;

  std::vector<vtkSmartPointer<vtkRenderer> > renderers;

  vtkRenderWindow *renderWindow_;

  vtkRenderWindowInteractor* interactor;
  std::vector<vtkImageData*> inputs;

  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor>          actor;

  vtkCamera *camera_;

  int count;

};