#pragma once

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>

#include <itkPoint.h>

#include <Data/Preferences.h>

class Session;
class Ui_OptimizeTool;
class QOptimize;

class OptimizeTool : public QWidget
{
  Q_OBJECT;
public:

  OptimizeTool(Preferences& prefs);
  ~OptimizeTool();

  /// set the pointer to the project
  void set_project(QSharedPointer<Session> project);

  void set_preferences(bool setScales = false);

  void update_preferences();
  void enableActions();
  void disableActions();
  void setCutPlanesFile(std::string file);
  std::string getCutPlanesFile();

  void shutdown_threads();

public Q_SLOTS:

  /// Run optimize tool
  void on_run_optimize_button_clicked();
  void on_cutPlanesFile_editingFinished();
  void on_cutPlanesFileButton_clicked();
  void on_restoreDefaults_clicked();
  void on_number_of_particles_valueChanged(int val);
  void handle_optimize_complete();
  void handle_progress(int val);
  void handle_error(std::string);
  void handle_warning(std::string);
  void meshDecimation_valueChanged(double v);
  void numClusters_valueChanged(int v);
  void on_weight_valueChanged(double v);
  void maxAngle_valueChanged(double v);
  void on_starting_regularization_valueChanged(double v);
  void on_ending_regularization_valueChanged(double v);
  void on_iterations_valueChanged(int v);
  void decay_span_valueChanged(int v);
  void on_procrustes_interval_valueChanged(int v);
  void handle_message(std::string);

private:
  void loadCutPlanesFile(std::string file);

signals:
  void optimize_start();
  void optimize_complete();

  void error_message(std::string);
  void warning_message(std::string);
  void progress(size_t);
  void message(std::string);

private:
  QList<QThread*> threads_;
  bool optimization_is_running_ = false;
  QOptimize* optimize_ = nullptr;
  Preferences& preferences_;
  Ui_OptimizeTool* ui_;
  QSharedPointer<Session> session_;
  std::vector<std::array<itk::Point<double>, 3 >> cutPlanes_;
};
