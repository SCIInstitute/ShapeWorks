#pragma once

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include <QElapsedTimer>

#include <itkPoint.h>

#include <Data/Preferences.h>
#include <Data/Telemetry.h>

class Ui_OptimizeTool;

class QCheckBox;
class QLabel;
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

  void handle_session_modified();

  bool validate_inputs();

Q_SIGNALS:
  void optimize_start();
  void optimize_complete();

  void progress(int);
  void status(std::string);

private:

  void setup_domain_boxes();

  void setup_mesh_scalar_boxes();

  void update_run_button();

  void handle_load_progress(int count);

  std::vector<QLineEdit*> particle_boxes_;
  std::vector<QWidget*> domain_grid_widgets_;

  //! one row per scalar field the meshes carry: name, "use it" box, and its weight
  std::vector<std::string> mesh_scalar_names_;
  std::vector<QCheckBox*> mesh_scalar_checks_;
  std::vector<QLabel*> mesh_scalar_weight_labels_;
  std::vector<QLineEdit*> mesh_scalar_weights_;
  std::vector<QWidget*> mesh_scalar_widgets_;

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
};
}
