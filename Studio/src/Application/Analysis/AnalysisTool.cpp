// std
#include <iostream>

// qt
#include <QXmlStreamWriter>
#include <QThread>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

// shapeworks
#include <Visualization/ShapeWorksStudioApp.h>
#include <Visualization/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>
#include <Analysis/AnalysisTool.h>
#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>

#include <ui_AnalysisTool.h>

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

  // defautl to linear scale
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
  connect(this->ui_->group_animate_checkbox, &QCheckBox::clicked, this,
          &AnalysisTool::handle_group_animate_state_changed);
  connect(&this->group_animate_timer_, &QTimer::timeout, this, &AnalysisTool::handle_group_timer);

  connect(this->ui_->pca_radio_button, &QRadioButton::clicked, this, &AnalysisTool::pca_update);
  connect(this->ui_->group_radio_button, &QRadioButton::clicked, this, &AnalysisTool::pca_update);
}

//---------------------------------------------------------------------------
std::string AnalysisTool::get_analysis_mode()
{
  if (this->ui_->tabWidget->currentWidget() == this->ui_->samples_tab) {
    if (this->ui_->allSamplesRadio->isChecked()) { return AnalysisTool::MODE_ALL_SAMPLES_C;}
    if (this->ui_->singleSamplesRadio->isChecked()) { return AnalysisTool::MODE_SINGLE_SAMPLE_C;}
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
std::vector<Point> AnalysisTool::get_group_difference_vectors()
{
  std::vector<Point> vecs;

  auto num_points = this->stats_.Mean().size() / 3;
  for (unsigned int i = 0; i < num_points; i++) {
    Point tmp;
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
  //this->ui_->run_optimize_button->setEnabled(false);
  this->ui_->reconstructionButton->setEnabled(false);
  QThread* thread = new QThread;
  std::vector<std::vector<itk::Point<double>>> local, global;
  std::vector<ImageType::Pointer> images;
  auto shapes = this->session_->get_shapes();
  local.resize(shapes.size());
  global.resize(shapes.size());
  images.resize(shapes.size());
  size_t ii = 0;
  for (auto &s : shapes) {
    auto l = s->get_local_correspondence_points();
    auto g = s->get_global_correspondence_points();
    for (size_t i = 0; i < l.size(); i += 3) {
      itk::Point<double> pt, pt2;
      pt[0] = l[i];
      pt[1] = l[i + 1];
      pt[2] = l[i + 2];
      pt2[0] = g[i];
      pt2[1] = g[i + 1];
      pt2[2] = g[i + 2];
      local[ii].push_back(pt);
      global[ii].push_back(pt2);
    }
    images[ii] = s->get_groomed_image();
    ii++;
  }
  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::ReconstructType, NULL, NULL, this->session_,
    local, global, images,
    this->ui_->maxAngle->value(),
    this->ui_->meshDecimation->value(),
    this->ui_->numClusters->value());
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_reconstruction_complete()));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(warning_message(std::string)), this, SLOT(handle_warning(std::string)));
  connect(worker, SIGNAL(message(std::string)), this, SLOT(handle_message(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
  emit progress(15);
}

//---------------------------------------------------------------------------
int AnalysisTool::getPCAMode()
{
  return this->ui_->pcaModeSpinBox->value();
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
bool AnalysisTool::groupAnimate()
{
  return this->ui_->group_animate_checkbox->isChecked();
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
int AnalysisTool::getSampleNumber()
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
}

//---------------------------------------------------------------------------
void AnalysisTool::set_app(ShapeWorksStudioApp* app)
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void AnalysisTool::activate()
{
  this->update_analysis_mode();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_analysis_mode()
{
  // update UI

  this->handle_analysis_options();
  // groups available
}

//---------------------------------------------------------------------------
void AnalysisTool::compute_mode_shape()
{}

//---------------------------------------------------------------------------
void AnalysisTool::handle_analysis_options()
{
  if (this->ui_->tabWidget->currentWidget() == this->ui_->samples_tab) {
    if (this->ui_->singleSamplesRadio->isChecked()) {
      //one sample mode
      this->ui_->sampleSpinBox->setEnabled(true);
      this->ui_->medianButton->setEnabled(true);
      this->ui_->pcaSlider->setEnabled(false);
      this->ui_->pcaAnimateCheckBox->setEnabled(false);
      this->ui_->pcaModeSpinBox->setEnabled(false);
      this->ui_->pcaAnimateCheckBox->setChecked(false);
    }
    else {
      //all samples mode
      this->ui_->sampleSpinBox->setEnabled(false);
      this->ui_->medianButton->setEnabled(false);
      this->ui_->pcaSlider->setEnabled(false);
      this->ui_->pcaAnimateCheckBox->setEnabled(false);
      this->ui_->pcaModeSpinBox->setEnabled(false);
      this->ui_->pcaAnimateCheckBox->setChecked(false);
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
  }

  this->ui_->group1_button->setEnabled(this->session_->groups_available());
  this->ui_->group2_button->setEnabled(this->session_->groups_available());
  this->ui_->difference_button->setEnabled(this->session_->groups_available());
  this->ui_->group_slider_widget->setEnabled(this->session_->groups_available());

  emit update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_median()
{
  if (!this->compute_stats()) { return;}
  this->ui_->sampleSpinBox->setValue(this->stats_.ComputeMedianShape(-32));       //-32 = both groups
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_overall_button_clicked()
{
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group1_button_clicked()
{
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_group2_button_clicked()
{
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_difference_button_clicked()
{
  emit update_view();
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

  std::vector < vnl_vector < double >> points;
  std::vector<int> group_ids;
  foreach(ShapeHandle shape, this->session_->get_shapes()) {
    points.push_back(shape->get_global_correspondence_points());
    group_ids.push_back(shape->get_group_id());
  }

  this->stats_.ImportPoints(points, group_ids);

  this->stats_.ComputeModes();
  this->stats_ready_ = true;
  std::vector<double> vals;
  for (int i = this->stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(this->stats_.Eigenvalues()[i]);
  }
  this->ui_->graph_->set_data(vals);

  this->ui_->graph_->repaint();

  // set widget enable state for groups
  bool groups_available = this->session_->groups_available();
  this->ui_->group_slider->setEnabled(groups_available);
  this->ui_->group_animate_checkbox->setEnabled(groups_available);
  this->ui_->group_radio_button->setEnabled(groups_available);

  return true;
}

//-----------------------------------------------------------------------------
const vnl_vector<double>& AnalysisTool::get_mean_shape()
{
  if (!this->compute_stats()) {
    return this->empty_shape_;
  }

  if (this->ui_->group1_button->isChecked()) {
    return this->stats_.Group1Mean();
  }
  else if (this->ui_->group2_button->isChecked()) {
    return this->stats_.Group2Mean();
  }

  return this->stats_.Mean();
}

//-----------------------------------------------------------------------------
const vnl_vector<double>& AnalysisTool::get_shape(int mode, double value, double group_value)
{
  if (!this->compute_stats() || this->stats_.Eigenvectors().size() <= 1) {
    return this->empty_shape_;
  }
  if (mode + 2 > this->stats_.Eigenvalues().size()) {mode = this->stats_.Eigenvalues().size() - 2;}

  unsigned int m = this->stats_.Eigenvectors().columns() - (mode + 1);

  vnl_vector<double> e = this->stats_.Eigenvectors().get_column(m);

  double lambda = sqrt(this->stats_.Eigenvalues()[m]);

  this->pca_labels_changed(QString::number(value, 'g', 2),
                           QString::number(this->stats_.Eigenvalues()[m]),
                           QString::number(value * lambda));

  if (this->ui_->group_radio_button->isChecked()) {
    this->temp_shape_ = this->stats_.Group1Mean() + (this->stats_.GroupDifference() * group_value);
  }
  else {
    this->temp_shape_ = this->stats_.Mean() + (e * (value * lambda));
  }

  return this->temp_shape_;
}

//---------------------------------------------------------------------------
ParticleShapeStatistics<3> AnalysisTool::get_stats()
{
  this->compute_stats();
  return this->stats_;
}

//---------------------------------------------------------------------------
void AnalysisTool::load_settings()
{
  Settings settings = this->session_->get_project()->get_settings(Settings::ANALYSIS_SETTINGS);
  this->ui_->numClusters->setValue(settings.get("reconstruction_clusters", 3));
  this->ui_->meshDecimation->setValue(settings.get("reconstruction_decimation", 0.30));
  this->ui_->maxAngle->setValue(settings.get("reconstruction_max_angle", 60));
}

//---------------------------------------------------------------------------
void AnalysisTool::store_settings()
{
  Settings settings = this->session_->get_project()->get_settings(Settings::ANALYSIS_SETTINGS);
  settings.set("reconstruction_clusters", this->ui_->numClusters->value());
  settings.set("reconstruction_decimation", this->ui_->meshDecimation->value());
  settings.set("reconstruction_max_angle", this->ui_->maxAngle->value());
  this->session_->get_project()->set_settings(Settings::ANALYSIS_SETTINGS, settings);
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
  QCoreApplication::processEvents();

  emit pca_update();
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
    //this->setPregenSteps();
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
  int value = this->ui_->pcaSlider->value();
  if (this->pca_animate_direction_) {
    value += this->ui_->pcaSlider->singleStep();
  }
  else {
    value -= this->ui_->pcaSlider->singleStep();
  }

  if (value >= this->ui_->pcaSlider->maximum() || value <= this->ui_->pcaSlider->minimum()) {
    this->pca_animate_direction_ = !this->pca_animate_direction_;
    //this->setPregenSteps();
  }

  this->ui_->pcaSlider->setValue(value);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_group_animate_state_changed()
{
  if (this->groupAnimate()) {
    //this->setPregenSteps();
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

  double value = (double)slider_value / (double)halfRange * range;
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
  this->stats_ = ParticleShapeStatistics<3>();
}

//---------------------------------------------------------------------------
void AnalysisTool::enable_actions()
{
  std::cerr << "particles present = " << this->session_->particles_present() << "\n";
  std::cerr << "groomed present = " << this->session_->get_groomed_present() << "\n";

  this->ui_->reconstructionButton->setEnabled(
    this->session_->particles_present() && this->session_->get_groomed_present());
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
