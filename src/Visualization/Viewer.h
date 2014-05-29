#ifndef STUDIO_VISUALIZATION_VIEWER_H
#define STUDIO_VISUALIZATION_VIEWER_H

#include <QSharedPointer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>

class vtkRenderer;
class vtkLookupTable;
class CustomImagePlaneWidget;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;
class vtkGlyph3D;

class Mesh;
class Shape;

class MiniViewer
{
public:
  vtkSmartPointer<vtkRenderer>             renderer_;
  vtkSmartPointer<vtkPoints>               glyphPoints;
  vtkSmartPointer<vtkPolyData>             glyphPointSet;
  vtkSmartPointer<vtkGlyph3D>              glyphs;
  vtkSmartPointer<vtkPolyDataMapper>       glyphMapper;
  vtkSmartPointer<vtkActor>                glyphActor;

  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor> actor;
};

class Viewer
{
public:
  Viewer();
  ~Viewer();

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
  void display_meshes();

  void insert_shape_into_view( QSharedPointer<Shape> shape, int position, int id );

  vtkSmartPointer<vtkRenderer> renderer_;

  vtkSmartPointer<vtkLookupTable> lut_;

  std::vector< QSharedPointer < Shape > > shapes_;

  std::vector< QSharedPointer < MiniViewer > > mini_viewers_;

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

#endif /* STUDIO_VISUALIZATION_VIEWER_H */
