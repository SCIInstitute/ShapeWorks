#pragma once

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include <QElapsedTimer>

#include <itkPoint.h>

#include <Data/Preferences.h>
#include <Data/Telemetry.h>

class Ui_OptimizeTool;

class QLineEdit;

namespace shapeworks {
class QOptimize;
class OptimizeParameters;
class Session;


class OptimizeTool : public QWidget {
Q_OBJECT;

public:

  OptimizeTool(Preferences& prefs, Telemetry& telemetry);
  ~OptimizeTool();

  /// set the pointer to the project
  void set_session(QSharedPointer<Session> session);

  //! activate this tool
  void activate();

  //! Load params from project
  void load_params();
  //! Store params to project

  //! Get flag if regression needs to enabled for analysis in Studio
  inline bool get_regression_analysis_status() { return regression_analysis_status; }

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
  void handle_optimize_failed();
  void handle_progress(int val, QString message);
  void handle_error(QString);
  void handle_warning(QString);
  void handle_message(QString);

  void update_ui_elements();

  bool validate_inputs();

Q_SIGNALS:
  void optimize_start();
  void optimize_complete();

  void progress(int);
  void status(std::string);

private:

  void setup_domain_boxes();

  void update_run_button();

  void handle_load_progress(int count);

  std::vector<QLineEdit*> particle_boxes_;

  Preferences& preferences_;
  Telemetry& telemetry_;


  std::vector<QLineEdit*> line_edits_;

  QList<QThread*> threads_;
  bool optimization_is_running_ = false;
  QSharedPointer<QOptimize> optimize_;
  QSharedPointer<OptimizeParameters> optimize_parameters_;
  QSharedPointer<Session> session_;
  QElapsedTimer elapsed_timer_;

  Ui_OptimizeTool* ui_;

  bool regression_analysis_status_;
};
}
