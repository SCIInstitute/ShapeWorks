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
#include <jkqtplotter/graphs/jkqtpboxplot.h>
#include <jkqtplotter/graphs/jkqtpstatisticsadaptors.h>
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

  connect(ui_->mean_radio, &QRadioButton::clicked, this, &ParticleAreaPanel::display_option_changed);
  connect(ui_->stddev_radio, &QRadioButton::clicked, this, &ParticleAreaPanel::display_option_changed);

  update_graphs();
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
Eigen::VectorXf ParticleAreaPanel::get_computed_values() const {
  if (!job_) {
    return Eigen::VectorXf();
  }
  if (ui_->mean_radio->isChecked()) {
    return job_->get_mean_areas();
  } else {
    return job_->get_stddev_areas();
  }
}

//---------------------------------------------------------------------------
std::string ParticleAreaPanel::get_computed_value_name() const {
  if (ui_->mean_radio->isChecked()) {
    return "Mean area";
  } else {
    return "Stddev of area";
  }
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
void ParticleAreaPanel::show_particle_area_clicked() {
  update_graphs();
  session_->trigger_reinsert_shapes();
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::display_option_changed() {
  update_graphs();
  session_->trigger_reinsert_shapes();
}

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

    update_graphs();
  }
}

//---------------------------------------------------------------------------
void ParticleAreaPanel::update_graphs() {
  if (!job_) {
    ui_->boxplot->hide();
    return;
  }
  ui_->boxplot->show();

  auto plot = ui_->boxplot->getPlotter();

  plot->clearGraphs();

  Eigen::VectorXf numbers;
  QString x_label = QString("Mean area");
  if (ui_->mean_radio->isChecked()) {
    numbers = job_->get_mean_areas();
  } else {
    x_label = QString("Stddev of area");
    numbers = job_->get_stddev_areas();
  }

  QVector<double> values;
//  for (int i = 0; i < numbers.size(); ++i) {
    for (int i = 0; i < 100000; ++i) {
    // values.push_back(numbers[i]);

    // fill with random data
    values.push_back((double)rand() / RAND_MAX);
  }

  // fill in datastore
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();
  size_t column_x = ds->addCopiedColumn(values, x_label);

  // jkqtpstatAddVBoxplotAndOutliers(JKQTBasePlotter* plotter, InputIt first, InputIt last, double boxposX, double
  // quantile1Spec=0.25, double quantile2Spec=0.75, double minimumQuantile=0.03, double maximumQuantile=0.97, const
  // QString& outliercolumnBaseName=QString("boxplot"), JKQTPStat5NumberStatistics* statOutput=nullptr) {

  JKQTPStat5NumberStatistics stat =
      jkqtpstat5NumberStatistics(ds->begin(column_x), ds->end(column_x), 0.25, 0.75, 0.03, 0.97);

  std::cerr << "median: " << stat.median << "\n";
  std::cerr << "lower quartile: " << stat.quantile1 << "\n";
  std::cerr << "upper quartile: " << stat.quantile2 << "\n";
  std::cerr << "minimum: " << stat.minimum << "\n";
  std::cerr << "maximum: " << stat.maximum << "\n";
  for (auto& outlier : stat.outliers) {
    std::cerr << "outlier: " << outlier << "\n";
  }

  auto pair = jkqtpstatAddVBoxplotAndOutliers(plot, ds->begin(column_x), ds->end(column_x), 0, 0.25, 0.75, 0.03, 0.97,
                                              "outliers");

  auto graph = pair.first;
  auto outliers = pair.second;

  // set color to blue
  graph->setColor(Qt::blue);
  outliers->setColor(Qt::blue);

  plot->zoomToFit();

  SW_LOG("draw box plot");

  // graph->setXColumn(column_x);
  // plot->addGraph(graph);
  // plot->setXLabel(x_label);

  //    jkqtpstatAddVBoxplotsAndOutliers(plot, )
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
