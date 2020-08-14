#pragma once

// qt
#include <QDialog>

// studio
#include <Data/Preferences.h>

class Ui_SplashScreen;
class QListWidgetItem;

namespace shapeworks {

/*!
 * The SplashScreen is a QDialog that implements the user interface for the Studio Splash Screen
 * It supports a recent file list as well as loading other existing projects
 */
class SplashScreen : public QDialog {
Q_OBJECT

public:

  //! constructor
  SplashScreen(QWidget* parent, Preferences& preferences);

  //! destructor
  virtual ~SplashScreen();

private Q_SLOTS:

  //! new project clicked
  void new_project();

  //! open existing project
  void open_existing();

  //! open recent project
  void open_recent();

  //! quit
  void quit();

  //! enable the load recent button
  void enable_load_recent_button(QListWidgetItem* item);

Q_SIGNALS:

  //! signal to open a particular project
  void open_project(QString file_path);

private:

  //! populate the recent project list
  void populate_recent_projects();

  Ui_SplashScreen* ui_;

  Preferences& preferences_;
};

} // end namespace
