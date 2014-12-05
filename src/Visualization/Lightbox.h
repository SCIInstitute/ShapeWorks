#ifndef STUDIO_VISUALIZATION_LIGHTBOX_H
#define STUDIO_VISUALIZATION_LIGHTBOX_H

#include <QSharedPointer>
#include <QVector>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>

#include <Visualization/Viewer.h>

class Mesh;
class Shape;
class DisplayObject;
class StudioInteractorStyle;
class Visualizer;

class Lightbox;
typedef QSharedPointer< Lightbox > LightboxHandle;

//! Display multiple Viewers in a tiled display
/*!
 * The LightBox class displays multiple Viewers in a tiled display
 */
class Lightbox
{
public:
  Lightbox();
  ~Lightbox();

  void set_display_objects( QVector < QSharedPointer < DisplayObject > > objects );

  void set_interactor( vtkRenderWindowInteractor* interactor );

  void set_render_window( vtkRenderWindow* render_window );

  void set_tile_layout( int width, int height );

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row( int row );

  ViewerList get_viewers();
  void redraw();

  void handle_pick( int* click_pos, bool one );

  void set_glyph_lut( vtkSmartPointer<vtkLookupTable> lut );

  void set_visualizer( Visualizer* visualizer );

private:

  void clear_renderers();
  void display_objects();

  void insert_object_into_viewer( QSharedPointer<DisplayObject> object, int position );

  vtkSmartPointer<vtkRenderer> renderer_;

  QVector < QSharedPointer < DisplayObject > > objects_;

  // there is one viewer for every tile in the lightbox display
  ViewerList viewers_;

  vtkRenderWindow* render_window_;

  vtkRenderWindowInteractor* interactor_;

  vtkCamera* camera_;

  int tile_layout_width_;
  int tile_layout_height_;

  int start_row_;

  bool first_draw_;

  vtkSmartPointer<StudioInteractorStyle> style_;

  Visualizer* visualizer_;
};

#endif /* STUDIO_VISUALIZATION_LIGHTBOX_H */
