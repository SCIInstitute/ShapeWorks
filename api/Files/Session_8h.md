---
title: Studio/src/Data/Session.h

---

# Studio/src/Data/Session.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Session](../Classes/classshapeworks_1_1Session.md)** <br>Representation of a session.  |




## Source code

```cpp
#pragma once

#include <Data/MeshManager.h>
#include <Data/Preferences.h>
#include <Data/StudioParticles.h>
#include <Libs/Project/Project.h>
#include <Shapeworks.h>
#include <Visualization/Viewer.h>
#include <itkMatrixOffsetTransformBase.h>

#include <QSharedPointer>
#include <QVector>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

namespace shapeworks {

class Shape;

class Session;
typedef QSharedPointer<Session> SessionHandle;

using TransformType = vtkSmartPointer<vtkTransform>;


class Session : public QObject {
  Q_OBJECT;

 public:
  Session(QWidget* parent, Preferences& prefs);

  ~Session();

  void set_parent(QWidget* parent);

  bool save_project(std::string filename);

  bool load_project(QString filename);

  bool load_light_project(QString filename);

  bool load_xl_project(QString filename);

  void set_project_path(QString relative_path);

  std::shared_ptr<shapeworks::Project> get_project();

  void load_original_files(std::vector<std::string> filenames);

  void load_groomed_files(std::vector<std::string> file_names, double iso, int domains_per_shape);

  bool load_point_files(std::vector<std::string> local, std::vector<std::string> world, int domains_per_shape);

  bool update_particles(std::vector<StudioParticles> particles);

  void update_procrustes_transforms(std::vector<std::vector<std::vector<double>>> transforms);

  bool is_light_project();

  bool get_groomed_present();

  void remove_shapes(QList<int> list);

  QVector<QSharedPointer<Shape>> get_shapes();

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

  QSharedPointer<MeshManager> get_mesh_manager() { return this->mesh_manager_; }

  shapeworks::Parameters& parameters();

  std::vector<DomainType> get_domain_types();

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

  void handle_ctrl_click(PickResult result);

  void trigger_landmarks_changed();

  void set_active_landmark_domain(int id);
  int get_active_landmark_domain();
  void set_placing_landmark(int id);
  int get_placing_landmark();
  void set_landmarks_active(bool active);
  void set_show_landmarks(bool show);
  bool get_show_landmarks();

 public Q_SLOTS:
  void set_feature_auto_scale(bool value);

  void set_landmark_drag_mode(bool mode);
  bool get_landmark_drag_mode();

  void handle_clear_cache();
  void handle_new_mesh();
  void handle_message(QString s);
  void handle_thread_complete();

 signals:
  void data_changed();
  void points_changed();
  void landmarks_changed();
  void update_display();
  void new_mesh();
  void message(QString);
  void error(QString);
  void feature_range_changed();

 public:
  // constants
  const static std::string DATA_C;
  const static std::string GROOM_C;
  const static std::string OPTIMIZE_C;
  const static std::string ANALYSIS_C;
  const static std::string DEEPSSM_C;

 private:
  Preferences& preferences_;

  void save_particles_file(std::string filename, const Eigen::VectorXd& points);

  void renumber_shapes();

  QWidget* parent_{nullptr};

  QString filename_;

  QString project_path_;

  QVector<QSharedPointer<Shape>> shapes_;

  QSharedPointer<MeshManager> mesh_manager_;

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
  bool show_landmark_labels_ = false;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2022-02-16 at 06:47:28 +0000
