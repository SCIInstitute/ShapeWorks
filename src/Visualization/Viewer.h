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

  void set_tile_layout(int width, int height);

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row(int row);

private:

  void insert_models();


  void insert_model_into_view(vtkSmartPointer<vtkPolyData> poly_data);

  vtkSmartPointer<vtkRenderer> renderer_;

  std::vector<vtkSmartPointer<vtkRenderer> > renderers_;

  vtkRenderWindow *render_window_;

  vtkRenderWindowInteractor* interactor_;

  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkActor>          actor_;


  std::vector<vtkSmartPointer<vtkPolyData> > models_;

  vtkCamera *camera_;

  int count_;

  int tile_layout_width_;
  int tile_layout_height_;

  int start_row_;



};