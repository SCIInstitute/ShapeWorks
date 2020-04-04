#pragma once

#include <QSharedPointer>
#include <QVector>
#include <QTimer>

#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkCamera.h>

#include <Visualization/Viewer.h>

class Mesh;
class Shape;
class StudioInteractorStyle;
class Visualizer;

class Lightbox;
typedef QSharedPointer< Lightbox > LightboxHandle;

//! Display multiple Viewers in a tiled display
/*!
 * The LightBox class displays multiple Viewers in a tiled display
 */
class Lightbox : public QObject
{
  Q_OBJECT;
public:
  Lightbox();
  ~Lightbox();

  void set_display_objects(QVector<QSharedPointer<Shape>> objects);

  void set_interactor(vtkRenderWindowInteractor* interactor);

  void set_render_window(vtkRenderWindow* render_window);

  void set_tile_layout(int width, int height);

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row(int row);

  ViewerList get_viewers();

  void redraw();

  void handle_pick(int* click_pos, bool one);

  void set_glyph_lut(vtkSmartPointer<vtkLookupTable> lut);

  void set_visualizer(Visualizer* visualizer);

  bool render_window_ready() { return render_window_ != NULL; }

  void clear_renderers();

  std::array<double, 3> initPos();

public Q_SLOTS:
  void handle_timer_callback();

private:

  void display_objects();

  void insert_object_into_viewer(QSharedPointer<Shape> object, int position);

  vtkSmartPointer<vtkRenderer> renderer_;

  QVector<QSharedPointer<Shape>> objects_;

  // there is one viewer for every tile in the lightbox display
  ViewerList viewers_;

  vtkRenderWindow* render_window_ = nullptr;

  vtkRenderWindowInteractor* interactor_ = nullptr;

  vtkCamera* camera_ = nullptr;

  int tile_layout_width_ = 4;
  int tile_layout_height_ = 4;

  int start_row_ = 0;

  bool first_draw_ = true;

  vtkSmartPointer<StudioInteractorStyle> style_;

  Visualizer* visualizer_ = nullptr;

  std::vector<vtkSmartPointer<vtkImageData>> spinner_images_;

  QTimer loading_timer_;

  int timer_callback_count_ = 0;

  std::array<double, 3> initPos_;
};
