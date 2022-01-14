---
title: Studio/src/Visualization/Visualizer.h

---

# Studio/src/Visualization/Visualizer.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Visualizer](../Classes/classshapeworks_1_1Visualizer.md)** <br>Controls display of objects in viewers.  |




## Source code

```cpp
#pragma once

#include <map>
#include <string>

#include <Data/Session.h>
#include <Data/Preferences.h>
#include <Visualization/Lightbox.h>

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

  void set_display_mode(std::string mode);

  std::string get_display_mode();

  void set_center(bool center);

  bool get_center();

  void set_alignment_domain(int domain);
  int get_alignment_domain();

  void set_show_glyphs(bool show);

  void set_show_surface(bool show);

  void display_samples();

  void update_samples();

  void display_sample(int i);

  void display_shape(ShapeHandle shape);
  void display_shapes(QVector<QSharedPointer<Shape>> shapes);

  void set_selected_point_one(int id);
  void set_selected_point_two(int id);

  static const std::string MODE_ORIGINAL_C;
  static const std::string MODE_GROOMED_C;
  static const std::string MODE_RECONSTRUCTION_C;

  void set_mean(const Eigen::VectorXd& mean);

  void reset_camera();

  void update_lut();

  StudioParticles get_current_shape();

  void handle_new_mesh();
  vtkSmartPointer<vtkPolyData> get_current_mesh();
  std::vector<vtkSmartPointer<vtkPolyData>> get_current_meshes_transformed();

  const std::string& get_feature_map() const;

  void set_uniform_feature_range(bool value);

  bool get_uniform_feature_range(void);

  void set_feature_map(const std::string& feature_map);

  void clear_viewers();

  void reset_feature_range();

  double* get_feature_range();

  double *get_feature_raw_range();

  bool get_feature_range_valid();

  void update_feature_range(double* range);

  vtkSmartPointer<vtkTransform> get_transform(QSharedPointer<Shape> shape, int alignment_domain, int domain);

  void set_opacities(std::vector<float> opacities);

  std::vector<float> get_opacities();

  double get_current_glyph_size();

public Q_SLOTS:

  void update_viewer_properties();

  void handle_feature_range_changed();

private:
  ShapeHandle create_display_object(const StudioParticles& points,
                                    const std::vector<Shape::Point>& vectors);
  Preferences& preferences_;

  void compute_measurements();

  std::string display_mode_;
  std::string feature_map_;
  int alignment_domain_;

  bool center_;
  bool needs_camera_reset_ = true;

  bool show_glyphs_;
  bool show_surface_;

  LightboxHandle lightbox_;
  SessionHandle session_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  Eigen::VectorXd cached_mean_;
  StudioParticles current_shape_;

  double feature_range_[2] = {0, 0};
  double feature_manual_range_[2] = {0, 0};
  bool feature_range_valid_ = false;
  bool feature_range_uniform_ = true;

  std::vector<float> opacities_;

  double current_glyph_size_{0};

};

}
```


-------------------------------

Updated on 2022-01-14 at 16:56:17 +0000
