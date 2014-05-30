#ifndef STUDIO_VISUALIZATION_LIGHTBOX_H
#define STUDIO_VISUALIZATION_LIGHTBOX_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>

#include <Visualization/Viewer.h>

class Mesh;
class Shape;

class Lightbox
{
public:
  Lightbox();
  ~Lightbox();

  void set_shapes( std::vector<QSharedPointer<Shape> > shapes );

  void set_interactor( vtkRenderWindowInteractor* interactor );

  void set_render_window( vtkRenderWindow* render_window );

  void set_tile_layout( int width, int height );

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row( int row );

  void set_auto_center( bool center );

  void set_mesh_mode( QString mode );

private:

  void clear_renderers();
  void display_shapes();

  void insert_shape_into_view( QSharedPointer<Shape> shape, int position, int id );

  vtkSmartPointer<vtkRenderer> renderer_;

  std::vector< QSharedPointer < Shape > > shapes_;

  std::vector< QSharedPointer < Viewer > > viewers_;

  vtkRenderWindow* render_window_;

  vtkRenderWindowInteractor* interactor_;

  vtkCamera* camera_;

  int tile_layout_width_;
  int tile_layout_height_;

  int start_row_;

  bool first_draw_;

  bool auto_center_;

  QString mesh_mode_;

public:

  // constants
  const static QString INITIAL_C;
  const static QString GROOMED_C;
  const static QString RECONSTRUCTED_C;
};

#endif /* STUDIO_VISUALIZATION_LIGHTBOX_H */
