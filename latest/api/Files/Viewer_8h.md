---
title: Studio/src/Visualization/Viewer.h

---

# Studio/src/Visualization/Viewer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PickResult](../Classes/classshapeworks_1_1PickResult.md)**  |
| class | **[shapeworks::Viewer](../Classes/classshapeworks_1_1Viewer.md)** <br>3D [Viewer]() |




## Source code

```cpp
#pragma once

#include <Data/Shape.h>
#include <Visualization/ColorMap.h>
#include <Visualization/ColorSchemes.h>
#include <Visualization/SliceView.h>

#include <QPointF>
#include <QSharedPointer>

class vtkRenderer;
class vtkLookupTable;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;
class vtkGlyph3D;
class vtkSphereSource;
class vtkArrowSource;
class vtkTransformPolyDataFilter;
class vtkScalarBarActor;
class vtkCornerAnnotation;
class vtkPolyDataMapper;
class vtkActor;
class vtkTransform;
class vtkReverseSense;
class vtkHandleWidget;
class vtkPolygonalSurfacePointPlacer;
class vtkImageSlice;
class vtkImageSliceMapper;
class vtkImageData;
class vtkCellPicker;
class vtkPropPicker;
class vtkColorSeries;

namespace shapeworks {

class Shape;
class Viewer;
class Visualizer;
class StudioInteractorStyle;
class LandmarkWidget;
class PlaneWidget;
class PaintWidget;
class Session;

typedef QSharedPointer<Viewer> ViewerHandle;
typedef QVector<ViewerHandle> ViewerList;

class PickResult {
 public:
  Shape::Point pos_;
  int domain_ = -1;
  int subject_ = -1;
};


class Viewer {
 public:
  Viewer();
  ~Viewer() = default;

  void set_renderer(vtkSmartPointer<vtkRenderer> renderer);
  vtkSmartPointer<vtkRenderer> get_renderer();

  void display_shape(QSharedPointer<Shape> shape);

  void clear_viewer();
  void reset_camera(std::array<double, 3> c);
  void reset_camera();

  void set_color_series(ColorMap color_series);
  void set_glyph_size_and_quality(double size, double quality);
  double get_glyph_size();
  double get_glyph_quality();
  void set_session(QSharedPointer<Session> session);
  QSharedPointer<Session> get_session();

  void set_show_glyphs(bool show);
  void set_show_surface(bool show);

  void update_points();
  void update_glyph_properties();

  int handle_pick(int* click_pos);

  PickResult handle_ctrl_click(int* click_pos);

  void set_selected_point(int id);

  void set_lut(vtkSmartPointer<vtkLookupTable> lut);

  void set_loading_screen(vtkSmartPointer<vtkImageData> loading_screen);

  void set_color_scheme(int i);

  void handle_new_mesh();

  bool is_viewer_ready();

  void set_visualizer(Visualizer* visualizer);

  void update_feature_range(double* range);

  void update_opacities();

  QSharedPointer<Shape> get_shape();

  void update_landmarks();
  void update_planes();
  void update_ffc_mode();

  std::vector<vtkSmartPointer<vtkActor>> get_surface_actors();
  std::vector<vtkSmartPointer<vtkActor>> get_unclipped_surface_actors();

  MeshGroup get_meshes();

  vtkSmartPointer<vtkTransform> get_transform(int alignment_domain, int domain);

  vtkSmartPointer<vtkTransform> get_landmark_transform(int domain);

  vtkSmartPointer<vtkTransform> get_inverse_landmark_transform(int domain);

  vtkSmartPointer<vtkTransform> get_image_transform();

  SliceView& slice_view();

  void update_image_volume();

  vtkSmartPointer<vtkPoints> get_glyph_points();

  vtkSmartPointer<vtkTransform> get_alignment_transform();

  void update_clipping_planes();

  vtkSmartPointer<vtkPolygonalSurfacePointPlacer> get_point_placer();

  void handle_ffc_paint(double display_pos[2], double world_pos[3]);

  static bool is_reverse(vtkSmartPointer<vtkTransform> transform);

  void update_actors();

  void remove_scalar_bar();

  bool is_ready() { return mesh_ready_; }

  vtkFloatArray* get_particle_scalars();

  vtkSmartPointer<vtkPolyData> get_particle_poly_data();

  void insert_compare_meshes();

  void set_scalar_visibility(vtkSmartPointer<vtkPolyData> poly_data, vtkSmartPointer<vtkPolyDataMapper> mapper, std::string scalar);

 private:
  void initialize_surfaces();

  void display_vector_field();

  void compute_point_differences(const Eigen::VectorXd& points, vtkSmartPointer<vtkFloatArray> magnitudes,
                                 vtkSmartPointer<vtkFloatArray> vectors);

  void compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes, vtkSmartPointer<vtkFloatArray> vectors);

  void update_difference_lut(float r0, float r1);

  bool showing_feature_map();
  std::string get_displayed_feature_map();

  vtkSmartPointer<vtkPlane> transform_plane(vtkSmartPointer<vtkPlane> plane, vtkSmartPointer<vtkTransform> transform);

  bool visible_ = false;

  QSharedPointer<Shape> shape_;

  bool show_glyphs_ = true;
  bool show_surface_ = true;

  double glyph_size_ = 1.0f;
  double glyph_quality_ = 5.0f;
  ColorMap color_series_;

  vtkSmartPointer<vtkRenderer> renderer_;

  vtkSmartPointer<vtkSphereSource> sphere_source_;
  vtkSmartPointer<vtkReverseSense> reverse_sphere_;

  vtkSmartPointer<vtkPoints> glyph_points_;
  vtkSmartPointer<vtkPolyData> glyph_point_set_;
  vtkSmartPointer<vtkGlyph3D> glyphs_;
  vtkSmartPointer<vtkPolyDataMapper> glyph_mapper_;
  vtkSmartPointer<vtkActor> glyph_actor_;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> surface_mappers_;
  std::vector<vtkSmartPointer<vtkActor>> surface_actors_;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> unclipped_surface_mappers_;
  std::vector<vtkSmartPointer<vtkActor>> unclipped_surface_actors_;
  std::vector<vtkSmartPointer<vtkLookupTable>> ffc_luts_;

  // for comparing to other shapes (e.g. original vs groomed)
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> compare_mappers_;
  std::vector<vtkSmartPointer<vtkActor>> compare_actors_;

  vtkSmartPointer<vtkLookupTable> lut_;
  vtkSmartPointer<vtkLookupTable> surface_lut_;

  vtkSmartPointer<vtkArrowSource> arrow_source_;
  vtkSmartPointer<vtkTransformPolyDataFilter> arrow_flip_filter_;
  vtkSmartPointer<vtkGlyph3D> arrow_glyphs_;
  vtkSmartPointer<vtkPolyDataMapper> arrow_glyph_mapper_;
  vtkSmartPointer<vtkActor> arrow_glyph_actor_;
  vtkSmartPointer<vtkTransform> transform_180_;
  vtkSmartPointer<vtkScalarBarActor> scalar_bar_actor_;

  vtkSmartPointer<vtkCornerAnnotation> corner_annotation_;

  bool arrows_visible_ = false;

  ColorSchemes color_schemes_;
  int scheme_ = 0;

  bool mesh_ready_ = false;
  bool viewer_ready_ = false;
  bool loading_displayed_ = false;

  MeshGroup meshes_;
  MeshGroup compare_meshes_;

  Visualizer* visualizer_{nullptr};

  int number_of_domains_ = 0;

  std::shared_ptr<LandmarkWidget> landmark_widget_;
  std::shared_ptr<PlaneWidget> plane_widget_;
  vtkSmartPointer<PaintWidget> paint_widget_;

  QSharedPointer<Session> session_;

  std::string current_image_name_ = "-none-";

  vtkSmartPointer<vtkCellPicker> cell_picker_;
  vtkSmartPointer<vtkPropPicker> prop_picker_;
  vtkSmartPointer<vtkPolygonalSurfacePointPlacer> point_placer_;

  // slice viewer
  SliceView slice_view_{this};
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-07-09 at 13:02:49 -0600
