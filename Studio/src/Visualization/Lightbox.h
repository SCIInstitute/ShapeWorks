#pragma once

#include <Data/Preferences.h>
#include <Visualization/Viewer.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

#include <QSharedPointer>
#include <QTimer>
#include <QVector>

class vtkOrientationMarkerWidget;

namespace shapeworks {

class StudioMesh;
class Shape;
class StudioInteractorStyle;
class StudioSliceInteractorStyle;
class Visualizer;

class Lightbox;
typedef QSharedPointer<Lightbox> LightboxHandle;

//! Display multiple Viewers in a tiled display
/*!
 * The LightBox class displays multiple Viewers in a tiled display
 */
class Lightbox : public QObject {
  Q_OBJECT;

 public:
  Lightbox();
  ~Lightbox();

  void set_shapes(QVector<QSharedPointer<Shape>> shapes);
  QVector<ShapeHandle> get_shapes();

  void set_interactor(vtkRenderWindowInteractor* interactor);

  void set_render_window(vtkRenderWindow* render_window);

  void set_tile_layout(int width, int height);

  void setup_renderers();

  int get_num_rows();
  int get_num_rows_visible();

  void set_start_row(int row);

  ViewerList get_viewers();

  void redraw();

  void handle_pick(int* click_pos, bool one, bool ctrl);

  void handle_hover(int* click_pos);

  void handle_key(int* click_pos, std::string key);

  void set_glyph_lut(vtkSmartPointer<vtkLookupTable> lut);
  void set_session(QSharedPointer<Session> session);

  void set_visualizer(Visualizer* visualizer);

  bool render_window_ready() { return render_window_ != NULL; }

  void clear_renderers();

  std::array<double, 3> initPos();

  void handle_new_mesh();

  void reset_camera();

  void reset_camera_clipping_range();

  void set_orientation_marker(Preferences::OrientationMarkerType type, Preferences::OrientationMarkerCorner corner);

  void set_orientation_marker_viewport();

  void update_feature_range();

  void update_interactor_style();

  void set_shared_window_and_level(double window, double level);

  vtkRenderWindow* get_render_window();

 public Q_SLOTS:
  void handle_timer_callback();

 private:
  vtkSmartPointer<vtkOrientationMarkerWidget> create_orientation_marker();

  void check_for_first_draw();

  void display_shapes();

  void insert_shape_into_viewer(QSharedPointer<Shape> shape, int position);

  int get_start_shape();

  vtkSmartPointer<vtkRenderer> renderer_;

  QVector<QSharedPointer<Shape>> shapes_;

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
  vtkSmartPointer<StudioSliceInteractorStyle> slice_style_;

  Visualizer* visualizer_{nullptr};

  std::vector<vtkSmartPointer<vtkImageData>> spinner_images_;

  QTimer loading_timer_;

  int timer_callback_count_ = 0;

  std::array<double, 3> initPos_;

  vtkSmartPointer<vtkOrientationMarkerWidget> orientation_marker_widget_;

  Preferences::OrientationMarkerType current_orientation_marker_type_ = Preferences::OrientationMarkerType::none;
  Preferences::OrientationMarkerCorner current_orientation_marker_corner_ =
      Preferences::OrientationMarkerCorner::upper_right;

  QSharedPointer<Session> session_;
};
}  // namespace shapeworks
