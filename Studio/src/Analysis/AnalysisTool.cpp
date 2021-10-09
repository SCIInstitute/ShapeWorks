// std
#include <iostream>
#include <fstream>

// qt
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>

// shapeworks
#include <Analysis/AnalysisTool.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Visualization/Lightbox.h>
#include <Python/PythonWorker.h>
#include <Job/GroupPvalueJob.h>

#include <jkqtplotter/jkqtplotter.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>

#include <ui_AnalysisTool.h>

namespace shapeworks {

const std::string AnalysisTool::MODE_ALL_SAMPLES_C("all samples");
const std::string AnalysisTool::MODE_MEAN_C("mean");
const std::string AnalysisTool::MODE_PCA_C("pca");
const std::string AnalysisTool::MODE_SINGLE_SAMPLE_C("single sample");
const std::string AnalysisTool::MODE_REGRESSION_C("regression");

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_AnalysisTool;
  this->ui_->setupUi(this);
  this->stats_ready_ = false;

  connect(this->ui_->view_header, &QPushButton::clicked,
          this->ui_->view_open_button, &QPushButton::toggle);
  connect(this->ui_->metrics_header, &QPushButton::clicked,
          this->ui_->metrics_open_button, &QPushButton::toggle);
  connect(this->ui_->surface_header, &QPushButton::clicked,
          this->ui_->surface_open_button, &QPushButton::toggle);
  this->ui_->view_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  this->ui_->surface_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  this->ui_->metrics_label->setAttribute(Qt::WA_TransparentForMouseEvents);

  // default to linear scale
  this->ui_->log_radio->setChecked(false);
  this->ui_->linear_radio->setChecked(true);

  connect(this->ui_->allSamplesRadio, SIGNAL(clicked()), this, SLOT(handle_analysis_options()));
  connect(this->ui_->singleSamplesRadio, SIGNAL(clicked()), this, SLOT(handle_analysis_options()));
  connect(this->ui_->sampleSpinBox, SIGNAL(valueChanged(int)), this,
          SLOT(handle_analysis_options()));
  connect(this->ui_->medianButton, SIGNAL(clicked()), this, SLOT(handle_median()));

  connect(this->ui_->pcaAnimateCheckBox, SIGNAL(stateChanged(int)), this,
          SLOT(handle_pca_animate_state_changed()));
  connect(&this->pca_animate_timer_, SIGNAL(timeout()), this, SLOT(handle_pca_timer()));

  // group animation
  connect(this->ui_->group_animate_checkbox, &QCheckBox::stateChanged, this,
          &AnalysisTool::handle_group_animate_state_changed);
  connect(&this->group_animate_timer_, &QTimer::timeout, this, &AnalysisTool::handle_group_timer);

  connect(this->ui_->group_box, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::update_group_values);

  connect(this->ui_->group_left, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::group_changed);

  connect(this->ui_->group_right, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::group_changed);

  connect(this->ui_->group_p_values_button, &QPushButton::clicked, this,
          &AnalysisTool::group_p_values_clicked);

  connect(this->ui_->reference_domain, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::handle_alignment_changed);

  this->ui_->surface_open_button->setChecked(false);
  this->ui_->metrics_open_button->setChecked(false);

  /// TODO nothing there yet (regression tab)
  this->ui_->tabWidget->removeTab(3);

  this->ui_->graph_->set_y_label("Explained Variance");

  for (auto button : {this->ui_->distance_transfom_radio_button,
                      this->ui_->mesh_warping_radio_button, this->ui_->legacy_radio_button}) {
    connect(button, &QRadioButton::clicked,
            this, &AnalysisTool::reconstruction_method_changed);
  }

  this->ui_->explained_variance_panel->hide();
  this->ui_->reconstruction_options->hide();
}

//---------------------------------------------------------------------------
std::string AnalysisTool::get_analysis_mode()
{
  if (this->ui_->tabWidget->currentWidget() == this->ui_->samples_tab) {
    if (this->ui_->allSamplesRadio->isChecked()) { return AnalysisTool::MODE_ALL_SAMPLES_C; }
    if (this->ui_->singleSamplesRadio->isChecked()) { return AnalysisTool::MODE_SINGLE_SAMPLE_C; }
  }

  if (this->ui_->tabWidget->currentWidget() == this->ui_->mean_tab) {
    return AnalysisTool::MODE_MEAN_C;
  }
  if (this->ui_->tabWidget->currentWidget() == this->ui_->pca_tab) {
    return AnalysisTool::MODE_PCA_C;
  }
  if (this->ui_->tabWidget->currentWidget() == this->ui_->regression_tab) {
    return AnalysisTool::MODE_REGRESSION_C;
  }
  return "";
}

//---------------------------------------------------------------------------
bool AnalysisTool::get_group_difference_mode()
{
  return this->ui_->difference_button->isChecked();
}

//---------------------------------------------------------------------------
std::vector<Shape::Point> AnalysisTool::get_group_difference_vectors()
{
  std::vector<Shape::Point> vecs;

  auto num_points = this->stats_.Mean().size() / 3;
  for (unsigned int i = 0; i < num_points; i++) {
    Shape::Point tmp;
    tmp.x = this->stats_.Group2Mean()[i * 3] - this->stats_.Group1Mean()[i * 3];
    tmp.y = this->stats_.Group2Mean()[i * 3 + 1] - this->stats_.Group1Mean()[i * 3 + 1];
    tmp.z = this->stats_.Group2Mean()[i * 3 + 2] - this->stats_.Group1Mean()[i * 3 + 2];
    vecs.push_back(tmp);
  }
  return vecs;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_linear_radio_toggled(bool b)
{
  if (b) {
    this->ui_->graph_->set_log_scale(false);
    this->ui_->graph_->repaint();
  }
  else {
    this->ui_->graph_->set_log_scale(true);
    this->ui_->graph_->repaint();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_reconstruction_complete()
{
  this->session_->handle_clear_cache();

  this->session_->calculate_reconstructed_samples();
  emit progress(100);
  emit message("Reconstruction Complete");
  emit reconstruction_complete();
  ///TODO: Studio
  ///this->ui_->run_optimize_button->setEnabled(true);

  this->enable_actions();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_reconstructionButton_clicked()
{
  this->store_settings();
  emit message("Please wait: running reconstruction step...");
  emit progress(5);
  QThread* thread = new QThread;
  std::vector<std::vector<itk::Point<double>>> local, global;
  std::vector<std::string> images;

  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::ReconstructType, nullptr, nullptr,
    nullptr, this->session_,
    local, global, images,
    this->ui_->maxAngle->value(),
    this->ui_->meshDecimation->value(),
    this->ui_->numClusters->value());
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_reconstruction_complete()));

  connect(worker, &ShapeworksWorker::error_message, this, &AnalysisTool::error);
  connect(worker, &ShapeworksWorker::warning_message, this, &AnalysisTool::warning);
  connect(worker, &ShapeworksWorker::message, this, &AnalysisTool::message);

  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
  emit progress(15);
}

//---------------------------------------------------------------------------
int AnalysisTool::getPCAMode()
{
  return this->ui_->pcaModeSpinBox->value() - 1;
}

//---------------------------------------------------------------------------
double AnalysisTool::get_group_value()
{
  double groupSliderValue = this->ui_->group_slider->value();
  double groupRatio = groupSliderValue / static_cast<double>(this->ui_->group_slider->maximum());
  return groupRatio;
}

//---------------------------------------------------------------------------
bool AnalysisTool::pcaAnimate()
{
  return this->ui_->pcaAnimateCheckBox->isChecked();
}

//---------------------------------------------------------------------------
void AnalysisTool::setLabels(QString which, QString value)
{
  if (which == QString("pca")) {
    this->ui_->pcaValueLabel->setText(value);
  }
  else if (which == QString("eigen")) {
    this->ui_->pcaEigenValueLabel->setText(value);
  }
  else if (which == QString("lambda")) {
    this->ui_->pcaLambdaLabel->setText(value);
  }
}

//---------------------------------------------------------------------------
int AnalysisTool::get_sample_number()
{
  return this->ui_->sampleSpinBox->value();
}

//---------------------------------------------------------------------------
AnalysisTool::~AnalysisTool()
{}

//---------------------------------------------------------------------------
void AnalysisTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
  // reset to original
  this->ui_->mesh_warping_radio_button->setChecked(true);
  this->ui_->difference_button->setChecked(false);
  this->ui_->group_p_values_button->setChecked(false);
  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
}

//---------------------------------------------------------------------------
void AnalysisTool::set_app(ShapeWorksStudioApp* app)
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_analysis_mode()
{
  this->handle_analysis_options();
}

//---------------------------------------------------------------------------
bool AnalysisTool::group_pvalues_valid()
{
  return this->group_pvalue_job_ && this->group_pvalue_job_->get_group_pvalues().rows() > 0;
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape()
{}

//---------------------------------------------------------------------------
void AnalysisTool::handle_analysis_options()
{

  if (this->ui_->tabWidget->currentWidget() == this->ui_->samples_tab) {
    this->ui_->pcaAnimateCheckBox->setChecked(false);
    this->ui_->pcaAnimateCheckBox->setEnabled(false);
    this->ui_->pcaModeSpinBox->setEnabled(false);
    this->pca_animate_timer_.stop();
    this->ui_->pcaSlider->setEnabled(false);
    if (this->ui_->singleSamplesRadio->isChecked()) {
      //one sample mode
      this->ui_->sampleSpinBox->setEnabled(true);
      this->ui_->medianButton->setEnabled(true);
    }
    else {
      //all samples mode
      this->ui_->sampleSpinBox->setEnabled(false);
      this->ui_->medianButton->setEnabled(false);
    }
  }
  else if (this->ui_->tabWidget->currentWidget() == this->ui_->mean_tab) {
    //mean mode
    this->ui_->sampleSpinBox->setEnabled(false);
    this->ui_->medianButton->setEnabled(false);
    this->ui_->pcaSlider->setEnabled(false);
    this->ui_->pcaAnimateCheckBox->setEnabled(false);
    this->ui_->pcaModeSpinBox->setEnabled(false);
    this->ui_->pcaAnimateCheckBox->setChecked(false);
    this->pca_animate_timer_.stop();
  }
  else if (this->ui_->tabWidget->currentWidget() == this->ui_->pca_tab) {
    //pca mode
    this->ui_->sampleSpinBox->setEnabled(false);
    this->ui_->medianButton->setEnabled(false);
    this->ui_->pcaSlider->setEnabled(true);
    this->ui_->pcaAnimateCheckBox->setEnabled(true);
    this->ui_->pcaModeSpinBox->setEnabled(true);
  }
  else {
    //regression mode
    this->ui_->sampleSpinBox->setEnabled(false);
    this->ui_->medianButton->setEnabled(false);
    this->ui_->pcaSlider->setEnabled(false);
    this->ui_->pcaAnimateCheckBox->setEnabled(false);
    this->ui_->pcaModeSpinBox->setEnabled(false);
    this->pca_animate_timer_.stop();
  }

  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_median()
{
  if (!this->compute_stats()) { return; }
  this->ui_->sampleSpinBox->setValue(
    this->stats_.ComputeMedianShape(-32));       //-32 = both groups
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_mean_button_clicked()
{
  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->difference_button->setChecked(false);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->mean_button->setChecked(true);
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group1_button_clicked()
{
  this->ui_->group_slider->setValue(this->ui_->group_slider->minimum());
  this->ui_->mean_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->difference_button->setChecked(false);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->group1_button->setChecked(true);
  this->ui_->group_p_values_button->setChecked(false);
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group2_button_clicked()
{
  this->ui_->group_slider->setValue(this->ui_->group_slider->maximum());
  this->ui_->mean_button->setChecked(false);
  this->ui_->group1_button->setChecked(false);
  this->ui_->difference_button->setChecked(false);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->group2_button->setChecked(true);
  this->ui_->group_p_values_button->setChecked(false);
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_difference_button_clicked()
{
  this->ui_->group_slider->setValue(this->ui_->group_slider->minimum());
  this->ui_->mean_button->setChecked(false);
  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->group_p_values_button->setChecked(false);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->difference_button->setChecked(true);
  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_p_values_clicked()
{
  this->ui_->group_slider->setValue(10);
  this->ui_->mean_button->setChecked(false);
  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->group_p_values_button->setChecked(true);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->difference_button->setChecked(false);

  if (this->group_pvalues_valid()) {
    this->handle_group_pvalues_complete();
  }
  else {

    if (this->group1_list_.size() < 3 || this->group2_list_.size() < 3) {
      emit error("Unable to compute p-values with less than 3 shapes per group");
      return;
    }
    this->group_pvalue_job_ = QSharedPointer<GroupPvalueJob>::create(this->stats_);
    connect(this->group_pvalue_job_.data(), &GroupPvalueJob::message, this, &AnalysisTool::message);
    connect(this->group_pvalue_job_.data(), &GroupPvalueJob::progress, this, &AnalysisTool::progress);
    connect(this->group_pvalue_job_.data(), &GroupPvalueJob::finished,
            this, &AnalysisTool::handle_group_pvalues_complete);
    this->app_->get_py_worker()->run_job(this->group_pvalue_job_);
  }
}

//-----------------------------------------------------------------------------
bool AnalysisTool::compute_stats()
{
  if (this->stats_ready_) {
    return true;
  }

  if (this->session_->get_shapes().size() == 0 || !this->session_->particles_present()) {
    return false;
  }

  this->compute_reconstructed_domain_transforms();

  this->ui_->pcaModeSpinBox->setMaximum(
    std::max<double>(1, this->session_->get_shapes().size() - 1));

  std::vector<vnl_vector<double>> points;
  std::vector<int> group_ids;

  std::string group_set = this->ui_->group_box->currentText().toStdString();
  std::string left_group = this->ui_->group_left->currentText().toStdString();
  std::string right_group = this->ui_->group_right->currentText().toStdString();

  bool groups_enabled = this->groups_active();

  this->group1_list_.clear();
  this->group2_list_.clear();

  for (ShapeHandle shape : this->session_->get_shapes()) {

    if (groups_enabled) {
      auto value = shape->get_subject()->get_group_value(group_set);
      if (value == left_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(1);
        this->group1_list_.push_back(shape);
      }
      else if (value == right_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(2);
        this->group2_list_.push_back(shape);
      }
      else {
        // we don't include it
      }
    }
    else {
      points.push_back(shape->get_global_correspondence_points());
      group_ids.push_back(shape->get_group_id());
    }
  }

  if (points.empty()) {
    return false;
  }

  // consistency check
  size_t point_size = points[0].size();
  for (auto&& p : points) {
    if (p.size() != point_size) {
      emit error(
        "Inconsistency in data, particle files must contain the same number of points");
      return false;
    }
  }

  this->stats_.ImportPoints(points, group_ids);
  this->stats_.ComputeModes();

  this->compute_shape_evaluations();

  this->stats_ready_ = true;
  std::vector<double> vals;
  for (int i = this->stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(this->stats_.Eigenvalues()[i]);
  }
  this->ui_->graph_->set_data(vals);
  this->ui_->graph_->repaint();

  ////  Uncomment this to write out long format sample data
  /*
     if (groups_enabled) {
     std::ofstream file;
     file.open ("/tmp/stats.csv");
     file << "subject,group,particle,x,y,z\n";

     int shape_id = 0;
     for (ShapeHandle shape : this->session_->get_shapes()) {
      auto group = shape->get_subject()->get_group_value(group_set);
      auto particles = shape->get_particles();
      auto points = particles.get_world_points(0);
      int point_id = 0;
      for (auto point : points) {
        file << shape_id << "," << group << "," << point_id++ << "," << point[0] << "," << point[1] << "," << point[2] << "\n";
      }
      shape_id++;
     }
     file.close();
     }
   */

  return true;
}

//-----------------------------------------------------------------------------
StudioParticles AnalysisTool::get_mean_shape_points()
{
  if (!this->compute_stats()) {
    return StudioParticles();
  }

  if (this->ui_->group1_button->isChecked() || this->ui_->difference_button->isChecked()) {
    return this->convert_from_combined(this->stats_.Group1Mean());
  }
  else if (this->ui_->group2_button->isChecked()) {
    return this->convert_from_combined(this->stats_.Group2Mean());
  }
  else if (this->ui_->mean_button->isChecked()) {
    return this->convert_from_combined(this->stats_.Mean());
  }

  if (this->groups_active()) {
    auto group_value = this->get_group_value();
    this->temp_shape_ = this->stats_.Group1Mean() + (this->stats_.GroupDifference() * group_value);
    return this->convert_from_combined(this->temp_shape_);
  }

  return this->convert_from_combined(this->stats_.Mean());
}

//-----------------------------------------------------------------------------
StudioParticles AnalysisTool::get_shape_points(int mode, double value)
{
  if (!this->compute_stats() || this->stats_.Eigenvectors().size() <= 1) {
    return StudioParticles();
  }
  if (mode + 2 > this->stats_.Eigenvalues().size()) {
    mode = this->stats_.Eigenvalues().size() - 2;
  }

  unsigned int m = this->stats_.Eigenvectors().columns() - (mode + 1);

  vnl_vector<double> e = this->stats_.Eigenvectors().get_column(m);

  double lambda = sqrt(this->stats_.Eigenvalues()[m]);

  this->pca_labels_changed(QString::number(value, 'g', 2),
                           QString::number(this->stats_.Eigenvalues()[m]),
                           QString::number(value * lambda));

  std::vector<double> vals;
  for (int i = this->stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(this->stats_.Eigenvalues()[i]);
  }
  double sum = std::accumulate(vals.begin(), vals.end(), 0.0);
  double cumulation = 0;
  for (size_t i = 0; i < mode + 1; ++i) {
    cumulation += vals[i];
  }
  if (sum > 0) {
    this->ui_->explained_variance->setText(QString::number(vals[mode] / sum * 100, 'f', 1) + "%");
    this->ui_->cumulative_explained_variance->setText(
      QString::number(cumulation / sum * 100, 'f', 1) + "%");
  }
  else {
    this->ui_->explained_variance->setText("");
    this->ui_->cumulative_explained_variance->setText("");
  }

  this->temp_shape_ = this->stats_.Mean() + (e * (value * lambda));

  return this->convert_from_combined(this->temp_shape_);
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mode_shape(int mode, double value)
{
  return this->create_shape_from_points(this->get_shape_points(mode, value));
}

//---------------------------------------------------------------------------
ParticleShapeStatistics AnalysisTool::get_stats()
{
  this->compute_stats();
  return this->stats_;
}

//---------------------------------------------------------------------------
void AnalysisTool::load_settings()
{
  Parameters params = this->session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  this->ui_->numClusters->setValue(params.get("reconstruction_clusters", 3));
  this->ui_->meshDecimation->setValue(params.get("reconstruction_decimation", 0.30));
  this->ui_->maxAngle->setValue(params.get("reconstruction_max_angle", 60));

  this->update_group_boxes();

  this->ui_->group_box->setCurrentText(
    QString::fromStdString(params.get("current_group", "-None-")));
}

//---------------------------------------------------------------------------
void AnalysisTool::store_settings()
{
  Parameters params = this->session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  params.set("reconstruction_clusters", this->ui_->numClusters->value());
  params.set("reconstruction_decimation", this->ui_->meshDecimation->value());
  params.set("reconstruction_max_angle", this->ui_->maxAngle->value());
  this->session_->get_project()->set_parameters(Parameters::ANALYSIS_PARAMS, params);
}

//---------------------------------------------------------------------------
void AnalysisTool::shutdown()
{
  this->pca_animate_timer_.stop();
}

//---------------------------------------------------------------------------
bool AnalysisTool::export_variance_graph(QString filename)
{
  return this->ui_->graph_->grab().save(filename);
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_shape_evaluations()
{
  if (this->evals_ready_) {
    return;
  }

  // reset
  this->eval_compactness_ = Eigen::VectorXd();
  this->eval_specificity_ = Eigen::VectorXd();
  this->eval_generalization_ = Eigen::VectorXd();

  this->ui_->compactness_graph->setMinimumSize(this->ui_->graph_->minimumSize());
  this->ui_->generalization_graph->setMinimumSize(this->ui_->graph_->minimumSize());
  this->ui_->specificity_graph->setMinimumSize(this->ui_->graph_->minimumSize());

  this->ui_->compactness_progress_widget->show();
  this->ui_->generalization_progress_widget->show();
  this->ui_->specificity_progress_widget->show();
  this->ui_->compactness_graph->hide();
  this->ui_->generalization_graph->hide();
  this->ui_->specificity_graph->hide();
  this->ui_->compactness_progress->setValue(0);
  this->ui_->generalization_progress->setValue(0);
  this->ui_->specificity_progress->setValue(0);

  auto job_types = {ShapeEvaluationJob::JobType::CompactnessType,
                    ShapeEvaluationJob::JobType::GeneralizationType,
                    ShapeEvaluationJob::JobType::SpecificityType};
  for (auto job_type : job_types) {
    auto worker = Worker::create_worker();
    auto job = QSharedPointer<ShapeEvaluationJob>::create(job_type, this->stats_);
    connect(job.data(), &ShapeEvaluationJob::result_ready, this, &AnalysisTool::handle_eval_thread_complete);
    connect(job.data(), &ShapeEvaluationJob::report_progress, this, &AnalysisTool::handle_eval_thread_progress);
    worker->run_job(job);
  }

  this->evals_ready_ = true;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_tabWidget_currentChanged()
{
  this->update_analysis_mode();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaSlider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  emit pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_group_slider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  //QCoreApplication::processEvents();

  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->difference_button->setChecked(false);
  this->ui_->mean_button->setChecked(false);

  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaModeSpinBox_valueChanged(int i)
{
  emit pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_animate_state_changed()
{
  if (this->pcaAnimate()) {
    this->pca_animate_timer_.setInterval(10);
    this->pca_animate_timer_.start();
  }
  else {
    this->pca_animate_timer_.stop();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_timer()
{
  if (!this->pcaAnimate()) {
    return;
  }

  int value = this->ui_->pcaSlider->value();
  if (this->pca_animate_direction_) {
    value += this->ui_->pcaSlider->singleStep();
  }
  else {
    value -= this->ui_->pcaSlider->singleStep();
  }

  if (value >= this->ui_->pcaSlider->maximum() || value <= this->ui_->pcaSlider->minimum()) {
    this->pca_animate_direction_ = !this->pca_animate_direction_;
  }

  this->ui_->pcaSlider->setValue(value);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_animate_state_changed()
{
  if (this->ui_->group_animate_checkbox->isChecked()) {
    this->group_animate_timer_.setInterval(10);
    this->group_animate_timer_.start();
  }
  else {
    this->group_animate_timer_.stop();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_timer()
{
  int value = this->ui_->group_slider->value();
  if (this->group_animate_direction_) {
    value += this->ui_->group_slider->singleStep();
  }
  else {
    value -= this->ui_->group_slider->singleStep();
  }

  if (value >= this->ui_->group_slider->maximum() || value <= this->ui_->group_slider->minimum()) {
    this->group_animate_direction_ = !this->group_animate_direction_;
    //this->setPregenSteps();
  }

  this->ui_->group_slider->setValue(value);
}

//---------------------------------------------------------------------------
double AnalysisTool::get_pca_value()
{
  int slider_value = this->ui_->pcaSlider->value();
  float range = preferences_.get_pca_range();
  int halfRange = this->ui_->pcaSlider->maximum();

  double value = (double) slider_value / (double) halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void AnalysisTool::pca_labels_changed(QString value, QString eigen, QString lambda)
{
  this->setLabels(QString("pca"), value);
  this->setLabels(QString("eigen"), eigen);
  this->setLabels(QString("lambda"), lambda);
}

//---------------------------------------------------------------------------
void AnalysisTool::updateSlider()
{
  auto steps = this->preferences_.get_pca_steps();
  auto sliderRange = this->ui_->pcaSlider->maximum() - this->ui_->pcaSlider->minimum();
  this->ui_->pcaSlider->setSingleStep(sliderRange / steps);
}

//---------------------------------------------------------------------------
void AnalysisTool::reset_stats()
{
  this->ui_->tabWidget->setCurrentWidget(this->ui_->mean_tab);
  this->ui_->allSamplesRadio->setChecked(true);
  this->ui_->singleSamplesRadio->setChecked(false);
  this->ui_->sampleSpinBox->setEnabled(false);
  this->ui_->medianButton->setEnabled(false);
  this->ui_->pcaSlider->setEnabled(false);
  this->ui_->pcaAnimateCheckBox->setEnabled(false);
  this->ui_->pcaModeSpinBox->setEnabled(false);
  this->ui_->pcaAnimateCheckBox->setChecked(false);
  this->stats_ready_ = false;
  this->evals_ready_ = false;
  this->stats_ = ParticleShapeStatistics();
}

//---------------------------------------------------------------------------
void AnalysisTool::enable_actions(bool newly_enabled)
{
  if (this->session_->get_num_shapes() < 1) {
    return;
  }

  if (newly_enabled) {
    this->ui_->mesh_warping_radio_button->setChecked(true);
    this->update_domain_alignment_box();
  }

  auto domain_types = this->session_->get_domain_types();
  bool image_domain = domain_types.size() > 0 && domain_types[0] == DomainType::Image;
  this->ui_->distance_transfom_radio_button->setEnabled(
    this->session_->particles_present() && this->session_->get_groomed_present() && image_domain);

  this->ui_->mesh_warping_radio_button->setEnabled(
    this->session_->particles_present() && this->session_->get_groomed_present());

  if (!this->ui_->mesh_warping_radio_button->isEnabled()) {
    this->ui_->legacy_radio_button->setChecked(true);
  }
  this->reconstruction_method_changed();

  this->update_group_boxes();
  this->ui_->sampleSpinBox->setMaximum(this->session_->get_num_shapes() - 1);
}

//---------------------------------------------------------------------------
void AnalysisTool::set_analysis_mode(std::string mode)
{
  if (mode == "all samples" || mode == "single sample") {
    this->ui_->allSamplesRadio->setChecked(mode == "all samples");
    this->ui_->singleSamplesRadio->setChecked(mode == "single sample");
    this->ui_->tabWidget->setCurrentWidget(this->ui_->samples_tab);
  }
  else if (mode == "mean") {
    this->ui_->tabWidget->setCurrentWidget(this->ui_->mean_tab);
  }
  else if (mode == "pca") {
    this->ui_->tabWidget->setCurrentWidget(this->ui_->pca_tab);
  }
  else if (mode == "regression") {
    this->ui_->tabWidget->setCurrentWidget(this->ui_->regression_tab);
  }
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mean_shape()
{
  auto shape_points = this->get_mean_shape_points();
  ShapeHandle shape = this->create_shape_from_points(shape_points);
  if (this->get_group_difference_mode()) {
    shape->set_vectors(this->get_group_difference_vectors());
  }
  if (this->ui_->group_p_values_button->isChecked() && this->group_pvalue_job_ &&
      this->group_pvalue_job_->get_group_pvalues().rows() > 0) {
    shape->set_point_features("p_values", this->group_pvalue_job_->get_group_pvalues());
    shape->set_override_feature("p_values");
    shape->set_vectors({});
  }

  int num_points = shape_points.get_combined_global_particles().size() / 3;
  std::vector<Eigen::VectorXf> values;

  if (this->feature_map_ != "") {
    auto shapes = this->session_->get_shapes();
    Eigen::VectorXf sum(num_points);
    sum.setZero();

    bool ready = true;

    if (!this->groups_active()) {
      for (int i = 0; i < shapes.size(); i++) {
        shapes[i]->load_feature(Visualizer::MODE_RECONSTRUCTION_C, this->feature_map_);
        auto value = shapes[i]->get_point_features(this->feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        }
        else {
          values.push_back(value);
          sum = sum + value;
        }
      }
      Eigen::VectorXf mean = sum / values.size();

      if (ready) {
        shape->set_point_features(this->feature_map_, mean);
      }
    }
    else {
      Eigen::VectorXf sum_left(num_points);
      sum_left.setZero();
      Eigen::VectorXf sum_right(num_points);
      sum_right.setZero();

      for (auto shape : this->group1_list_) {
        shape->load_feature(Visualizer::MODE_RECONSTRUCTION_C, this->feature_map_);
        auto value = shape->get_point_features(this->feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        }
        else {
          sum_left = sum_left + value;
        }
      }
      Eigen::VectorXf left_mean = sum_left / static_cast<double>(this->group1_list_.size());

      for (auto shape : this->group2_list_) {
        shape->load_feature(Visualizer::MODE_RECONSTRUCTION_C, this->feature_map_);
        auto value = shape->get_point_features(this->feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        }
        else {
          sum_right = sum_right + value;
        }
      }
      Eigen::VectorXf right_mean = sum_right / static_cast<double>(this->group2_list_.size());

      if (ready) {
        double ratio = this->get_group_value();
        auto blend = left_mean * (1 - ratio) + right_mean * ratio;
        shape->set_point_features(this->feature_map_, blend);
      }
    }
  }

  return shape;
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::create_shape_from_points(StudioParticles points)
{
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(this->session_->get_mesh_manager());
  shape->set_particles(points);
  shape->get_reconstructed_meshes();
  shape->set_reconstruction_transforms(this->reconstruction_transforms_);
  return shape;
}

//---------------------------------------------------------------------------
void AnalysisTool::set_feature_map(const std::string& feature_map)
{
  this->feature_map_ = feature_map;
}

//---------------------------------------------------------------------------
std::string AnalysisTool::get_display_feature_map()
{
  if (this->ui_->group_p_values_button->isChecked() && this->group_pvalue_job_ &&
      this->group_pvalue_job_->get_group_pvalues().rows() > 0) {
    return "p_values";
  }
  return this->feature_map_;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_boxes()
{
  // populate the group sets
  auto group_names = this->session_->get_project()->get_group_names();

  this->ui_->group_widget->setEnabled(!group_names.empty());

  if (group_names != this->current_group_names_) { // only update if different
    this->ui_->group_box->clear();
    this->ui_->group_box->addItem("-None-");
    for (const std::string& group : group_names) {
      this->ui_->group_box->addItem(QString::fromStdString(group));
    }
    this->current_group_names_ = group_names;
    this->group_changed();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_values()
{
  auto values = this->session_->get_project()->
                get_group_values(std::string("group_")
                                 + this->ui_->group_box->currentText().toStdString());

  if (values != this->current_group_values_) {
    // populate group values
    this->ui_->group_left->clear();
    this->ui_->group_right->clear();
    for (const std::string& value : values) {
      QString item = QString::fromStdString(value);
      this->ui_->group_left->addItem(item);
      this->ui_->group_right->addItem(item);
    }
    this->current_group_values_ = values;

    // try to set the right one to a different value than left
    size_t i = 0;
    while (this->ui_->group_left->currentIndex() == this->ui_->group_right->currentIndex() &&
           i < this->current_group_values_.size()) {
      this->ui_->group_right->setCurrentIndex(i++);
    }
  }

  bool groups_on = this->ui_->group_box->currentText() != "-None-";

  this->ui_->group1_button->setEnabled(groups_on);
  this->ui_->group2_button->setEnabled(groups_on);
  this->ui_->difference_button->setEnabled(groups_on);
  this->ui_->group_p_values_button->setEnabled(groups_on);
  this->ui_->group_slider->setEnabled(groups_on);
  this->ui_->group_left->setEnabled(groups_on);
  this->ui_->group_right->setEnabled(groups_on);
  this->ui_->group_animate_checkbox->setEnabled(groups_on);

  this->group_changed();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_domain_alignment_box()
{
  auto domain_names = this->session_->get_project()->get_domain_names();

  bool multiple_domains = domain_names.size() > 1;
  this->ui_->reference_domain_widget->setVisible(multiple_domains);
  this->ui_->reference_domain->clear();
  if (multiple_domains) {
    this->ui_->reference_domain->addItem("Global Alignment");
    this->ui_->reference_domain->addItem("Local Alignment");
    for (auto name : domain_names) {
      this->ui_->reference_domain->addItem(QString::fromStdString(name));
    }
    this->ui_->reference_domain->setCurrentIndex(0);
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::group_changed()
{
  this->stats_ready_ = false;
  this->group_pvalue_job_ = nullptr;
  this->compute_stats();
}

//---------------------------------------------------------------------------
bool AnalysisTool::groups_active()
{
  std::string group_set = this->ui_->group_box->currentText().toStdString();
  bool groups_enabled = group_set != "" && group_set != "-None-";
  return groups_enabled;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_view_open_button_toggled()
{
  bool show = this->ui_->view_open_button->isChecked();
  this->ui_->view_content->setVisible(show);
}

//---------------------------------------------------------------------------
void AnalysisTool::on_surface_open_button_toggled()
{
  bool show = this->ui_->surface_open_button->isChecked();
  this->ui_->surface_content->setVisible(show);
}

//---------------------------------------------------------------------------
void AnalysisTool::on_metrics_open_button_toggled()
{
  bool show = this->ui_->metrics_open_button->isChecked();
  this->ui_->metrics_content->setVisible(show);

  if (show) {
    this->compute_stats();
  }
}

//---------------------------------------------------------------------------
bool AnalysisTool::is_group_active(int shape_index)
{
  std::string group_set = this->ui_->group_box->currentText().toStdString();
  std::string left_group = this->ui_->group_left->currentText().toStdString();
  std::string right_group = this->ui_->group_right->currentText().toStdString();

  bool groups_enabled = this->groups_active();

  auto shapes = this->session_->get_shapes();
  auto shape = shapes[shape_index];

  bool left = false;
  bool right = false;
  bool both = true;
  if (this->ui_->group1_button->isChecked()) {
    both = false;
    left = true;
  }
  else if (this->ui_->group2_button->isChecked()) {
    both = false;
    right = true;
  }

  if (groups_enabled) {
    auto value = shape->get_subject()->get_group_value(group_set);
    if (left && value == left_group) {
      return true;
    }
    else if (right && value == right_group) {
      return true;
    }
    else if (both) {
      return true;
    }
    else { return false; }
  }

  return true;
}

//---------------------------------------------------------------------------
void AnalysisTool::initialize_mesh_warper()
{
  if (this->session_->particles_present() && this->session_->get_groomed_present()) {

    this->compute_stats();
    int median = this->stats_.ComputeMedianShape(-32); //-32 = both groups

    if (median < 0 || median >= this->session_->get_num_shapes()) {
      STUDIO_LOG_ERROR("Unable to set reference mesh, stats returned invalid median index");
      return;
    }
    QSharedPointer<Shape> median_shape = this->session_->get_shapes()[median];

    auto mesh_group = median_shape->get_groomed_meshes(true);

    if (!mesh_group.valid()) {
      STUDIO_LOG_ERROR("Unable to set reference mesh, groomed mesh is unavailable");
      return;
    }
    auto meshes = mesh_group.meshes();
    for (int i = 0; i < mesh_group.meshes().size(); i++) {

      vnl_vector<double> particles = median_shape->get_particles().get_local_particles(i);
      Eigen::MatrixXd points = Eigen::Map<const Eigen::VectorXd>(
        (double*) particles.data_block(), particles.size());
      points.resize(3, points.size() / 3);
      points.transposeInPlace();

      this->session_->get_mesh_manager()->get_mesh_warper(i)->set_reference_mesh(
        meshes[i]->get_poly_data(), points);
    }
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_thread_complete(ShapeEvaluationJob::JobType job_type,
                                               Eigen::VectorXd data)
{
  switch (job_type) {
  case ShapeEvaluationJob::JobType::CompactnessType:
    this->eval_compactness_ = data;
    this->create_plot(this->ui_->compactness_graph, data, "Compactness", "Number of Modes",
                      "Explained Variance");
    this->ui_->compactness_graph->show();
    this->ui_->compactness_progress_widget->hide();
    break;
  case ShapeEvaluationJob::JobType::SpecificityType:
    this->create_plot(this->ui_->specificity_graph, data, "Specificity", "Number of Modes",
                      "Specificity");
    this->eval_specificity_ = data;
    this->ui_->specificity_graph->show();
    this->ui_->specificity_progress_widget->hide();
    break;
  case ShapeEvaluationJob::JobType::GeneralizationType:
    this->create_plot(this->ui_->generalization_graph, data, "Generalization", "Number of Modes",
                      "Generalization");
    this->eval_generalization_ = data;
    this->ui_->generalization_graph->show();
    this->ui_->generalization_progress_widget->hide();
    break;
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_eval_thread_progress(ShapeEvaluationJob::JobType job_type,
                                               float progress)
{
  switch (job_type) {
  case ShapeEvaluationJob::JobType::CompactnessType:
    this->ui_->compactness_progress->setValue(progress * 100);
    break;
  case ShapeEvaluationJob::JobType::SpecificityType:
    this->ui_->specificity_progress->setValue(progress * 100);
    break;
  case ShapeEvaluationJob::JobType::GeneralizationType:
    this->ui_->generalization_progress->setValue(progress * 100);
    break;
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_pvalues_complete()
{
  emit progress(100);
  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_alignment_changed(int new_alignment)
{
  new_alignment -= 2;
  if (new_alignment == this->current_alignment_) {
    return;
  }
  this->current_alignment_ = static_cast<AlignmentType>(new_alignment);

  for (ShapeHandle shape : this->session_->get_shapes()) {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (this->current_alignment_ == AlignmentType::Local) {
      transform = nullptr;
    }
    else if (this->current_alignment_ == AlignmentType::Global) {
      auto domain_names = this->session_->get_project()->get_domain_names();
      transform = shape->get_groomed_transform(domain_names.size());
    }
    else {
      transform = shape->get_groomed_transform(new_alignment);
    }

    shape->set_particle_transform(transform);
  }

  this->evals_ready_ = false;
  this->group_changed();
  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::reconstruction_method_changed()
{
  this->ui_->reconstruction_options->setVisible(
    this->ui_->distance_transfom_radio_button->isChecked());
  std::string method = MeshGenerator::RECONSTRUCTION_LEGACY_C;
  if (this->ui_->distance_transfom_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_DISTANCE_TRANSFORM_C;
  }
  else if (this->ui_->mesh_warping_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_MESH_WARPER_C;
  }

  auto previous_method =
    this->session_->get_mesh_manager()->get_mesh_generator()->get_reconstruction_method();
  if (previous_method != method) {
    this->session_->get_mesh_manager()->get_mesh_generator()->set_reconstruction_method(method);
    this->session_->handle_clear_cache();
    emit reconstruction_complete();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::set_active(bool active)
{
  if (!active) {
    this->ui_->pcaAnimateCheckBox->setChecked(false);
    this->pca_animate_timer_.stop();
  }
  this->active_ = active;
}

//---------------------------------------------------------------------------
bool AnalysisTool::get_active()
{
  return this->active_;
}

//---------------------------------------------------------------------------
StudioParticles AnalysisTool::convert_from_combined(const vnl_vector<double>& points)
{
  StudioParticles particles;
  if (this->session_->get_shapes().empty()) {
    return particles;
  }
  auto base = this->session_->get_shapes()[0]->get_particles();

  auto worlds = base.get_world_particles();
  int idx = 0;
  for (int d = 0; d < worlds.size(); d++) {
    vnl_vector<double> new_world(worlds[d].size());
    for (int i = 0; i < worlds[d].size(); i++) {
      new_world[i] = points[idx++];
    }
    particles.set_world_particles(d, new_world);
  }

  return particles;
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_reconstructed_domain_transforms()
{
  auto shapes = this->session_->get_shapes();
  if (this->current_alignment_ == AlignmentType::Local) {

    this->reconstruction_transforms_.resize(this->session_->get_domains_per_shape());
    for (int domain = 0; domain < this->session_->get_domains_per_shape(); domain++) {
      int num_shapes = shapes.size();
      auto transform = vtkSmartPointer<vtkTransform>::New();
      double* values = transform->GetMatrix()->GetData();

      for (int i = 0; i < shapes.size(); i++) {
        vtkSmartPointer<vtkTransform> base = shapes[i]->get_alignment(0);
        vtkSmartPointer<vtkTransform> offset = shapes[i]->get_alignment(domain);

        for (int j = 0; j < 16; j++) {
          values[j] += (base->GetMatrix()->GetData()[j] -
                        offset->GetMatrix()->GetData()[j]) / num_shapes;
        }
      }
      this->reconstruction_transforms_[domain] = transform;
    }
  }
  else {
    this->reconstruction_transforms_.clear();
  }
  for (int s = 0; s < shapes.size(); s++) {
    shapes[s]->set_reconstruction_transforms(this->reconstruction_transforms_);
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title,
                               QString x_label, QString y_label)
{
  JKQTPDatastore* ds = plot->getDatastore();

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
  //plot->getPlotter()->setPlotLabel("\\textbf{"+title+"}");
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
}
