#ifndef STUDIO_DATA_PROJECT_H
#define STUDIO_DATA_PROJECT_H

#include <vector>

#include <QSharedPointer>
#include <QVector>

class DataManager;
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
  Project();

  /// destructor
  ~Project();

  /// save project to file
  bool save_project( QString filename = "" );

  /// save project to file
  bool load_project( QString filename );

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
  QVector<QSharedPointer<Shape> > get_shapes();

  /// reset the project
  void reset();

  /// get the current pipeline state
  //QString get_pipeline_state();

  /// get the filename
  QString get_filename();

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

  void renumber_shapes();

  /// project filename
  QString filename_;

  /// collection of shapes
  QVector<QSharedPointer<Shape> > shapes_;

  bool originals_present_;
  bool groomed_present_;
  bool reconstructed_present_;

  QString pipeline_state_;

};

#endif /* STUDIO_DATA_PROJECT_H */
