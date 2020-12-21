#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include <QSharedPointer>
#include <QVector>

#include <itkMatrixOffsetTransformBase.h>

#include <Libs/Project/Project.h>

#include <Data/Preferences.h>
#include <Data/MeshManager.h>

namespace shapeworks {

class Shape;

class Session;
typedef QSharedPointer<Session> SessionHandle;

//! TODO: replace this
using TransformType = itk::MatrixOffsetTransformBase<double, 3, 3>::ParametersType;

//! Representation of a session.
/*!
 * The Session class encapsulates everything about a session/project.
 *
 */
class Session : public QObject {
Q_OBJECT;

public:

  /// constructor
  Session(QWidget* parent, Preferences& prefs);

  /// destructor
  ~Session();

  /// set QWidget parent
  void set_parent(QWidget* parent);

  /// save project to file
  bool save_project(std::string filename);

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
  void load_groomed_files(std::vector<std::string> file_names, double iso);

  /// load point files
  bool load_point_files(std::vector<std::string> file_names, bool local);
  bool update_points(std::vector<std::vector<itk::Point<double>>> points, bool local);

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

  void set_groom_unsaved(bool value);

  QSharedPointer<MeshManager> get_mesh_manager()
  { return this->mesh_manager_; }

  shapeworks::Parameters& parameters();

public Q_SLOTS:
  void handle_clear_cache();
  void handle_new_mesh();
  void handle_message(std::string s);
  void handle_thread_complete();

signals:
  /// signal that the data has changed
  void data_changed();
  void points_changed();
  void update_display();
  void new_mesh();
  void message(std::string s);

public:
  // constants
  const static std::string DATA_C;
  const static std::string GROOM_C;
  const static std::string OPTIMIZE_C;
  const static std::string ANALYSIS_C;

private:

  Preferences& preferences_;

  void save_particles_file(std::string filename, const vnl_vector<double>& points);

  void renumber_shapes();

  QWidget* parent_{nullptr};

  /// project filename
  QString filename_;

  QString project_path_;

  /// collection of shapes
  QVector<QSharedPointer<Shape>> shapes_;

  QSharedPointer<MeshManager> mesh_manager_;

  bool groups_available_{false};
  bool is_light_project_{false};

  bool unsaved_groomed_files_{false};
  bool unsaved_particle_files_{false};

  Parameters params_;

  std::shared_ptr<Project> project_{new Project()};
};

}