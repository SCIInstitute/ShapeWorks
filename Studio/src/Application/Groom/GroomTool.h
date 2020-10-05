#pragma once

#include <string>

#include <QWidget>
#include <QSharedPointer>
#include <QProgressDialog>
#include <QElapsedTimer>

#include <Groom/QGroom.h>

class Session;
class Ui_GroomTool;
class ShapeWorksStudioApp;

Q_DECLARE_METATYPE(std::string)

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool();
  ~GroomTool();

  /// set the pointer to the project
  void set_session(QSharedPointer<Session> session);

  //! Store settings to session
  void store_settings();

  //! Load settings from session
  void load_settings();

  //! Disable action buttons
  void disable_actions();

  //! Enable action buttons
  void enable_actions();

Q_SIGNALS:
  void groom_complete();
  void error_message(std::string);
  void message(std::string);
  void progress(size_t);

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged(int state);
  void on_blur_checkbox_stateChanged(int state);
  void on_autopad_checkbox_stateChanged(int state);
  void on_skipButton_clicked();
  void on_restoreDefaults_clicked();

  //! Run groom tool
  void on_run_groom_button_clicked();

  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error(std::string msg);

private:

  Ui_GroomTool* ui_;
  QSharedPointer<Session> session_;

  QGroom* groom_;

  QElapsedTimer timer_;
};
