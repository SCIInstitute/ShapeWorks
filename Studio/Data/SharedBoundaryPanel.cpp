// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
#include <Logging.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <Utils/StudioUtils.h>
#include <ui_SharedBoundaryPanel.h>

#include "SharedBoundaryPanel.h"

namespace shapeworks {

//---------------------------------------------------------------------------
SharedBoundaryPanel::SharedBoundaryPanel(QWidget* parent) : QWidget(parent), ui_(new Ui_SharedBoundaryPanel) {
  ui_->setupUi(this);

  connect(ui_->open_button, &QPushButton::toggled, ui_->content, &QWidget::setVisible);
  connect(ui_->header, &QPushButton::clicked, ui_->open_button, &QPushButton::toggle);

  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->open_button->setChecked(false);
  ui_->progress->hide();

  update_run_button();

  connect(ui_->run_button, &QPushButton::clicked, this, &SharedBoundaryPanel::run_clicked);
}

//---------------------------------------------------------------------------
void SharedBoundaryPanel::set_session(QSharedPointer<Session> session) {
  session_ = session;
  update_panel();
  reset();
}

//---------------------------------------------------------------------------
void SharedBoundaryPanel::reset() { job_.reset(); }

//---------------------------------------------------------------------------
void SharedBoundaryPanel::update_panel() {
  if (!session_) {
    return;
  }
  auto domain_names = session_->get_project()->get_domain_names();
  // only show when there are at least two domains
  setVisible(domain_names.size() > 1);

  StudioUtils::update_domain_combobox(ui_->shared_boundary_first_domain, session_);
  StudioUtils::update_domain_combobox(ui_->shared_boundary_second_domain, session_);
  if (ui_->shared_boundary_first_domain->currentIndex() == 0 &&
      ui_->shared_boundary_second_domain->currentIndex() == 0) {
    ui_->shared_boundary_second_domain->setCurrentIndex(1);
  }
}

//---------------------------------------------------------------------------
void SharedBoundaryPanel::run_clicked() {
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

  int first_domain = ui_->shared_boundary_first_domain->currentIndex();
  int second_domain = ui_->shared_boundary_second_domain->currentIndex();
  job_ = QSharedPointer<SharedBoundaryJob>::create(session_, first_domain, second_domain);
  connect(job_.data(), &SharedBoundaryJob::progress, this, &SharedBoundaryPanel::handle_job_progress);
  connect(job_.data(), &SharedBoundaryJob::finished, this, &SharedBoundaryPanel::handle_job_complete);
  auto worker = Worker::create_worker();
  worker->run_job(job_);

  // re-enable after 1 second to prevent accidental double-clicks
  QTimer::singleShot(1000, this, [&]() { update_run_button(); });
}

//---------------------------------------------------------------------------
void SharedBoundaryPanel::handle_job_progress(int progress) { ui_->progress->setValue(progress * 100); }

//---------------------------------------------------------------------------
void SharedBoundaryPanel::handle_job_complete() {
  update_run_button();
  ui_->progress->hide();
  if (job_->is_aborted()) {
    job_.reset();
  } else {
    // ???
  }
}

//---------------------------------------------------------------------------
void SharedBoundaryPanel::update_run_button() {
  ui_->run_button->setEnabled(true);
  if (job_ && !job_->is_complete()) {
    Style::apply_abort_button_style(ui_->run_button);
    ui_->run_button->setText("Abort");
    ui_->run_button->setEnabled(true);
  } else {
    Style::apply_normal_button_style(ui_->run_button);
    ui_->run_button->setText("Generate");
    ui_->run_button->setEnabled(true);
  }
}
}  // namespace shapeworks
