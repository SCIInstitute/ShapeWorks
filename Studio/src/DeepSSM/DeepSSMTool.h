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

class Ui_DeepSSMTool;

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

  void run_augmentation_clicked();
  void restore_augmentation_defaults();
  void run_training_clicked();
  void restore_training_defaults();

  void handle_thread_complete();

  void handle_progress(int val);
  void handle_error(QString msg);

  void update_panels();

signals:

  void update_view();
  void progress(int);
  void message(QString);
  void error(QString);
  void warning(QString);

private:

  void run_tool(ShapeworksWorker::ThreadType type);
  void update_data();
  void load_violin_plot();
  void resize_plot();

  Preferences& preferences_;

  Ui_DeepSSMTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  bool tool_is_running_ = false;
  ShapeworksWorker::ThreadType current_tool_ = ShapeworksWorker::DeepSSM_AugmentationType;
  QSharedPointer<QDeepSSM> deep_ssm_;
  QElapsedTimer timer_;

  QVector<QSharedPointer<Shape>> shapes_;
  QPixmap violin_plot_;

};

}
