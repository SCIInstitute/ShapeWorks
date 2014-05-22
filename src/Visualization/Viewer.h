#ifndef STUDIO_VISUALIZATION_VIEWER_H
#define STUDIO_VISUALIZATION_VIEWER_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>

class vtkRenderer;
class CustomImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;

class Mesh;

class Viewer
{
public:
  Viewer();
  ~Viewer();

  void set_meshes( std::vector<QSharedPointer<Mesh> > meshes );

  void set_interactor( vtkRenderWindowInteractor* interactor );

  void set_render_window( vtkRenderWindow* render_window );

  void set_tile_layout( int width, int height );

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row( int row );

  void set_auto_center( bool center );

private:

  void clear_renderers();
  void display_meshes();

  void insert_mesh_into_view( vtkSmartPointer<vtkPolyData> poly_data, int position, int id, QString note );

  std::vector<QSharedPointer<Mesh> > meshes_;

  vtkSmartPointer<vtkRenderer> renderer_;

  std::vector<vtkSmartPointer<vtkRenderer> > renderers_;

  vtkRenderWindow* render_window_;

  vtkRenderWindowInteractor* interactor_;

  vtkCamera* camera_;

  int tile_layout_width_;
  int tile_layout_height_;

  int start_row_;

  bool first_draw_;

  bool auto_center_;
};

#endif /* STUDIO_VISUALIZATION_VIEWER_H */
