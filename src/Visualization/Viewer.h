#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>


class vtkRenderer;
class CustomImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageData;

class Viewer
{
public:
  Viewer();
  ~Viewer();

  vtkRenderer *getRenderer();
  void setInteractor(vtkRenderWindowInteractor *interactor);

  void addInput(vtkSmartPointer<vtkImageData> imageData);


private:

  bool initialized;
  vtkSmartPointer<vtkRenderer> renderer;
  vtkRenderWindowInteractor* interactor;
  std::vector<vtkImageData*> inputs;

  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor>          actor;
};