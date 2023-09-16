#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <ParticleShapeStatistics.h>

// Studio
#include <Data/Preferences.h>
#include <Job/ParticleAreaJob.h>
#include <Shape.h>
#include <Visualization/Visualizer.h>

class Ui_ParticleAreaPanel;
class JKQTPlotter;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class ParticleAreaPanel : public QWidget {
  Q_OBJECT;

 public:
  ParticleAreaPanel(QWidget* parent = 0);
  ~ParticleAreaPanel();

  //! set the pointer to the session
  void set_session(QSharedPointer<Session> session);

 public Q_SLOTS:

  void run_clicked();
  void show_particle_area_clicked();

  void handle_job_progress(int);
  void handle_job_complete();

 Q_SIGNALS:

  void update_view();

 private:
  QSharedPointer<Session> session_;
  QSharedPointer<ParticleAreaJob> job_;

  Ui_ParticleAreaPanel* ui_;
};
}  // namespace shapeworks
