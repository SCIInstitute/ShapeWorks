#include <vtkSmartPointer.h>

#include <QStringList>

#include <Data/Shape.h>

class vtkPolyData;
class QTableWidget;
class Viewer;
class Shape;

class DataManager
{

public:
  DataManager();
  ~DataManager();

  void import_file( std::string file );

  std::vector<vtkSmartPointer<vtkPolyData> > get_meshes();

  void set_table_widget( QTableWidget* table_widget );
  void set_viewer( Viewer* viewer );

  void import_files( QStringList file_names );

  void update_table();

private:
  int num_shapes_;
  std::vector<vtkSmartPointer<vtkPolyData> > meshes_;

  std::vector<Shape> shapes_;

  QTableWidget* table_widget_;
  Viewer* viewer_;
};
