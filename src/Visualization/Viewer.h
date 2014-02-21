#include <vtkSmartPointer.h>


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

  void addInput(vtkImageData *imageData);


private:

  bool initialized;
  vtkSmartPointer<vtkRenderer> renderer;

  std::vector<vtkImageData*> inputs;
};