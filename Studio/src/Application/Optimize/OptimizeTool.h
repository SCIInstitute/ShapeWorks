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

  OptimizeTool();
  ~OptimizeTool();

  /// set the pointer to the project
  void set_session(QSharedPointer<Session> session);

  void load_settings();
  void store_settings();

  void enable_actions();
  void disable_actions();

  void shutdown_threads();

public Q_SLOTS:

  /// Run optimize tool
  void on_run_optimize_button_clicked();
  void on_restoreDefaults_clicked();
  void handle_optimize_complete();
  void handle_progress(int val);
  void handle_error(std::string);
  void handle_warning(std::string);
  void handle_message(std::string);

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
  Ui_OptimizeTool* ui_;
  QSharedPointer<Session> session_;
};
