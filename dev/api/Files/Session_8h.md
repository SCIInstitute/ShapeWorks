---
title: Studio/Data/Session.h

---

# Studio/Data/Session.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::CompareSettings](../Classes/classshapeworks_1_1CompareSettings.md)**  |
| class | **[shapeworks::Session](../Classes/classshapeworks_1_1Session.md)** <br>Representation of a session.  |




## Source code

```cpp
#pragma once

#include <Analyze/Analyze.h>
#include <Analyze/Particles.h>
#include <Data/Preferences.h>
#include <Logging.h>
#include <MeshManager.h>
#include <Particles/ParticleSystemEvaluation.h>
#include <Project/Project.h>
#include <Shapeworks.h>
#include <StudioEnums.h>
#include <Visualization/Viewer.h>
#include <itkMatrixOffsetTransformBase.h>
#include <vtkLookupTable.h>

#include <QSharedPointer>
#include <QVector>
#include <string>
#include <vector>

namespace shapeworks {

class PythonWorker;

class CompareSettings {
 public:
  bool compare_enabled_ = false;
  bool surface_distance_mode_ = false;
  bool original_checked_ = false;
  bool groomed_checked_ = false;
  bool reconstructed_checked_ = false;
  bool mean_shape_checked_ = false;
  float opacity_ = 1.0;
  DisplayMode get_display_mode() {
    if (original_checked_) {
      return DisplayMode::Original;
    } else if (groomed_checked_) {
      return DisplayMode::Groomed;
    } else {
      return DisplayMode::Reconstructed;
    }
  }
  bool get_mean_shape_checked() { return mean_shape_checked_; }
};

class Shape;

class Session;
typedef QSharedPointer<Session> SessionHandle;

using TransformType = vtkSmartPointer<vtkTransform>;


class Session : public QObject, public QEnableSharedFromThis<Session> {
  Q_OBJECT;

 public:
  using AlignmentType = Analyze::AlignmentType;

  Session(QWidget* parent, Preferences& prefs);

  ~Session();

  void set_parent(QWidget* parent);

  bool save_project(QString filename);

  bool load_project(QString filename);

  bool load_light_project(QString filename);

  bool load_xl_project(QString filename);

  void set_project_path(QString relative_path);

  std::shared_ptr<shapeworks::Project> get_project();

  void load_original_files(std::vector<std::string> filenames);

  void load_groomed_files(std::vector<std::string> file_names, double iso, int domains_per_shape);

  bool load_point_files(std::vector<std::string> local, std::vector<std::string> world, int domains_per_shape);

  bool update_particles(std::vector<Particles> particles);

  int get_num_particles();

  ParticleSystemEvaluation get_local_particle_system(int domain);

  void update_procrustes_transforms(std::vector<std::vector<std::vector<double>>> transforms);

  bool is_light_project();

  bool get_groomed_present();

  void remove_shapes(QList<int> list);

  ShapeList get_shapes();

  ShapeList get_non_excluded_shapes();

  void calculate_reconstructed_samples();

  QString get_filename();

  QString get_display_name();

  bool original_present();
  bool groomed_present();
  bool particles_present();
  bool groups_available();
  int get_num_shapes();

  int get_domains_per_shape();

  std::string get_default_feature_map();

  static bool is_supported_file_format(std::string filename);

  std::shared_ptr<MeshManager> get_mesh_manager() { return this->mesh_manager_; }

  shapeworks::Parameters& parameters();

  std::vector<DomainType> get_groomed_domain_types();

  double update_auto_glyph_size();

  double get_auto_glyph_size();

  static Point3 get_point(const Eigen::VectorXd& points, int i);

  void clear_particles();

  bool get_feature_auto_scale();

  double get_feature_range_max();
  double get_feature_range_min();
  void set_feature_range(double min, double max);
  void set_feature_range_min(double value);
  void set_feature_range_max(double value);

  void set_feature_uniform_scale(bool value);
  bool get_feature_uniform_scale();

  void handle_ctrl_click(PickResult result);

  void trigger_landmarks_changed();
  void trigger_planes_changed();
  void trigger_ffc_changed();
  void trigger_annotations_changed();
  void trigger_save();
  void trigger_data_changed();
  void reload_particles();

  void set_active_landmark_domain(int id);
  int get_active_landmark_domain();
  void set_placing_landmark(int id);
  int get_placing_landmark();
  void set_landmarks_active(bool active);
  bool get_landmarks_active();
  void set_planes_active(bool active);
  bool get_planes_active();
  void set_show_landmark_labels(bool show);
  bool get_show_landmark_labels();

  void set_show_planes(bool show);
  bool get_show_planes();
  bool should_show_planes();

  void set_show_landmarks(bool show);
  bool get_show_landmarks();

  // image volume
  bool set_image_name(std::string image_name);
  std::string get_image_name();

  // image axis
  void set_image_axis(QString axis);
  Axis get_image_axis();

  // image view 3d mode
  void set_image_3d_mode(bool mode);
  bool get_image_3d_mode();

  // image sync/share window width and level
  void set_image_share_window_and_level(bool enabled);
  bool get_image_share_brightness_contrast();

  // image sync slice
  void set_image_sync_slice(bool enabled);
  bool get_image_sync_slice();

  // 3D image, thickness feature
  void set_image_thickness_feature(bool enabled);
  bool get_image_thickness_feature();

  void set_feature_map(std::string feature_map);
  std::string get_feature_map();

  bool has_constraints();

  void set_loading(bool loading);
  bool is_loading();

  void set_tool_state(std::string state);
  std::string get_tool_state();
  bool is_analysis_mode();

  void set_ffc_paint_active(bool enabled);
  bool get_ffc_paint_active();

  void set_seg_paint_active(bool enabled);
  bool get_seg_paint_active();

  void set_seg_paint_value(int value);
  int get_seg_paint_value();

  void set_ffc_paint_mode_inclusive(bool inclusive);
  bool get_ffc_paint_mode_inclusive();

  void set_ffc_paint_size(double size);
  double get_ffc_paint_size();

  void set_seg_paint_size(double size);
  double get_seg_paint_size();

  bool get_show_good_bad_particles();
  void set_show_good_bad_particles(bool enabled);

  bool get_show_difference_vectors();
  void set_show_difference_vectors(bool enabled);
  bool should_difference_vectors_show();

  std::vector<bool> get_good_bad_particles();
  void set_good_bad_particles(const std::vector<bool>& good_bad);

  // for setting difference to mean, etc
  void set_difference_particles(Particles particles) { difference_particles_ = particles; }
  Particles get_difference_particles() { return difference_particles_; }

  void set_compare_settings(CompareSettings settings);
  CompareSettings get_compare_settings();

  void trigger_repaint();

  void trigger_reinsert_shapes();

  void set_display_mode(DisplayMode mode);

  DisplayMode get_display_mode();

  void set_glyph_lut(vtkSmartPointer<vtkLookupTable> lut) { glyph_lut_ = lut; }
  vtkSmartPointer<vtkLookupTable> get_glyph_lut() { return glyph_lut_; }

  void set_py_worker(QSharedPointer<PythonWorker> worker) { py_worker_ = worker; }
  QSharedPointer<PythonWorker> get_py_worker() { return py_worker_; }

  Eigen::MatrixXd get_all_particles();
  Eigen::MatrixXd get_all_scalars(std::string target_feature);

  void set_current_alignment(AlignmentType alignment) { current_alignment_ = alignment; }
  AlignmentType get_current_alignment() { return current_alignment_; }

  bool is_modified() { return modified_; }
  void set_modified(bool modified);

  void recompute_surfaces();

 public Q_SLOTS:
  void set_feature_auto_scale(bool value);

  void set_landmark_drag_mode(bool mode);
  bool get_landmark_drag_mode();

  void handle_clear_cache();
  void handle_new_mesh();
  void handle_thread_complete();

 Q_SIGNALS:
  void data_changed();
  void tool_state_changed();
  void points_changed();
  void landmarks_changed();
  void planes_changed();
  void ffc_changed();
  void update_display();
  void feature_map_changed();
  void reset_stats();
  void new_mesh();
  void feature_range_changed();
  void update_view_mode();
  void image_slice_settings_changed();
  void paint_mode_changed();
  void repaint();
  void reinsert_shapes();
  void annotations_changed();
  void save();
  void session_title_changed();
  void image_name_changed();

 public:
  // constants
  const static std::string DATA_C;
  const static std::string GROOM_C;
  const static std::string OPTIMIZE_C;
  const static std::string ANALYSIS_C;
  const static std::string DEEPSSM_C;
  const static std::string MONAI_C;

 private:
  void renumber_shapes();

  void new_landmark(PickResult result);

  void new_plane_point(PickResult result);

  QWidget* parent_{nullptr};

  Preferences& preferences_;

  QString filename_;

  ShapeList shapes_;

  Particles difference_particles_;

  std::shared_ptr<MeshManager> mesh_manager_;

  bool groups_available_{false};
  bool is_light_project_{false};

  bool unsaved_particle_files_{false};

  Parameters params_;

  std::shared_ptr<Project> project_{new Project()};

  double auto_glyph_size_ = -1;

  int active_landmark_domain_ = -1;
  int placing_landmark_ = -1;
  bool landmark_drag_mode_ = false;
  bool landmarks_active_ = false;
  bool planes_active_ = false;
  bool show_landmark_labels_ = false;
  bool show_difference_vectors_ = false;

  bool ffc_painting_active_ = false;
  bool ffc_painting_inclusive_mode_ = false;
  double ffc_paint_size_ = 50;
  double seg_paint_size_ = 50;
  bool seg_painting_active_ = false;
  int seg_painting_value_ = 1;

  bool is_loading_ = false;
  CompareSettings compare_settings_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;

  QSharedPointer<PythonWorker> py_worker_;

  AlignmentType current_alignment_{AlignmentType::Local};

  bool modified_{false};
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2025-06-03 at 17:15:47 +0000
