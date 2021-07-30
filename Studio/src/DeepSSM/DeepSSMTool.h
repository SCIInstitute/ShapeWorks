#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <ParticleShapeStatistics.h>
#include <Data/ShapeWorksWorker.h>

// Studio
#include <Data/Shape.h>
#include <Data/Preferences.h>
#include <Visualization/Visualizer.h>
#include <Visualization/BarGraph.h>
#include <Python/PythonWorker.h>

class Ui_DeepSSMTool;
class QLabel;

namespace shapeworks {

class Session;
class Lightbox;
class QDeepSSM;
class ShapeWorksStudioApp;

class DeepSSMTool : public QWidget {
Q_OBJECT;
public:

  DeepSSMTool(Preferences& prefs);
  ~DeepSSMTool();

  /// set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  /// set the pointer to the application
  void set_app(ShapeWorksStudioApp* app);

  //! Set if this tool is active
  void set_active(bool active);

  //! Return if this tool is active
  bool get_active();

  void load_params();
  void store_params();

  void shutdown();

  QVector<QSharedPointer<Shape>> get_shapes();

  void resizeEvent(QResizeEvent* event) override;

public Q_SLOTS:

  void run_clicked();
  void restore_defaults();

  void handle_thread_complete();

  void handle_progress(int val);
  void handle_error(QString msg);

  void tab_changed(int tab);

  void update_panels();

signals:

  void update_view();
  void progress(int);
  void message(QString);
  void error(QString);
  void warning(QString);

private:

  void run_tool(PythonWorker::JobType type);
  void update_data();
  void update_training_table();
  void update_validation_examples();
  void load_plots();
  void resize_plots();
  QPixmap load_plot(QString filename);
  void set_plot(QLabel *qlabel, QPixmap pixmap);

  Preferences& preferences_;

  Ui_DeepSSMTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  bool tool_is_running_ = false;
  PythonWorker::JobType current_tool_ = PythonWorker::JobType::DeepSSM_AugmentationType;
  QSharedPointer<QDeepSSM> deep_ssm_;
  QElapsedTimer timer_;

  QVector<QSharedPointer<Shape>> shapes_;
  QPixmap violin_plot_;
  QPixmap training_plot_;

  QSharedPointer<PythonWorker> py_worker;

};

}
