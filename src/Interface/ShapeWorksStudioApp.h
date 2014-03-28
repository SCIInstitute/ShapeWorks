#include <QMainWindow>
#include <QActionGroup>

class DataManager;
class Viewer;
class GroomTool;

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

class ViewerLayout;

class ShapeWorksStudioApp : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksStudioApp( int argc, char** argv );
  ~ShapeWorksStudioApp();

public Q_SLOTS:

  void on_actionQuit_triggered();
  void on_actionImport_triggered();
  void on_thumbnail_size_slider_valueChanged();
  void on_vertical_scroll_bar_valueChanged();
  void on_addButton_clicked();
  void on_deleteButton_clicked();

  void on_actionGroomMode_triggered();
  void on_actionImportMode_triggered();

  void mode_changed();

private:

  void update_scrollbar();

  void update_table();

  void import_files( QStringList fileNames );

  // designer form
  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_;

  QSharedPointer<Viewer> viewer_;

  QSharedPointer<DataManager> data_manager_;

  QSharedPointer<GroomTool> groom_tool_;
};
