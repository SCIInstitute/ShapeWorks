// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Analysis/ParticleAreaPanel.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
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
  connect(ui_->header, &QPushButton::clicked, ui_->open_button, &QPushButton::toggle);

  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->open_button->setChecked(false);
  ui_->progress->hide();

  ui_->show_particle_area->setEnabled(false);

  update_run_button();

  connect(ui_->run_button, &QPushButton::clicked, this, &ParticleAreaPanel::run_clicked);
  connect(ui_->show_particle_area, &QPushButton::clicked, this, &ParticleAreaPanel::show_particle_area_clicked);
}
//---------------------------------------------------------------------------
ParticleAreaPanel::~ParticleAreaPanel() {}

//---------------------------------------------------------------------------
void ParticleAreaPanel::set_session(QSharedPointer<Session> session) {
  session_ = session;
  reset();
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::reset() {
  job_.reset();
  ui_->show_particle_area->setEnabled(false);
}

//---------------------------------------------------------------------------
bool ParticleAreaPanel::get_display_particle_area() const {
  return ui_->show_particle_area->isChecked() && ui_->show_particle_area->isEnabled();
}

//---------------------------------------------------------------------------
Eigen::VectorXf ParticleAreaPanel::get_mean_areas() const {
  if (!job_) {
    return Eigen::VectorXf();
  }
  return job_->get_mean_areas();
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::run_clicked() {
  if (!session_) {
    return;
  }

  // ensure someone doesn't accidentally abort right after clicking RUN
  ui_->run_button->setEnabled(false);

  if (job_ && !job_->is_complete()) {
    ui_->run_button->setText("Aborting...");
    job_->abort();
    SW_LOG("Aborting {}", job_->name());
    return;
  }

  ui_->progress->show();
  handle_job_progress(0);

  job_ = QSharedPointer<ParticleAreaJob>::create(session_);
  connect(job_.data(), &ParticleAreaJob::progress, this, &ParticleAreaPanel::handle_job_progress);
  connect(job_.data(), &ParticleAreaJob::finished, this, &ParticleAreaPanel::handle_job_complete);
  auto worker = Worker::create_worker();
  worker->run_job(job_);

  // re-enable after 1 second to prevent accidental double-clicks
  QTimer::singleShot(1000, this, [&]() { update_run_button(); });
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::show_particle_area_clicked() { session_->trigger_reinsert_shapes(); }

//---------------------------------------------------------------------------
void ParticleAreaPanel::handle_job_progress(int progress) { ui_->progress->setValue(progress * 100); }

//---------------------------------------------------------------------------
void ParticleAreaPanel::handle_job_complete() {
  update_run_button();
  ui_->progress->hide();
  if (job_->is_aborted()) {
    job_.reset();
  } else {
    ui_->show_particle_area->setEnabled(true);
    ui_->show_particle_area->setChecked(true);
    session_->trigger_reinsert_shapes();
  }
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::update_run_button() {
  ui_->run_button->setEnabled(true);
  if (job_ && !job_->is_complete()) {
    Style::apply_abort_button_style(ui_->run_button);
    ui_->run_button->setText("Abort");
    ui_->run_button->setEnabled(true);
  } else {
    Style::apply_normal_button_style(ui_->run_button);
    ui_->run_button->setText("Run");
    ui_->run_button->setEnabled(true);
  }
}
}  // namespace shapeworks
