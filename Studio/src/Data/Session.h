#pragma once

#include <Data/MeshManager.h>
#include <Data/Preferences.h>
#include <Data/StudioParticles.h>
#include <Libs/Project/Project.h>
#include <Shapeworks.h>
#include <Visualization/Viewer.h>
#include <itkMatrixOffsetTransformBase.h>

#include <Libs/Particles/ParticleSystem.h>
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

//! TODO: replace this
using TransformType = vtkSmartPointer<vtkTransform>;

//! Representation of a session.
/*!
 * The Session class encapsulates everything about a session/project.
 *
 */
class Session : public QObject, public QEnableSharedFromThis<Session> {
  Q_OBJECT;

 public:
  /// constructor
  Session(QWidget* parent, Preferences& prefs);

  /// destructor
  ~Session();

  /// set QWidget parent
  void set_parent(QWidget* parent);

  /// save project to file
  bool save_project(QString filename);

  /// load project from file
  bool load_project(QString filename);

  /// read a lightweight project file
  bool load_light_project(QString filename);

  /// load a shapeworks project
  bool load_xl_project(QString filename);

  void set_project_path(QString relative_path);

  std::shared_ptr<shapeworks::Project> get_project();

  /// import files
  void load_original_files(std::vector<std::string> filenames);

  /// load groomed files
  void load_groomed_files(std::vector<std::string> file_names, double iso, int domains_per_shape);

  bool load_point_files(std::vector<std::string> local, std::vector<std::string> world, int domains_per_shape);

  bool update_particles(std::vector<StudioParticles> particles);

  //! Return the total number of particles for all domains, combined
  int get_num_particles();

  ParticleSystem get_local_particle_system(int domain);

  void update_procrustes_transforms(std::vector<std::vector<std::vector<double>>> transforms);

  bool is_light_project();

  bool get_groomed_present();

  /// remove shapes
  void remove_shapes(QList<int> list);

  /// return all shapes
  QVector<QSharedPointer<Shape>> get_shapes();

  void calculate_reconstructed_samples();

  /// get the filename
  QString get_filename();

  /// get file display name
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

  std::vector<DomainType> get_groomed_domain_types();

  double update_auto_glyph_size();

  double get_auto_glyph_size();

  static Point3 get_point(const Eigen::VectorXd& points, int i);

  //! clear particles from session (e.g. groom start, optimize start)
  void clear_particles();

  bool get_feature_auto_scale();

  double get_feature_range_max();
  double get_feature_range_min();
  void set_feature_range(double min, double max);
  void set_feature_range_min(double value);
  void set_feature_range_max(double value);

  void handle_ctrl_click(PickResult result);

  void trigger_landmarks_changed();
  void trigger_planes_changed();
  void trigger_ffc_changed();

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
  bool get_image_share_window_and_level();

  bool has_constraints();

  void set_loading(bool loading);
  bool is_loading();

  void set_tool_state(std::string state);
  std::string get_tool_state();

  void set_ffc_paint_active(bool enabled);
  bool get_ffc_paint_active();

  void set_ffc_paint_mode_inclusive(bool inclusive);
  bool get_ffc_paint_mode_inclusive();

  void set_ffc_paint_size(double size);
  double get_ffc_paint_size();

  bool get_show_good_bad_particles();
  void set_show_good_bad_particles(bool enabled);

  std::vector<bool> get_good_bad_particles();
  void set_good_bad_particles(const std::vector<bool>& good_bad);

  void trigger_repaint();

 public Q_SLOTS:
  void set_feature_auto_scale(bool value);

  void set_landmark_drag_mode(bool mode);
  bool get_landmark_drag_mode();

  void handle_clear_cache();
  void handle_new_mesh();
  void handle_message(QString s);
  void handle_thread_complete();

 signals:
  /// signal that the data has changed
  void data_changed();
  void points_changed();
  void landmarks_changed();
  void planes_changed();
  void ffc_changed();
  void update_display();
  void new_mesh();
  void message(QString);
  void error(QString);
  void feature_range_changed();
  void update_view_mode();
  void image_slice_settings_changed();
  void ffc_paint_mode_changed();
  void repaint();

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

  void new_landmark(PickResult result);

  void new_plane_point(PickResult result);

  QWidget* parent_{nullptr};

  /// project filename
  QString filename_;

  QString project_path_;

  /// collection of shapes
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
  bool planes_active_ = false;
  bool show_landmark_labels_ = false;

  bool ffc_painting_active_ = false;
  bool ffc_painting_inclusive_mode_ = false;
  double ffc_paint_size = 50;

  bool is_loading_ = false;
};

}  // namespace shapeworks
