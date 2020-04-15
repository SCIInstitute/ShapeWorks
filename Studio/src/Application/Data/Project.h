#ifndef STUDIO_DATA_PROJECT_H
#define STUDIO_DATA_PROJECT_H

#include <vector>

#include <QSharedPointer>
#include <QVector>
#include "Data/Preferences.h"
#include "Data/MeshManager.h"
#include <Groom/ShapeWorksGroom.h>

class Shape;

class Project;
typedef QSharedPointer< Project > ProjectHandle;

//! Representation of a project.
/*!
 * The Project class encapsulates everything about a project.
 *
 */
class Project : public QObject
{
  Q_OBJECT;

public:

  /// constructor
  Project(QWidget* parent, Preferences& prefs);

  /// destructor
  ~Project();

  /// set QWidget parent
  void set_parent( QWidget* parent );

  /// save project to file
  bool save_project( std::string filename, std::string dataDir, std::string cutPlanesFile);

  /// load project from file
  bool load_project( QString filename, std::string& planesFile);
  
  /// read a lightweight project file
  bool load_light_project( QString filename, std::string& planesFile );

  /// get the pointer to the data manager
  //QSharedPointer<DataManager> get_data_manager();

  /// import files
    void load_original_files( std::vector<std::string> file_names );

  /// load groomed files
  void load_groomed_files(std::vector<std::string> file_names, double iso);
  void load_groomed_images(std::vector<ImageType::Pointer> images, double iso);

  /// load point files
  bool load_point_files(std::vector<std::string> file_names, bool local);
  bool update_points(std::vector<std::vector<itk::Point<double> > > points, bool local);

  void set_reconstructed_present(bool b);

  bool is_light_project();

  bool get_groomed_present();

  /// remove shapes
  void remove_shapes( QList<int> list );

  /// return all shapes
  QVector<QSharedPointer<Shape> > get_shapes();
  
  void calculate_reconstructed_samples();

  /// reset the project
  void reset();

  /// get the filename
  QString get_filename();

  bool original_present();
  bool groomed_present();
  bool reconstructed_present();
  bool groups_available();
  int get_num_shapes();

  QSharedPointer<MeshManager> get_mesh_manager() { return this->mesh_manager_; }


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
  void message(std::string s);

public:
  // constants
  const static std::string DATA_C;
  const static std::string GROOM_C;
  const static std::string OPTIMIZE_C;
  const static std::string ANALYSIS_C;

private:

  Preferences& preferences_;

  void renumber_shapes();

  QWidget* parent_;

  /// project filename
  QString filename_;

  /// collection of shapes
  QVector<QSharedPointer<Shape> > shapes_;

  QSharedPointer<MeshManager> mesh_manager_;

  bool original_present_;
  bool groomed_present_;
  bool reconstructed_present_;
  bool groups_available_ = false;
  bool is_light_project_ = false;

};

#endif /* STUDIO_DATA_PROJECT_H */
