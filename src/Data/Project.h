#ifndef STUDIO_DATA_PROJECT_H
#define STUDIO_DATA_PROJECT_H

#include <vector>

#include <QSharedPointer>

class DataManager;
class Shape;

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
  Project();

  /// destructor
  ~Project();

  /// save project to file
  bool save_project( QString filename = "" );

  /// save project to file
  bool open_project( QString filename );

  /// get the pointer to the data manager
  QSharedPointer<DataManager> get_data_manager();

  /// import files
  void import_files( QStringList file_names );

  /// load groomed files
  void load_groomed_files( QStringList file_names );

  /// load point files
  void load_point_files( QStringList file_names );

  /// remove shapes
  void remove_shapes( QList<int> list );

  /// return all shapes
  std::vector<QSharedPointer<Shape> > get_shapes();

  /// reset the project
  void reset();

  /// get the current pipeline state
  //QString get_pipeline_state();


  bool originals_present();
  bool groomed_present();
  bool reconstructed_present();

Q_SIGNALS:
  /// signal that the data has changed
  void data_changed();

public:
  // constants
  const static QString INITIAL_C;
  const static QString GROOMED_C;
  const static QString OPTIMIZED_C;

private:

  /// project filename
  QString filename_;

  /// collection of shapes
  std::vector<QSharedPointer<Shape> > shapes_;


  bool originals_present_;
  bool groomed_present_;
  bool reconstructed_present_;


  QString pipeline_state_;
};

#endif /* STUDIO_DATA_PROJECT_H */
