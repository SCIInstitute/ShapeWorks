#pragma once

#include <string>

#include <QWidget>
#include <QSharedPointer>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QObject>

#include <Data/Preferences.h>
#include <Groom/QGroom.h>
#include <Libs/Groom/GroomParameters.h>

class Ui_GroomTool;

Q_DECLARE_METATYPE(std::string)

namespace shapeworks {

class Session;
class ShapeWorksStudioApp;

class GroomTool : public QWidget {
Q_OBJECT;
public:

  GroomTool(Preferences& prefs);
  ~GroomTool();

  //! Set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  //! activate this tool
  void activate();

  //! Load params from project
  void load_params();
  //! Store params to project
  void store_params();

  //! Disable action buttons
  void disable_actions();
  //! Enable action buttons
  void enable_actions();

  //! shut down any running threads
  void shutdown_threads();

Q_SIGNALS:
  void groom_start();
  void groom_complete();
  void error_message(QString);
  void message(QString);
  void progress(int);

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged(int state);
  void on_blur_checkbox_stateChanged(int state);
  void on_autopad_checkbox_stateChanged(int state);
  void on_skip_button_clicked();
  void on_restore_defaults_clicked();

  void update_ui();

  void domain_changed();

  void alignment_checkbox_changed(int state);
  void alignment_option_changed(int index);

  void fill_holes_changed(int state);

  //! Run groom tool
  void on_run_groom_button_clicked();

  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error(QString msg);

private:

  void set_ui_from_params(GroomParameters params);

  void update_page();

  void update_reflect_columns();
  void update_reflect_choices();

  Preferences& preferences_;

  QList<QThread*> threads_;

  Ui_GroomTool* ui_;
  QSharedPointer<Session> session_;

  QSharedPointer<shapeworks::QGroom> groom_;

  QElapsedTimer timer_;

  bool groom_is_running_ = false;

  std::string current_domain_;

  QStringList reflect_columns_;

};
}
