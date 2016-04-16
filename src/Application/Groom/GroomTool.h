#ifndef STUDIO_GROOM_GROOMTOOL_H
#define STUDIO_GROOM_GROOMTOOL_H

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include "Data/Preferences.h"
#include <ShapeWorksGroom.h>
#include <string>

class Project;
class Ui_GroomTool;
class ShapeWorksStudioApp;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool(Preferences& prefs, std::vector<std::string>& files);
  ~GroomTool();

  /// export XML for ShapeWorksGroom
  bool export_xml( QString filename );

  /// set the pointer to the project
  void set_project( QSharedPointer<Project> project );

  void update_preferences();
  void set_preferences();

Q_SIGNALS:
  void groom_complete();
  void error_message(std::string);
  void message(std::string);
  void progress(size_t);

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged( int state );
  void on_blur_checkbox_stateChanged(int state);
  void on_fastmarching_checkbox_stateChanged(int state);
  void on_autopad_checkbox_stateChanged(int state);
  void on_skipButton_clicked();
  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error(std::string msg);

  /// Export XML
  void on_export_xml_button_clicked();

  /// Run groom tool
  void on_run_groom_button_clicked();

private:

  Ui_GroomTool* ui_;
  QSharedPointer<Project> project_;
  Preferences& preferences_;
  std::vector<std::string>& files_;
  ShapeWorksGroom * groom_;
};

#endif /* STUDIO_GROOM_GROOMTOOL_H */
