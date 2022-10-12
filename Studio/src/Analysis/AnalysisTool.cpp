// std
#include <fstream>
#include <iostream>

// qt
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Analysis/AnalysisTool.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Job/GroupPvalueJob.h>
#include <Job/ParticleNormalEvaluationJob.h>
#include <Job/StatsGroupLDAJob.h>
#include <Logging.h>
#include <Python/PythonWorker.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <Visualization/Lightbox.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_AnalysisTool.h>

namespace shapeworks {

const std::string AnalysisTool::MODE_ALL_SAMPLES_C("all samples");
const std::string AnalysisTool::MODE_MEAN_C("mean");
const std::string AnalysisTool::MODE_PCA_C("pca");
const std::string AnalysisTool::MODE_SINGLE_SAMPLE_C("single sample");
const std::string AnalysisTool::MODE_REGRESSION_C("regression");

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_AnalysisTool;
  ui_->setupUi(this);

#ifdef Q_OS_MACOS
  ui_->tabWidget->tabBar()->setMinimumWidth(200);
  ui_->tabWidget->setStyleSheet(
      "QTabBar::tab:selected { border-radius: 4px; background-color: #3784f7; color: white; }");
  ui_->tabWidget->tabBar()->setElideMode(Qt::TextElideMode::ElideNone);
#endif

  stats_ready_ = false;

  connect(ui_->view_header, &QPushButton::clicked, ui_->view_open_button, &QPushButton::toggle);
  connect(ui_->metrics_header, &QPushButton::clicked, ui_->metrics_open_button, &QPushButton::toggle);
  connect(ui_->surface_header, &QPushButton::clicked, ui_->surface_open_button, &QPushButton::toggle);
  connect(ui_->particles_header, &QPushButton::clicked, ui_->particles_open_button, &QPushButton::toggle);
  connect(ui_->particles_open_button, &QPushButton::toggled, ui_->particles_content, &QWidget::setVisible);

  ui_->view_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->surface_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->metrics_label->setAttribute(Qt::WA_TransparentForMouseEvents);

  connect(ui_->allSamplesRadio, SIGNAL(clicked()), this, SLOT(handle_analysis_options()));
  connect(ui_->singleSamplesRadio, SIGNAL(clicked()), this, SLOT(handle_analysis_options()));
  connect(ui_->sampleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(handle_analysis_options()));
  connect(ui_->medianButton, SIGNAL(clicked()), this, SLOT(handle_median()));

  connect(ui_->pcaAnimateCheckBox, SIGNAL(stateChanged(int)), this, SLOT(handle_pca_animate_state_changed()));
  connect(&pca_animate_timer_, SIGNAL(timeout()), this, SLOT(handle_pca_timer()));

  // group animation
  connect(ui_->group_animate_checkbox, &QCheckBox::stateChanged, this,
          &AnalysisTool::handle_group_animate_state_changed);
  connect(&group_animate_timer_, &QTimer::timeout, this, &AnalysisTool::handle_group_timer);

  connect(ui_->group_box, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::update_group_values);
  connect(ui_->group_left, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::group_changed);
  connect(ui_->group_right, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::group_changed);
  connect(ui_->group_p_values_button, &QPushButton::clicked, this, &AnalysisTool::group_p_values_clicked);

  connect(ui_->reference_domain, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &AnalysisTool::handle_alignment_changed);

  ui_->surface_open_button->setChecked(false);
  ui_->metrics_open_button->setChecked(false);

  /// TODO nothing there yet (regression tab)
  ui_->tabWidget->removeTab(3);

  for (auto button : {ui_->distance_transfom_radio_button, ui_->mesh_warping_radio_button, ui_->legacy_radio_button}) {
    connect(button, &QRadioButton::clicked, this, &AnalysisTool::reconstruction_method_changed);
  }

  connect(ui_->run_good_bad, &QPushButton::clicked, this, &AnalysisTool::run_good_bad_particles);

  ui_->reconstruction_options->hide();
  ui_->particles_open_button->toggle();
  ui_->particles_progress->hide();

  ui_->lda_panel->hide();
  ui_->lda_graph->hide();
  ui_->lda_hint_label->hide();
  group_lda_job_ = QSharedPointer<StatsGroupLDAJob>::create();
  connect(group_lda_job_.data(), &StatsGroupLDAJob::progress, this, &AnalysisTool::handle_lda_progress);
  connect(group_lda_job_.data(), &StatsGroupLDAJob::finished, this, &AnalysisTool::handle_lda_complete);

  connect(ui_->show_difference_to_mean, &QPushButton::clicked, this, &AnalysisTool::show_difference_to_mean_clicked);
}

//---------------------------------------------------------------------------
std::string AnalysisTool::get_analysis_mode() {
  if (ui_->tabWidget->currentWidget() == ui_->samples_tab) {
    if (ui_->allSamplesRadio->isChecked()) {
      return AnalysisTool::MODE_ALL_SAMPLES_C;
    }
    if (ui_->singleSamplesRadio->isChecked()) {
      return AnalysisTool::MODE_SINGLE_SAMPLE_C;
    }
  }

  if (ui_->tabWidget->currentWidget() == ui_->mean_tab) {
    return AnalysisTool::MODE_MEAN_C;
  }
  if (ui_->tabWidget->currentWidget() == ui_->pca_tab) {
    return AnalysisTool::MODE_PCA_C;
  }
  if (ui_->tabWidget->currentWidget() == ui_->regression_tab) {
    return AnalysisTool::MODE_REGRESSION_C;
  }
  return "";
}

//---------------------------------------------------------------------------
bool AnalysisTool::get_group_difference_mode() { return ui_->difference_button->isChecked(); }

//---------------------------------------------------------------------------
std::vector<Shape::Point> AnalysisTool::get_group_difference_vectors() {
  std::vector<Shape::Point> vecs;

  auto num_points = stats_.Mean().size() / 3;
  for (unsigned int i = 0; i < num_points; i++) {
    Shape::Point tmp;
    tmp.x = stats_.Group2Mean()[i * 3] - stats_.Group1Mean()[i * 3];
    tmp.y = stats_.Group2Mean()[i * 3 + 1] - stats_.Group1Mean()[i * 3 + 1];
    tmp.z = stats_.Group2Mean()[i * 3 + 2] - stats_.Group1Mean()[i * 3 + 2];
    vecs.push_back(tmp);
  }
  return vecs;
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_reconstruction_complete() {
  session_->handle_clear_cache();

  session_->calculate_reconstructed_samples();
  Q_EMIT progress(100);
  SW_LOG("Reconstruction Complete");
  Q_EMIT reconstruction_complete();
  /// TODO: Studio
  /// ui_->run_optimize_button->setEnabled(true);

  enable_actions();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_reconstructionButton_clicked() {
  store_settings();
  SW_LOG("Please wait: running reconstruction step...");
  Q_EMIT progress(5);
  QThread* thread = new QThread;

  ShapeworksWorker* worker =
      new ShapeworksWorker(ShapeworksWorker::ReconstructType, nullptr, nullptr, nullptr, session_,
                           ui_->maxAngle->value(), ui_->meshDecimation->value(), ui_->numClusters->value());
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_reconstruction_complete()));

  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
  Q_EMIT progress(15);
}

//---------------------------------------------------------------------------
int AnalysisTool::get_pca_mode() { return ui_->pcaModeSpinBox->value() - 1; }

//---------------------------------------------------------------------------
double AnalysisTool::get_group_ratio() {
  double group_slider_value = ui_->group_slider->value();
  double group_ratio = group_slider_value / static_cast<double>(ui_->group_slider->maximum());
  return group_ratio;
}

//---------------------------------------------------------------------------
bool AnalysisTool::pca_animate() { return ui_->pcaAnimateCheckBox->isChecked(); }

//---------------------------------------------------------------------------
void AnalysisTool::set_labels(QString which, QString value) {
  if (which == QString("pca")) {
    ui_->pcaValueLabel->setText(value);
  } else if (which == QString("eigen")) {
    ui_->pcaEigenValueLabel->setText(value);
  } else if (which == QString("lambda")) {
    ui_->pcaLambdaLabel->setText(value);
  }
}

//---------------------------------------------------------------------------
int AnalysisTool::get_sample_number() { return ui_->sampleSpinBox->value(); }

//---------------------------------------------------------------------------
AnalysisTool::~AnalysisTool() {}

//---------------------------------------------------------------------------
void AnalysisTool::set_session(QSharedPointer<Session> session) {
  session_ = session;
  // reset to original
  ui_->mesh_warping_radio_button->setChecked(true);
  ui_->difference_button->setChecked(false);
  ui_->group_p_values_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  update_difference_particles();

  connect(ui_->show_good_bad, &QCheckBox::toggled, session_.data(), &Session::set_show_good_bad_particles);
}

//---------------------------------------------------------------------------
QSharedPointer<Session> AnalysisTool::get_session() { return session_; }

//---------------------------------------------------------------------------
void AnalysisTool::set_app(ShapeWorksStudioApp* app) { app_ = app; }

//---------------------------------------------------------------------------
void AnalysisTool::update_analysis_mode() { handle_analysis_options(); }

//---------------------------------------------------------------------------
void AnalysisTool::update_interface() {
  ui_->show_good_bad->setEnabled(session_->get_good_bad_particles().size() == session_->get_num_particles());
  ui_->show_good_bad->setChecked(ui_->show_good_bad->isEnabled() && session_->get_show_good_bad_particles());
}

//---------------------------------------------------------------------------
bool AnalysisTool::group_pvalues_valid() {
  return group_pvalue_job_ && group_pvalue_job_->get_group_pvalues().rows() > 0;
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape() {}

//---------------------------------------------------------------------------
void AnalysisTool::handle_analysis_options() {
  if (ui_->tabWidget->currentWidget() == ui_->samples_tab) {
    ui_->pcaAnimateCheckBox->setChecked(false);
    ui_->pcaAnimateCheckBox->setEnabled(false);
    ui_->pcaModeSpinBox->setEnabled(false);
    pca_animate_timer_.stop();
    ui_->pcaSlider->setEnabled(false);
    if (ui_->singleSamplesRadio->isChecked()) {
      // one sample mode
      ui_->sampleSpinBox->setEnabled(true);
      ui_->medianButton->setEnabled(true);
    } else {
      // all samples mode
      ui_->sampleSpinBox->setEnabled(false);
      ui_->medianButton->setEnabled(false);
    }
  } else if (ui_->tabWidget->currentWidget() == ui_->mean_tab) {
    // mean mode
    ui_->sampleSpinBox->setEnabled(false);
    ui_->medianButton->setEnabled(false);
    ui_->pcaSlider->setEnabled(false);
    ui_->pcaAnimateCheckBox->setEnabled(false);
    ui_->pcaModeSpinBox->setEnabled(false);
    ui_->pcaAnimateCheckBox->setChecked(false);
    pca_animate_timer_.stop();
  } else if (ui_->tabWidget->currentWidget() == ui_->pca_tab) {
    // pca mode
    ui_->sampleSpinBox->setEnabled(false);
    ui_->medianButton->setEnabled(false);
    ui_->pcaSlider->setEnabled(true);
    ui_->pcaAnimateCheckBox->setEnabled(true);
    ui_->pcaModeSpinBox->setEnabled(true);
  } else {
    // regression mode
    ui_->sampleSpinBox->setEnabled(false);
    ui_->medianButton->setEnabled(false);
    ui_->pcaSlider->setEnabled(false);
    ui_->pcaAnimateCheckBox->setEnabled(false);
    ui_->pcaModeSpinBox->setEnabled(false);
    pca_animate_timer_.stop();
  }

  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_median() {
  if (!compute_stats()) {
    return;
  }
  ui_->sampleSpinBox->setValue(stats_.ComputeMedianShape(-32));  //-32 = both groups
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_mean_button_clicked() {
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->difference_button->setChecked(false);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->mean_button->setChecked(true);
  update_difference_particles();
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group1_button_clicked() {
  ui_->group_slider->setValue(ui_->group_slider->minimum());
  ui_->mean_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->difference_button->setChecked(false);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->group1_button->setChecked(true);
  ui_->group_p_values_button->setChecked(false);
  update_difference_particles();
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group2_button_clicked() {
  ui_->group_slider->setValue(ui_->group_slider->maximum());
  ui_->mean_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->difference_button->setChecked(false);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->group2_button->setChecked(true);
  ui_->group_p_values_button->setChecked(false);
  update_difference_particles();
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_difference_button_clicked() {
  ui_->group_slider->setValue(ui_->group_slider->minimum());
  ui_->mean_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->group_p_values_button->setChecked(false);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->difference_button->setChecked(true);
  update_difference_particles();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_p_values_clicked() {
  ui_->group_slider->setValue(10);
  ui_->mean_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->group_p_values_button->setChecked(true);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->difference_button->setChecked(false);
  update_difference_particles();

  if (group_pvalues_valid()) {
    handle_group_pvalues_complete();
  } else {
    if (group1_list_.size() < 3 || group2_list_.size() < 3) {
      SW_ERROR("Unable to compute p-values with less than 3 shapes per group");
      return;
    }
    group_pvalue_job_ = QSharedPointer<GroupPvalueJob>::create(stats_);
    connect(group_pvalue_job_.data(), &GroupPvalueJob::progress, this, &AnalysisTool::progress);
    connect(group_pvalue_job_.data(), &GroupPvalueJob::finished, this, &AnalysisTool::handle_group_pvalues_complete);
    app_->get_py_worker()->run_job(group_pvalue_job_);
  }
}

//-----------------------------------------------------------------------------
bool AnalysisTool::compute_stats() {
  if (stats_ready_) {
    return true;
  }

  if (session_->get_shapes().size() == 0 || !session_->particles_present()) {
    return false;
  }

  compute_reconstructed_domain_transforms();

  ui_->pcaModeSpinBox->setMaximum(std::max<double>(1, session_->get_shapes().size() - 1));

  std::vector<Eigen::VectorXd> points;
  std::vector<int> group_ids;

  std::string group_set = ui_->group_box->currentText().toStdString();
  std::string left_group = ui_->group_left->currentText().toStdString();
  std::string right_group = ui_->group_right->currentText().toStdString();

  bool groups_enabled = groups_active();

  group1_list_.clear();
  group2_list_.clear();

  for (auto& shape : session_->get_shapes()) {
    if (groups_enabled) {
      auto value = shape->get_subject()->get_group_value(group_set);
      if (value == left_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(1);
        group1_list_.push_back(shape);
      } else if (value == right_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(2);
        group2_list_.push_back(shape);
      } else {
        // we don't include it
      }
    } else {
      points.push_back(shape->get_global_correspondence_points());
      group_ids.push_back(1);
    }
  }

  if (points.empty()) {
    return false;
  }

  // consistency check
  size_t point_size = points[0].size();
  for (auto&& p : points) {
    if (p.size() != point_size) {
      SW_ERROR("Inconsistency in data, particle files must contain the same number of points");
      return false;
    }
  }

  stats_.ImportPoints(points, group_ids);
  stats_.ComputeModes();

  update_difference_particles();

  compute_shape_evaluations();

  stats_ready_ = true;
  std::vector<double> vals;
  for (int i = stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(stats_.Eigenvalues()[i]);
  }
  ////  Uncomment this to write out long format sample data
  /*
  if (groups_enabled) {
    auto feature_names = session_->get_project()->get_feature_names();
    std::ofstream file;
    file.open("/tmp/stats.csv");
    file << "subject,group,particle,x,y,z";
    for (int i=0;i<feature_names.size();i++) {
      file << "," << feature_names[i];
    }
    file << "\n";

    int shape_id = 0;
    Q_FOREACH (ShapeHandle shape, session_->get_shapes()) {
      auto group = shape->get_subject()->get_group_value(group_set);
      auto particles = shape->get_particles();
      auto points = particles.get_world_points(0);
      int point_id = 0;
      for (int j=0;j<points.size(); j++) {
        auto point = points[j];
        file << shape_id << "," << group << "," << point_id++ << "," << point[0] << "," << point[1] << "," << point[2];

        for (int i=0;i<feature_names.size();i++) {
          auto scalars = shape->get_point_features(feature_names[i]);
          file << "," << scalars[j];
        }
        file << "\n";
      }

      shape_id++;
    }
    file.close();
  }
  */
  return true;
}

//-----------------------------------------------------------------------------
Particles AnalysisTool::get_mean_shape_points() {
  if (!compute_stats()) {
    return Particles();
  }

  if (ui_->group1_button->isChecked() || ui_->difference_button->isChecked()) {
    return convert_from_combined(stats_.Group1Mean());
  } else if (ui_->group2_button->isChecked()) {
    return convert_from_combined(stats_.Group2Mean());
  } else if (ui_->mean_button->isChecked()) {
    return convert_from_combined(stats_.Mean());
  }

  if (groups_active()) {
    auto group_ratio = get_group_ratio();
    temp_shape_ = stats_.Group1Mean() + (stats_.GroupDifference() * group_ratio);
    return convert_from_combined(temp_shape_);
  }

  return convert_from_combined(stats_.Mean());
}

//-----------------------------------------------------------------------------
Particles AnalysisTool::get_shape_points(int mode, double value) {
  if (!compute_stats() || stats_.Eigenvectors().size() <= 1) {
    return Particles();
  }
  if (mode + 2 > stats_.Eigenvalues().size()) {
    mode = stats_.Eigenvalues().size() - 2;
  }

  unsigned int m = stats_.Eigenvectors().cols() - (mode + 1);

  Eigen::VectorXd e = stats_.Eigenvectors().col(m);

  double lambda = sqrt(stats_.Eigenvalues()[m]);

  pca_labels_changed(QString::number(value, 'g', 2), QString::number(stats_.Eigenvalues()[m]),
                     QString::number(value * lambda));

  std::vector<double> vals;
  for (int i = stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(stats_.Eigenvalues()[i]);
  }
  double sum = std::accumulate(vals.begin(), vals.end(), 0.0);
  double cumulation = 0;
  for (size_t i = 0; i < mode + 1; ++i) {
    cumulation += vals[i];
  }
  if (sum > 0) {
    ui_->explained_variance->setText(QString::number(vals[mode] / sum * 100, 'f', 1) + "%");
    ui_->cumulative_explained_variance->setText(QString::number(cumulation / sum * 100, 'f', 1) + "%");
  } else {
    ui_->explained_variance->setText("");
    ui_->cumulative_explained_variance->setText("");
  }

  temp_shape_ = stats_.Mean() + (e * (value * lambda));

  return convert_from_combined(temp_shape_);
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mode_shape(int mode, double value) {
  return create_shape_from_points(get_shape_points(mode, value));
}

//---------------------------------------------------------------------------
ParticleShapeStatistics AnalysisTool::get_stats() {
  compute_stats();
  return stats_;
}

//---------------------------------------------------------------------------
void AnalysisTool::load_settings() {
  Parameters params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  ui_->numClusters->setValue(params.get("reconstruction_clusters", 3));
  ui_->meshDecimation->setValue(params.get("reconstruction_decimation", 0.30));
  ui_->maxAngle->setValue(params.get("reconstruction_max_angle", 60));

  update_group_boxes();

  ui_->group_box->setCurrentText(QString::fromStdString(params.get("current_group", "-None-")));
}

//---------------------------------------------------------------------------
void AnalysisTool::store_settings() {
  Parameters params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  params.set("reconstruction_clusters", ui_->numClusters->value());
  params.set("reconstruction_decimation", ui_->meshDecimation->value());
  params.set("reconstruction_max_angle", ui_->maxAngle->value());
  session_->get_project()->set_parameters(Parameters::ANALYSIS_PARAMS, params);
}

//---------------------------------------------------------------------------
void AnalysisTool::shutdown() { pca_animate_timer_.stop(); }

//---------------------------------------------------------------------------
void AnalysisTool::compute_shape_evaluations() {
  if (evals_ready_) {
    return;
  }

  // reset
  eval_compactness_ = Eigen::VectorXd();
  eval_specificity_ = Eigen::VectorXd();
  eval_generalization_ = Eigen::VectorXd();

  ui_->compactness_graph->setMinimumSize(QSize(250, 250));
  ui_->generalization_graph->setMinimumSize(QSize(250, 250));
  ui_->specificity_graph->setMinimumSize(QSize(250, 250));

  ui_->compactness_progress_widget->show();
  ui_->generalization_progress_widget->show();
  ui_->specificity_progress_widget->show();
  ui_->compactness_graph->hide();
  ui_->generalization_graph->hide();
  ui_->specificity_graph->hide();
  ui_->compactness_progress->setValue(0);
  ui_->generalization_progress->setValue(0);
  ui_->specificity_progress->setValue(0);

  auto job_types = {ShapeEvaluationJob::JobType::CompactnessType, ShapeEvaluationJob::JobType::GeneralizationType,
                    ShapeEvaluationJob::JobType::SpecificityType};
  for (auto job_type : job_types) {
    auto worker = Worker::create_worker();
    auto job = QSharedPointer<ShapeEvaluationJob>::create(job_type, stats_);
    connect(job.data(), &ShapeEvaluationJob::result_ready, this, &AnalysisTool::handle_eval_thread_complete);
    connect(job.data(), &ShapeEvaluationJob::report_progress, this, &AnalysisTool::handle_eval_thread_progress);
    worker->run_job(job);
  }

  evals_ready_ = true;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_tabWidget_currentChanged() { update_analysis_mode(); }

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaSlider_valueChanged() {
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  Q_EMIT pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_group_slider_valueChanged() {
  // this will make the slider handle redraw making the UI appear more responsive
  // QCoreApplication::processEvents();

  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->difference_button->setChecked(false);
  ui_->mean_button->setChecked(false);

  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaModeSpinBox_valueChanged(int i) { Q_EMIT pca_update(); }

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_animate_state_changed() {
  if (pca_animate()) {
    pca_animate_timer_.setInterval(10);
    pca_animate_timer_.start();
  } else {
    pca_animate_timer_.stop();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_timer() {
  if (!pca_animate()) {
    return;
  }

  int value = ui_->pcaSlider->value();
  if (pca_animate_direction_) {
    value += ui_->pcaSlider->singleStep();
  } else {
    value -= ui_->pcaSlider->singleStep();
  }

  if (value >= ui_->pcaSlider->maximum() || value <= ui_->pcaSlider->minimum()) {
    pca_animate_direction_ = !pca_animate_direction_;
  }

  ui_->pcaSlider->setValue(value);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_animate_state_changed() {
  if (ui_->group_animate_checkbox->isChecked()) {
    group_animate_timer_.setInterval(10);
    group_animate_timer_.start();
  } else {
    group_animate_timer_.stop();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_timer() {
  int value = ui_->group_slider->value();
  if (group_animate_direction_) {
    value += ui_->group_slider->singleStep();
  } else {
    value -= ui_->group_slider->singleStep();
  }

  if (value >= ui_->group_slider->maximum() || value <= ui_->group_slider->minimum()) {
    group_animate_direction_ = !group_animate_direction_;
    // setPregenSteps();
  }

  ui_->group_slider->setValue(value);
}

//---------------------------------------------------------------------------
double AnalysisTool::get_pca_value() {
  int slider_value = ui_->pcaSlider->value();
  float range = preferences_.get_pca_range();
  int halfRange = ui_->pcaSlider->maximum();

  double value = (double)slider_value / (double)halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void AnalysisTool::pca_labels_changed(QString value, QString eigen, QString lambda) {
  set_labels(QString("pca"), value);
  set_labels(QString("eigen"), eigen);
  set_labels(QString("lambda"), lambda);
}

//---------------------------------------------------------------------------
void AnalysisTool::update_slider() {
  auto steps = preferences_.get_pca_steps();
  auto sliderRange = ui_->pcaSlider->maximum() - ui_->pcaSlider->minimum();
  ui_->pcaSlider->setSingleStep(sliderRange / steps);
}

//---------------------------------------------------------------------------
void AnalysisTool::reset_stats() {
  ui_->tabWidget->setCurrentWidget(ui_->mean_tab);
  ui_->allSamplesRadio->setChecked(true);
  ui_->singleSamplesRadio->setChecked(false);
  ui_->sampleSpinBox->setEnabled(false);
  ui_->medianButton->setEnabled(false);
  ui_->pcaSlider->setEnabled(false);
  ui_->pcaAnimateCheckBox->setEnabled(false);
  ui_->pcaModeSpinBox->setEnabled(false);
  ui_->pcaAnimateCheckBox->setChecked(false);
  stats_ready_ = false;
  evals_ready_ = false;
  stats_ = ParticleShapeStatistics();
}

//---------------------------------------------------------------------------
void AnalysisTool::enable_actions(bool newly_enabled) {
  if (session_->get_num_shapes() < 1) {
    return;
  }

  if (newly_enabled) {
    ui_->mesh_warping_radio_button->setChecked(true);
    update_domain_alignment_box();
  }

  auto domain_types = session_->get_groomed_domain_types();
  bool image_domain = domain_types.size() > 0 && domain_types[0] == DomainType::Image;
  ui_->distance_transfom_radio_button->setEnabled(session_->particles_present() && session_->get_groomed_present() &&
                                                  image_domain);

  ui_->mesh_warping_radio_button->setEnabled(session_->particles_present() && session_->get_groomed_present());

  if (!ui_->mesh_warping_radio_button->isEnabled()) {
    ui_->legacy_radio_button->setChecked(true);
  }
  reconstruction_method_changed();

  update_group_boxes();
  ui_->sampleSpinBox->setMaximum(session_->get_num_shapes() - 1);
}

//---------------------------------------------------------------------------
void AnalysisTool::set_analysis_mode(std::string mode) {
  if (mode == "all samples" || mode == "single sample") {
    ui_->allSamplesRadio->setChecked(mode == "all samples");
    ui_->singleSamplesRadio->setChecked(mode == "single sample");
    ui_->tabWidget->setCurrentWidget(ui_->samples_tab);
  } else if (mode == "mean") {
    ui_->tabWidget->setCurrentWidget(ui_->mean_tab);
  } else if (mode == "pca") {
    ui_->tabWidget->setCurrentWidget(ui_->pca_tab);
  } else if (mode == "regression") {
    ui_->tabWidget->setCurrentWidget(ui_->regression_tab);
  }
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mean_shape() {
  auto shape_points = get_mean_shape_points();
  ShapeHandle shape = create_shape_from_points(shape_points);
  if (ui_->group_p_values_button->isChecked() && group_pvalue_job_ &&
      group_pvalue_job_->get_group_pvalues().rows() > 0) {
    shape->set_point_features("p_values", group_pvalue_job_->get_group_pvalues());
    shape->set_override_feature("p_values");
  }

  int num_points = shape_points.get_combined_global_particles().size() / 3;
  std::vector<Eigen::VectorXf> values;

  if (feature_map_ != "") {
    auto shapes = session_->get_shapes();
    Eigen::VectorXf sum(num_points);
    sum.setZero();

    bool ready = true;

    if (!groups_active()) {
      for (int i = 0; i < shapes.size(); i++) {
        shapes[i]->load_feature(DisplayMode::Reconstructed, feature_map_);
        auto value = shapes[i]->get_point_features(feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        } else {
          values.push_back(value);
          sum = sum + value;
        }
      }
      Eigen::VectorXf mean = sum / values.size();

      if (ready) {
        shape->set_point_features(feature_map_, mean);
      }
    } else {
      Eigen::VectorXf sum_left(num_points);
      sum_left.setZero();
      Eigen::VectorXf sum_right(num_points);
      sum_right.setZero();

      Q_FOREACH (auto shape, group1_list_) {
        shape->load_feature(DisplayMode::Reconstructed, feature_map_);
        auto value = shape->get_point_features(feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        } else {
          sum_left = sum_left + value;
        }
      }
      Eigen::VectorXf left_mean = sum_left / static_cast<double>(group1_list_.size());

      Q_FOREACH (auto shape, group2_list_) {
        shape->load_feature(DisplayMode::Reconstructed, feature_map_);
        auto value = shape->get_point_features(feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        } else {
          sum_right = sum_right + value;
        }
      }
      Eigen::VectorXf right_mean = sum_right / static_cast<double>(group2_list_.size());

      if (ready) {
        double group_ratio = get_group_ratio();
        auto blend = left_mean * (1 - group_ratio) + right_mean * group_ratio;
        shape->set_point_features(feature_map_, blend);
      }
    }
  }

  return shape;
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::create_shape_from_points(Particles points) {
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(session_->get_mesh_manager());
  shape->set_particles(points);
  shape->get_reconstructed_meshes();
  shape->set_reconstruction_transforms(reconstruction_transforms_);
  return shape;
}

//---------------------------------------------------------------------------
void AnalysisTool::set_feature_map(const std::string& feature_map) { feature_map_ = feature_map; }

//---------------------------------------------------------------------------
std::string AnalysisTool::get_display_feature_map() {
  if (session_->get_show_difference_vectors()) {
    return "surface_difference";
  }
  if (ui_->group_p_values_button->isChecked() && group_pvalue_job_ &&
      group_pvalue_job_->get_group_pvalues().rows() > 0) {
    return "p_values";
  }
  return feature_map_;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_boxes() {
  // populate the group sets
  auto group_names = session_->get_project()->get_group_names();

  ui_->group_widget->setEnabled(!group_names.empty());

  if (group_names != current_group_names_) {  // only update if different
    ui_->group_box->clear();
    ui_->group_box->addItem("-None-");
    for (const std::string& group : group_names) {
      ui_->group_box->addItem(QString::fromStdString(group));
    }
    current_group_names_ = group_names;
    group_changed();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_values() {
  block_group_change_ = true;
  auto values = session_->get_project()->get_group_values(ui_->group_box->currentText().toStdString());

  if (values != current_group_values_) {
    // populate group values
    ui_->group_left->clear();
    ui_->group_right->clear();
    for (const std::string& value : values) {
      QString item = QString::fromStdString(value);
      ui_->group_left->addItem(item);
      ui_->group_right->addItem(item);
    }
    current_group_values_ = values;

    // try to set the right one to a different value than left
    size_t i = 0;
    while (ui_->group_left->currentIndex() == ui_->group_right->currentIndex() && i < current_group_values_.size()) {
      ui_->group_right->setCurrentIndex(i++);
    }
  }

  bool groups_on = ui_->group_box->currentText() != "-None-";

  ui_->group1_button->setEnabled(groups_on);
  ui_->group2_button->setEnabled(groups_on);
  ui_->difference_button->setEnabled(groups_on);
  ui_->group_p_values_button->setEnabled(groups_on);
  ui_->group_slider->setEnabled(groups_on);
  ui_->group_left->setEnabled(groups_on);
  ui_->group_right->setEnabled(groups_on);
  ui_->group_animate_checkbox->setEnabled(groups_on);

  block_group_change_ = false;
  group_changed();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_domain_alignment_box() {
  auto domain_names = session_->get_project()->get_domain_names();

  bool multiple_domains = domain_names.size() > 1;
  ui_->reference_domain_widget->setVisible(multiple_domains);
  ui_->reference_domain->clear();
  if (multiple_domains) {
    ui_->reference_domain->addItem("Global Alignment");
    ui_->reference_domain->addItem("Local Alignment");
    for (const auto& name : domain_names) {
      ui_->reference_domain->addItem(QString::fromStdString(name));
    }
    ui_->reference_domain->setCurrentIndex(0);
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::update_lda_graph() {
  if (groups_active()) {
    if (!group_lda_job_running_) {
      group_lda_job_running_ = true;
      ui_->lda_panel->show();
      ui_->lda_label->show();
      ui_->lda_progress->setValue(0);
      ui_->lda_progress->setMaximum(0);
      ui_->lda_progress->update();
      group_lda_job_->set_stats(stats_);
      app_->get_py_worker()->run_job(group_lda_job_);
    }
  } else {
    ui_->lda_graph->setVisible(false);
    ui_->lda_hint_label->setVisible(false);
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::update_difference_particles() {
  if (session_->get_num_shapes() < 1) {
    return;
  }

  // start with a copy from the first shape so that the sizes of domains are already filled out
  Particles target = session_->get_shapes()[0]->get_particles();
  auto all_particles = target.get_combined_global_particles();

  Eigen::VectorXd mean = stats_.Mean();

  if (get_group_difference_mode()) {
    mean = stats_.Group2Mean();
  }

  for (unsigned int i = 0; i < stats_.Mean().size(); i++) {
    all_particles[i] = mean[i];
  }

  target.set_combined_global_particles(all_particles);

  session_->set_show_difference_vectors(get_group_difference_mode() || ui_->show_difference_to_mean->isChecked());

  session_->set_difference_particles(target);
}

//---------------------------------------------------------------------------
void AnalysisTool::group_changed() {
  if (block_group_change_) {
    return;
  }
  stats_ready_ = false;
  group_pvalue_job_ = nullptr;
  compute_stats();
  update_lda_graph();
}

//---------------------------------------------------------------------------
bool AnalysisTool::groups_active() {
  std::string group_set = ui_->group_box->currentText().toStdString();
  bool groups_enabled = group_set != "" && group_set != "-None-";
  return groups_enabled;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_view_open_button_toggled() {
  bool show = ui_->view_open_button->isChecked();
  ui_->view_content->setVisible(show);
}

//---------------------------------------------------------------------------
void AnalysisTool::on_surface_open_button_toggled() {
  bool show = ui_->surface_open_button->isChecked();
  ui_->surface_content->setVisible(show);
}

//---------------------------------------------------------------------------
void AnalysisTool::on_metrics_open_button_toggled() {
  bool show = ui_->metrics_open_button->isChecked();
  ui_->metrics_content->setVisible(show);

  if (show) {
    compute_stats();
  }
}

//---------------------------------------------------------------------------
bool AnalysisTool::is_group_active(int shape_index) {
  std::string group_set = ui_->group_box->currentText().toStdString();
  std::string left_group = ui_->group_left->currentText().toStdString();
  std::string right_group = ui_->group_right->currentText().toStdString();

  bool groups_enabled = groups_active();

  auto shapes = session_->get_shapes();
  auto shape = shapes[shape_index];

  bool left = false;
  bool right = false;
  bool both = true;
  if (ui_->group1_button->isChecked()) {
    both = false;
    left = true;
  } else if (ui_->group2_button->isChecked()) {
    both = false;
    right = true;
  }

  if (groups_enabled) {
    auto value = shape->get_subject()->get_group_value(group_set);
    if (left && value == left_group) {
      return true;
    } else if (right && value == right_group) {
      return true;
    } else if (both) {
      return true;
    } else {
      return false;
    }
  }

  return true;
}

//---------------------------------------------------------------------------
void AnalysisTool::initialize_mesh_warper() {
  if (session_->particles_present() && session_->get_groomed_present()) {
    compute_stats();
    int median = stats_.ComputeMedianShape(-32);  //-32 = both groups

    if (median < 0 || median >= session_->get_num_shapes()) {
      SW_ERROR("Unable to set reference mesh, stats returned invalid median index");
      return;
    }
    std::shared_ptr<Shape> median_shape = session_->get_shapes()[median];

    auto mesh_group = median_shape->get_groomed_meshes(true);

    if (!mesh_group.valid()) {
      SW_ERROR("Unable to set reference mesh, groomed mesh is unavailable");
      return;
    }
    auto meshes = mesh_group.meshes();
    for (int i = 0; i < mesh_group.meshes().size(); i++) {
      Eigen::VectorXd particles = median_shape->get_particles().get_local_particles(i);
      Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>((double*)particles.data(), particles.size());
      points.resize(3, points.size() / 3);
      points.transposeInPlace();

      auto poly_data = meshes[i]->get_poly_data();
      Mesh mesh(poly_data);
      median_shape->get_constraints(i).clipMesh(mesh);

      session_->get_mesh_manager()->get_mesh_warper(i)->set_reference_mesh(mesh.getVTKMesh(), points);
    }
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_thread_complete(ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data) {
  switch (job_type) {
    case ShapeEvaluationJob::JobType::CompactnessType:
      eval_compactness_ = data;
      create_plot(ui_->compactness_graph, data, "Compactness", "Number of Modes", "Explained Variance");
      ui_->compactness_graph->show();
      ui_->compactness_progress_widget->hide();
      break;
    case ShapeEvaluationJob::JobType::SpecificityType:
      create_plot(ui_->specificity_graph, data, "Specificity", "Number of Modes", "Specificity");
      eval_specificity_ = data;
      ui_->specificity_graph->show();
      ui_->specificity_progress_widget->hide();
      break;
    case ShapeEvaluationJob::JobType::GeneralizationType:
      create_plot(ui_->generalization_graph, data, "Generalization", "Number of Modes", "Generalization");
      eval_generalization_ = data;
      ui_->generalization_graph->show();
      ui_->generalization_progress_widget->hide();
      break;
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_thread_progress(ShapeEvaluationJob::JobType job_type, float progress) {
  switch (job_type) {
    case ShapeEvaluationJob::JobType::CompactnessType:
      ui_->compactness_progress->setValue(progress * 100);
      break;
    case ShapeEvaluationJob::JobType::SpecificityType:
      ui_->specificity_progress->setValue(progress * 100);
      break;
    case ShapeEvaluationJob::JobType::GeneralizationType:
      ui_->generalization_progress->setValue(progress * 100);
      break;
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_particle_normals_progress(float progress) {
  ui_->particles_progress->setValue(progress * 100);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_particle_normals_complete(std::vector<bool> good_bad) {
  ui_->particles_progress->hide();
  ui_->run_good_bad->setEnabled(true);
  session_->set_good_bad_particles(good_bad);
  session_->set_show_good_bad_particles(true);
  update_interface();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_pvalues_complete() {
  Q_EMIT progress(100);
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_alignment_changed(int new_alignment) {
  new_alignment -= 2;
  if (new_alignment == current_alignment_) {
    return;
  }
  current_alignment_ = static_cast<AlignmentType>(new_alignment);

  Q_FOREACH (ShapeHandle shape, session_->get_shapes()) {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (current_alignment_ == AlignmentType::Local) {
      transform = nullptr;
    } else if (current_alignment_ == AlignmentType::Global) {
      auto domain_names = session_->get_project()->get_domain_names();
      transform = shape->get_groomed_transform(domain_names.size());
    } else {
      transform = shape->get_groomed_transform(new_alignment);
    }

    shape->set_particle_transform(transform);
  }

  evals_ready_ = false;
  group_changed();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::run_good_bad_particles() {
  auto worker = Worker::create_worker();
  ui_->particles_progress->show();
  ui_->run_good_bad->setEnabled(false);
  auto job = QSharedPointer<ParticleNormalEvaluationJob>::create(session_, ui_->good_bad_max_angle->value());
  connect(job.data(), &ParticleNormalEvaluationJob::result_ready, this,
          &AnalysisTool::handle_eval_particle_normals_complete);
  connect(job.data(), &ParticleNormalEvaluationJob::progress, this,
          &AnalysisTool::handle_eval_particle_normals_progress);
  worker->run_job(job);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_lda_progress(double progress) {
  if (progress > 0) {
    ui_->lda_progress->setMaximum(100);
  } else {
    ui_->lda_progress->setMaximum(0);
  }
  ui_->lda_progress_widget->setVisible(progress < 1);
  ui_->lda_progress->setValue(progress * 100);
  ui_->lda_progress->update();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_lda_complete() {
  ui_->lda_progress_widget->setVisible(false);
  ui_->lda_label->setVisible(false);
  group_lda_job_running_ = false;

  QString left_group = ui_->group_left->currentText();
  QString right_group = ui_->group_right->currentText();

  group_lda_job_->plot(ui_->lda_graph, left_group, right_group);
  ui_->lda_graph->setVisible(true);
  ui_->lda_hint_label->setVisible(true);
}

//---------------------------------------------------------------------------
void AnalysisTool::show_difference_to_mean_clicked() {
  update_difference_particles();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::reconstruction_method_changed() {
  ui_->reconstruction_options->setVisible(ui_->distance_transfom_radio_button->isChecked());
  std::string method = MeshGenerator::RECONSTRUCTION_LEGACY_C;
  if (ui_->distance_transfom_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_DISTANCE_TRANSFORM_C;
  } else if (ui_->mesh_warping_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_MESH_WARPER_C;
  }

  auto previous_method = session_->get_mesh_manager()->get_mesh_generator()->get_reconstruction_method();
  if (previous_method != method) {
    session_->get_mesh_manager()->get_mesh_generator()->set_reconstruction_method(method);
    session_->handle_clear_cache();
    Q_EMIT reconstruction_complete();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::set_active(bool active) {
  if (!active) {
    ui_->pcaAnimateCheckBox->setChecked(false);
    pca_animate_timer_.stop();
  }
  active_ = active;
  update_interface();
}

//---------------------------------------------------------------------------
bool AnalysisTool::get_active() { return active_; }

//---------------------------------------------------------------------------
Particles AnalysisTool::convert_from_combined(const Eigen::VectorXd& points) {
  Particles particles;
  if (session_->get_shapes().empty()) {
    return particles;
  }
  auto base = session_->get_shapes()[0]->get_particles();

  auto worlds = base.get_world_particles();
  int idx = 0;
  for (int d = 0; d < worlds.size(); d++) {
    Eigen::VectorXd new_world(worlds[d].size());
    for (int i = 0; i < worlds[d].size(); i++) {
      new_world[i] = points[idx++];
    }
    particles.set_world_particles(d, new_world);
    particles.set_local_particles(d, new_world);
  }

  return particles;
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_reconstructed_domain_transforms() {
  auto shapes = session_->get_shapes();
  if (current_alignment_ == AlignmentType::Local) {
    reconstruction_transforms_.resize(session_->get_domains_per_shape());
    for (int domain = 0; domain < session_->get_domains_per_shape(); domain++) {
      int num_shapes = shapes.size();
      auto transform = vtkSmartPointer<vtkTransform>::New();
      double* values = transform->GetMatrix()->GetData();

      for (int i = 0; i < shapes.size(); i++) {
        vtkSmartPointer<vtkTransform> base = shapes[i]->get_alignment(0);
        vtkSmartPointer<vtkTransform> offset = shapes[i]->get_alignment(domain);

        for (int j = 0; j < 16; j++) {
          values[j] += (base->GetMatrix()->GetData()[j] - offset->GetMatrix()->GetData()[j]) / num_shapes;
        }
      }
      reconstruction_transforms_[domain] = transform;
    }
  } else {
    reconstruction_transforms_.clear();
  }
  for (int s = 0; s < shapes.size(); s++) {
    shapes[s]->set_reconstruction_transforms(reconstruction_transforms_);
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label,
                               QString y_label) {
  JKQTPDatastore* ds = plot->getDatastore();
  ds->clear();

  QVector<double> x, y;
  for (int i = 0; i < data.size(); i++) {
    x << i + 1;
    y << data[i];
  }
  size_t column_x = ds->addCopiedColumn(x, x_label);
  size_t column_y = ds->addCopiedColumn(y, y_label);

  plot->clearGraphs();
  JKQTPXYLineGraph* graph = new JKQTPXYLineGraph(plot);
  graph->setColor(Qt::blue);
  graph->setSymbolType(JKQTPNoSymbol);
  graph->setXColumn(column_x);
  graph->setYColumn(column_y);
  graph->setTitle(title);

  plot->getPlotter()->setUseAntiAliasingForGraphs(true);
  plot->getPlotter()->setUseAntiAliasingForSystem(true);
  plot->getPlotter()->setUseAntiAliasingForText(true);
  plot->getPlotter()->setPlotLabelFontSize(18);
  plot->getPlotter()->setPlotLabel("\\textbf{" + title + "}");
  plot->getPlotter()->setDefaultTextSize(14);
  plot->getPlotter()->setShowKey(false);

  plot->getXAxis()->setAxisLabel(x_label);
  plot->getXAxis()->setLabelFontSize(14);
  plot->getYAxis()->setAxisLabel(y_label);
  plot->getYAxis()->setLabelFontSize(14);

  plot->clearAllMouseWheelActions();
  plot->setMousePositionShown(false);
  plot->setMinimumSize(250, 250);
  plot->addGraph(graph);
  plot->zoomToFit();
}
}  // namespace shapeworks
