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

  /// remove shapes
  void remove_shapes( QList<int> list );

  /// return all shapes
  std::vector<QSharedPointer<Shape> > get_shapes();

  /// empty the project
  void clear();

Q_SIGNALS:
  /// signal that the data has changed
  void data_changed();

private:

  /// project filename
  QString filename_;

  /// collection of shapes
  std::vector<QSharedPointer<Shape> > shapes_;
};

#endif /* STUDIO_DATA_PROJECT_H */
