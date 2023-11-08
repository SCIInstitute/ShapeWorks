// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Analysis/ShapeScalarPanel.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
#include <Job/GroupPvalueJob.h>
#include <Logging.h>
#include <Python/PythonWorker.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <jkqtplotter/graphs/jkqtpboxplot.h>
#include <jkqtplotter/graphs/jkqtpstatisticsadaptors.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_ShapeScalarPanel.h>

namespace shapeworks {

//---------------------------------------------------------------------------
ShapeScalarPanel::ShapeScalarPanel(QWidget* parent) : QWidget(parent), ui_(new Ui_ShapeScalarPanel) {
  ui_->setupUi(this);

  connect(ui_->open_button, &QPushButton::toggled, ui_->content, &QWidget::setVisible);
  connect(ui_->header, &QPushButton::clicked, ui_->open_button, &QPushButton::toggle);
  connect(ui_->run_button, &QPushButton::clicked, this, &ShapeScalarPanel::run_clicked);

  ui_->header_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->open_button->setChecked(false);
  ui_->progress->hide();

  ui_->jk_plot->hide();
  update_run_button();

  update_graphs();
}
//---------------------------------------------------------------------------
ShapeScalarPanel::~ShapeScalarPanel() {}

//---------------------------------------------------------------------------
void ShapeScalarPanel::set_session(QSharedPointer<Session> session) {
  session_ = session;
  reset();
}

//---------------------------------------------------------------------------
void ShapeScalarPanel::reset() {
  job_.reset();
  // populate feature combo box
  ui_->feature_combo->clear();
  if (session_) {
    for (const auto& feature : session_->get_project()->get_feature_names()) {
      ui_->feature_combo->addItem(QString::fromStdString(feature));
    }
  }
}

//---------------------------------------------------------------------------
void ShapeScalarPanel::run_clicked() {
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

  ShapeScalarJob::JobType job_type =
      ui_->find_components_mode ? ShapeScalarJob::JobType::Find_Components : ShapeScalarJob::JobType::MSE_Plot;

  job_ =
      QSharedPointer<ShapeScalarJob>::create(session_, ui_->feature_combo->currentText(), Eigen::MatrixXd{}, job_type);
  job_->set_number_of_components(ui_->num_components->text().toInt());
  job_->set_number_of_folds(ui_->num_folds->text().toInt());

  connect(job_.data(), &ShapeScalarJob::progress, this, &ShapeScalarPanel::handle_job_progress);
  connect(job_.data(), &ShapeScalarJob::finished, this, &ShapeScalarPanel::handle_job_complete);

  session_->get_py_worker()->run_job(job_);

  // re-enable after 1 second to prevent accidental double-clicks
  QTimer::singleShot(1000, this, [&]() { update_run_button(); });
}

//---------------------------------------------------------------------------
void ShapeScalarPanel::handle_job_progress(int progress) { ui_->progress->setValue(progress * 100); }

//---------------------------------------------------------------------------
void ShapeScalarPanel::handle_job_complete() {
  update_run_button();
  ui_->progress->hide();
  if (job_->is_aborted()) {
    job_.reset();
  } else {
    update_graphs();
  }
}

//---------------------------------------------------------------------------
void ShapeScalarPanel::update_graphs() {
  if (!job_) {
    ui_->plot->hide();
    return;
  }
  ui_->plot->show();

  auto pixmap = job_->get_plot();

  if (!pixmap.isNull()) {
    QPixmap resized = pixmap.scaledToWidth(this->width() * 0.95, Qt::SmoothTransformation);
    ui_->plot->setPixmap(resized);
  }
}

//---------------------------------------------------------------------------
void ShapeScalarPanel::update_run_button() {
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
