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

  vtkRenderer *get_renderer();
  void set_interactor(vtkRenderWindowInteractor *interactor);

  void add_input(vtkSmartPointer<vtkPolyData> poly_data);


  void set_render_window(vtkRenderWindow *render_window);

private:


  void setup_renderers();

  vtkSmartPointer<vtkRenderer> renderer_;

  std::vector<vtkSmartPointer<vtkRenderer> > renderers_;

  vtkRenderWindow *render_window_;

  vtkRenderWindowInteractor* interactor_;

  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor>          actor_;


  vtkCamera *camera_;

  int count_;

};