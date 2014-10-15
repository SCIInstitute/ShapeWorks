#ifndef STUDIO_GROOM_GROOMTOOL_H
#define STUDIO_GROOM_GROOMTOOL_H

#include <QSharedPointer>
#include <QWidget>

class Project;
class Ui_GroomTool;
class ShapeWorksStudioApp;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool();
  ~GroomTool();

  /// export XML for ShapeWorksGroom
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  /// set the pointer to the application
  void set_app( ShapeWorksStudioApp* app );


Q_SIGNALS:
  void groom_complete();

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged( int state );
  void on_blur_checkbox_stateChanged( int state );

  /// Export XML
  void on_export_xml_button_clicked();

  /// Run groom tool
  void on_run_groom_button_clicked();

private:

  Ui_GroomTool* ui_;
  QSharedPointer<Project> project_;
  ShapeWorksStudioApp* app_;
};

#endif /* STUDIO_GROOM_GROOMTOOL_H */
