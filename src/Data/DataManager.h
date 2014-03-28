#include <vtkSmartPointer.h>

#include <QStringList>

#include <Data/Shape.h>

class vtkPolyData;
class QTableWidget;
class Viewer;
class Shape;

class DataManager : public QObject
{
  Q_OBJECT;

public:
  DataManager();
  ~DataManager();

  void import_file( QString file );

  std::vector<vtkSmartPointer<vtkPolyData> > get_meshes();

  void set_table_widget( QTableWidget* table_widget );
  void set_viewer( QSharedPointer<Viewer> viewer );

  void import_files( QStringList file_names );

  void remove_shape( int i );

  std::vector<QSharedPointer<Shape> > get_shapes();

  void update_shapes();

Q_SIGNALS:
  void data_changed();

private:

  std::vector<QSharedPointer<Shape> > shapes_;

  QTableWidget* table_widget_;
  QSharedPointer<Viewer> viewer_;
};
