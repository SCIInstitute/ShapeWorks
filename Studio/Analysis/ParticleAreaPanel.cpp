// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Analysis/ParticleAreaPanel.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Job/GroupPvalueJob.h>
#include <Job/NetworkAnalysisJob.h>
#include <Job/ParticleNormalEvaluationJob.h>
#include <Job/StatsGroupLDAJob.h>
#include <Logging.h>
#include <Python/PythonWorker.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_ParticleAreaPanel.h>

namespace shapeworks {

//---------------------------------------------------------------------------
ParticleAreaPanel::ParticleAreaPanel(QWidget* parent) : QWidget(parent), ui_(new Ui_ParticleAreaPanel) {
  ui_->setupUi(this);

  connect(ui_->open_button, &QPushButton::toggled, ui_->content, &QWidget::setVisible);

  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_->open_button->setChecked(false);

  connect(ui_->run_button, &QPushButton::clicked, this, &ParticleAreaPanel::run_clicked);
  connect(ui_->show_particle_area, &QPushButton::clicked, this, &ParticleAreaPanel::show_particle_area_clicked);
}
//---------------------------------------------------------------------------
ParticleAreaPanel::~ParticleAreaPanel() {}

//---------------------------------------------------------------------------
void ParticleAreaPanel::set_session(QSharedPointer<Session> session) { session_ = session; }

//---------------------------------------------------------------------------
void ParticleAreaPanel::run_clicked() {
  if (!session_) {
    return;
  }
  job_ = QSharedPointer<ParticleAreaJob>::create(session_);

  connect(job_.data(), &ParticleAreaJob::progress, this, &ParticleAreaPanel::handle_job_progress);
  connect(job_.data(), &ParticleAreaJob::finished, this, &ParticleAreaPanel::handle_job_complete);

  auto worker = Worker::create_worker();
  worker->run_job(job_);
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::show_particle_area_clicked() {
  SW_DEBUG("Show particle area clicked: {}", ui_->show_particle_area->isChecked());
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::handle_job_progress(int) {}

//---------------------------------------------------------------------------
void ParticleAreaPanel::handle_job_complete() {}

}  // namespace shapeworks
