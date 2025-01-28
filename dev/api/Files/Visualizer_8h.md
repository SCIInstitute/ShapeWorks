---
title: Studio/Visualization/Visualizer.h

---

# Studio/Visualization/Visualizer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Visualizer](../Classes/classshapeworks_1_1Visualizer.md)** <br>Controls display of objects in viewers.  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Visualization/Lightbox.h>

#include <map>
#include <string>

namespace shapeworks {

class Visualizer;

typedef QSharedPointer<Visualizer> VisualizerHandle;


class Visualizer : public QObject {
  Q_OBJECT;

 public:
  Visualizer(Preferences& prefs);
  ~Visualizer();

  void set_lightbox(LightboxHandle lightbox);

  void set_session(SessionHandle session);

  void set_center(bool center);

  bool get_center();

  void set_alignment_domain(int domain);
  int get_alignment_domain();

  void set_show_glyphs(bool show);

  void set_show_surface(bool show);

  void display_samples();

  void update_samples();

  void update_landmarks();
  void update_planes();
  void update_paint_mode();

  void display_sample(int i);

  void display_shape(ShapeHandle shape);
  void display_shapes(ShapeList shapes);

  void set_selected_point_one(int id);
  void set_selected_point_two(int id);

  void set_mean(const Eigen::VectorXd& mean);
  void set_mean_shape(ShapeHandle mean_shape);
  ShapeHandle get_mean_shape();

  void reset_camera();

  void update_lut();
  void update_annotations();

  Particles get_current_shape();

  vtkFloatArray* get_current_particle_scalars();
  vtkSmartPointer<vtkPolyData> get_current_particle_poly_data();

  void handle_new_mesh();
  vtkSmartPointer<vtkPolyData> get_current_mesh(int index);
  std::vector<vtkSmartPointer<vtkPolyData>> get_current_meshes_transformed(int index);

  const std::string& get_feature_map() const;

  void set_uniform_feature_range(bool value);

  bool get_uniform_feature_range(void);

  void set_feature_map(const std::string& feature_map);

  void clear_viewers();

  void reset_feature_range();

  double* get_feature_range();

  double* get_feature_raw_range();

  bool get_feature_range_valid();

  void update_feature_range(double* range);

  void update_feature_range(double min, double max);

  vtkSmartPointer<vtkTransform> get_transform(std::shared_ptr<Shape> shape, int alignment_domain, int domain);

  vtkSmartPointer<vtkTransform> get_transform(std::shared_ptr<Shape> shape, DisplayMode display_mode,
                                              int alignment_domain, int domain);

  void set_opacities(std::vector<float> opacities);

  std::vector<float> get_opacities();

  void set_domain_particle_visibilities(std::vector<bool> visibilities);

  std::vector<bool> get_domain_particle_visibilities();

  double get_current_glyph_size();

  void handle_ctrl_click(PickResult result);

  void redraw();

  QPixmap export_to_pixmap(QSize size, bool transparent_background, bool show_orientation_marker, bool show_color_scale,
                           bool& ready);

  QSize get_render_size();

  LightboxHandle get_lightbox() { return this->lightbox_; }

 public Q_SLOTS:

  void update_viewer_properties();

  void handle_feature_range_changed();

  void handle_image_slice_settings_changed();

 private:
  Preferences& preferences_;

  void compute_measurements();

  void setup_single_selected_point_lut();

  std::string feature_map_;
  int alignment_domain_;

  bool center_;
  bool needs_camera_reset_ = true;

  bool show_glyphs_ = true;
  bool show_surface_ = true;

  LightboxHandle lightbox_;
  SessionHandle session_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  Eigen::VectorXd cached_mean_;
  ShapeHandle mean_shape_;
  Particles current_shape_;

  double feature_range_[2] = {0, 0};
  double feature_manual_range_[2] = {0, 0};
  bool feature_range_valid_ = false;
  bool feature_range_uniform_ = true;

  std::vector<bool> domain_particle_visibilities_;
  std::vector<float> opacities_;

  double current_glyph_size_{0};
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-01-28 at 22:52:41 +0000
