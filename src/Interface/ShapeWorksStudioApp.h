#ifndef STUDIO_INTERFACE_SHAPEWORKSSTUDIOAPP_H
#define STUDIO_INTERFACE_SHAPEWORKSSTUDIOAPP_H


#include <QMainWindow>
#include <QActionGroup>

class Viewer;
class GroomTool;
class Project;

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

  void on_actionOpenProject_triggered();
  void on_actionSaveProjectAs_triggered();
  void on_actionQuit_triggered();
  void on_actionImport_triggered();
  void on_vertical_scroll_bar_valueChanged();
  void on_addButton_clicked();
  void on_deleteButton_clicked();

  void on_actionGroomMode_triggered();
  void on_actionImportMode_triggered();

  void on_center_checkbox_stateChanged();
  void on_thumbnail_size_slider_valueChanged();


  void mode_changed();

  void handle_project_changed();

private:

  void update_scrollbar();

  void update_table();

  void import_files( QStringList fileNames );

  // designer form
  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_;

  QSharedPointer<Viewer> viewer_;

  QSharedPointer<GroomTool> groom_tool_;

  QSharedPointer<Project> project_;

};

#endif /* STUDIO_INTERFACE_SHAPEWORKSSTUDIOAPP_H */