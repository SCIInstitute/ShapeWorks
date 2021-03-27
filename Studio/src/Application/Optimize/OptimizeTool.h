#pragma once

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include <QElapsedTimer>

#include <itkPoint.h>

#include <Data/Preferences.h>

class Ui_OptimizeTool;

class QLineEdit;

namespace shapeworks {
class QOptimize;
class OptimizeParameters;
class Session;


class OptimizeTool : public QWidget {
Q_OBJECT;

public:

  OptimizeTool();
  ~OptimizeTool();

  /// set the pointer to the project
  void set_session(QSharedPointer<Session> session);

  //! activate this tool
  void activate();

  //! Load params from project
  void load_params();
  //! Store params to project
  void store_params();

  //! Enable action buttons
  void enable_actions();
  //! Disable action buttons
  void disable_actions();

  //! shut down any running threads
  void shutdown_threads();

public Q_SLOTS:

  /// Run optimize tool
  void on_run_optimize_button_clicked();
  void on_restoreDefaults_clicked();
  void handle_optimize_complete();
  void handle_progress(int val, QString message);
  void handle_error(std::string);
  void handle_warning(std::string);
  void handle_message(std::string);

  void update_ui_elements();

  bool validate_inputs();

signals:
  void optimize_start();
  void optimize_complete();

  void error_message(std::string);
  void warning_message(std::string);
  void progress(int);
  void message(std::string);
  void status(std::string);

private:

  void update_run_button();

  void handle_load_progress(int count);

  void clear_particles();

  std::vector<QLineEdit*> line_edits_;

  QList<QThread*> threads_;
  bool optimization_is_running_ = false;
  QSharedPointer<QOptimize> optimize_;
  QSharedPointer<OptimizeParameters> optimize_parameters_;
  QSharedPointer<Session> session_;
  QElapsedTimer elapsed_timer_;

  Ui_OptimizeTool* ui_;
};
}