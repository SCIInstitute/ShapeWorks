#pragma once

#include <Job/SharedBoundaryJob.h>

#include <QSharedPointer>
#include <QWidget>

class Ui_SharedBoundaryPanel;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;

class SharedBoundaryPanel : public QWidget {
  Q_OBJECT;

 public:
  SharedBoundaryPanel(QWidget* parent = 0);
  ~SharedBoundaryPanel() = default;

  void set_session(QSharedPointer<Session> session);
  void reset();
  void update_panel();

 public Q_SLOTS:

  void run_clicked();

  void handle_job_progress(int progress);
  void handle_job_complete();

 Q_SIGNALS:

 private:
  void update_run_button();

  QSharedPointer<Session> session_;
  QSharedPointer<SharedBoundaryJob> job_;

  Ui_SharedBoundaryPanel* ui_;
};
}  // namespace shapeworks
