#pragma once


#include <QSharedPointer>
#include <Visualization/ColorSchemes.h>
#include <Application/Data/Shape.h>

class vtkRenderer;
class vtkLookupTable;
class vtkRenderWindowInteractor;
class vtkImageData;
class vtkCamera;
class vtkGlyph3D;
class vtkSphereSource;
class vtkColorTransferFunction;
class vtkArrowSource;
class vtkTransformPolyDataFilter;
class vtkScalarBarActor;
class vtkCornerAnnotation;
class vtkPolyDataMapper;
class vtkActor;
class vtkTransform;
class Shape;
class Viewer;
class Visualizer;
class StudioInteractorStyle;

typedef QSharedPointer<Viewer> ViewerHandle;
typedef QVector<ViewerHandle> ViewerList;

//! 3D Viewer
/*!
 * The Viewer class encapsulates all the functionality for visualizing a single subject/shape
 *
 */
class Viewer {

public:

  Viewer();
  ~Viewer() = default;

  void set_renderer(vtkSmartPointer<vtkRenderer> renderer);
  vtkSmartPointer<vtkRenderer> get_renderer();

  void display_shape(QSharedPointer<Shape> shape);

  void clear_viewer();
  void reset_camera(std::array<double, 3> c);

  void set_glyph_size_and_quality(double size, double quality);
  void set_show_glyphs(bool show);
  void set_show_surface(bool show);

  void update_points();
  void update_glyph_properties();

  int handle_pick(int* click_pos);

  void set_selected_point(int id);

  void set_lut(vtkSmartPointer<vtkLookupTable> lut);

  void set_loading_screen(vtkSmartPointer<vtkImageData> loading_screen);

  void set_color_scheme(int i);

  void handle_new_mesh();

  bool is_viewer_ready();

  void set_visualizer(Visualizer* visualizer);

private:

  void display_vector_field();

  void compute_point_differences(const std::vector<Point>& points,
                                 vtkSmartPointer<vtkFloatArray> magnitudes,
                                 vtkSmartPointer<vtkFloatArray> vectors);

  void compute_surface_differences(vtkSmartPointer<vtkFloatArray> magnitudes,
                                   vtkSmartPointer<vtkFloatArray> vectors);

  void draw_exclusion_spheres(QSharedPointer<Shape> object);

  void updateDifferenceLUT(float r0, float r1);

  void update_actors();


  bool visible_;

  QSharedPointer<Shape> shape_;

  bool show_glyphs_ = true;
  bool show_surface_ = true;

  double glyph_size_ = 1.0f;
  double glyph_quality_ = 5.0f;

  vtkSmartPointer<vtkRenderer> renderer_;

  vtkSmartPointer<vtkSphereSource> sphere_source_;

  vtkSmartPointer<vtkPoints> glyph_points_;
  vtkSmartPointer<vtkPolyData> glyph_point_set_;
  vtkSmartPointer<vtkGlyph3D> glyphs_;
  vtkSmartPointer<vtkPolyDataMapper> glyph_mapper_;
  vtkSmartPointer<vtkActor> glyph_actor_;

  vtkSmartPointer<vtkPoints> exclusion_sphere_points_;
  vtkSmartPointer<vtkPolyData> exclusion_sphere_point_set_;
  vtkSmartPointer<vtkGlyph3D> exclusion_sphere_glyph_;
  vtkSmartPointer<vtkPolyDataMapper> exclusion_sphere_mapper_;
  vtkSmartPointer<vtkActor> exclusion_sphere_actor_;

  vtkSmartPointer<vtkPolyDataMapper> surface_mapper_;
  vtkSmartPointer<vtkActor> surface_actor_;

  vtkSmartPointer<vtkLookupTable> lut_;

  vtkSmartPointer<StudioInteractorStyle> style_;

  vtkSmartPointer<vtkColorTransferFunction> difference_lut_;
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
  int scheme_;

  bool mesh_ready_ = false;
  bool viewer_ready_ = false;
  bool loading_displayed_ = false;

  QSharedPointer<Mesh> mesh_;

  Visualizer* visualizer_{nullptr};
};
