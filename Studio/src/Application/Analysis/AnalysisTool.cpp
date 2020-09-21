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
#include <Data/StudioLog.h>
#include <Analysis/AnalysisTool.h>
#include <Visualization/Lightbox.h>

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
  connect(this->ui_->group_animate_checkbox, &QCheckBox::stateChanged, this,
          &AnalysisTool::handle_group_animate_state_changed);
  connect(&this->group_animate_timer_, &QTimer::timeout, this, &AnalysisTool::handle_group_timer);

  connect(this->ui_->group_box, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::update_group_values);

  connect(this->ui_->group_left, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::group_changed);

  connect(this->ui_->group_right, qOverload<const QString&>(&QComboBox::currentIndexChanged),
          this, &AnalysisTool::group_changed);

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
  for (auto& s : shapes) {
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

  connect(worker, &ShapeworksWorker::error_message, this, &AnalysisTool::handle_error);
  connect(worker, &ShapeworksWorker::warning_message, this, &AnalysisTool::handle_warning);
  connect(worker, &ShapeworksWorker::message, this, &AnalysisTool::handle_message);

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
}

//---------------------------------------------------------------------------
void AnalysisTool::set_app(ShapeWorksStudioApp* app)
{
  this->app_ = app;
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
  emit update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_difference_button_clicked()
{
  this->ui_->group_slider->setValue(this->ui_->group_slider->minimum());
  this->ui_->mean_button->setChecked(false);
  this->ui_->group1_button->setChecked(false);
  this->ui_->group2_button->setChecked(false);
  this->ui_->group_animate_checkbox->setChecked(false);
  this->ui_->difference_button->setChecked(true);
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

  std::vector<vnl_vector<double>> points;
  std::vector<int> group_ids;

  std::string group_set = this->ui_->group_box->currentText().toStdString();
  std::string left_group = this->ui_->group_left->currentText().toStdString();
  std::string right_group = this->ui_->group_right->currentText().toStdString();

  bool groups_enabled = group_set != "-None-";

  for (ShapeHandle shape : this->session_->get_shapes()) {

    if (groups_enabled) {
      auto value = shape->get_subject()->get_group_value(group_set);
      if (value == left_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(1);
      }
      else if (value == right_group) {
        points.push_back(shape->get_global_correspondence_points());
        group_ids.push_back(2);
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

  this->stats_.ImportPoints(points, group_ids);
  this->stats_.ComputeModes();
  this->stats_.PrincipalComponentProjections();

  this->stats_ready_ = true;
  std::vector<double> vals;
  for (int i = this->stats_.Eigenvalues().size() - 1; i > 0; i--) {
    vals.push_back(this->stats_.Eigenvalues()[i]);
  }
  this->ui_->graph_->set_data(vals);

  this->ui_->graph_->repaint();

  return true;
}

//-----------------------------------------------------------------------------
const vnl_vector<double>& AnalysisTool::get_mean_shape_points()
{
  if (!this->compute_stats()) {
    return this->empty_shape_;
  }

  if (this->ui_->group1_button->isChecked() || this->ui_->difference_button->isChecked()) {
    return this->stats_.Group1Mean();
  }
  else if (this->ui_->group2_button->isChecked()) {
    return this->stats_.Group2Mean();
  }
  else if (this->ui_->mean_button->isChecked()) {
    return this->stats_.Mean();
  }

  if (this->groups_active()) {
    auto group_value = this->get_group_value();
    this->temp_shape_ = this->stats_.Group1Mean() + (this->stats_.GroupDifference() * group_value);
    return this->temp_shape_;
  }

  return this->stats_.Mean();
}

//-----------------------------------------------------------------------------
const vnl_vector<double>& AnalysisTool::get_shape_points(int mode, double value, double group_value)
{
  if (!this->compute_stats() || this->stats_.Eigenvectors().size() <= 1) {
    return this->empty_shape_;
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

  ///TODO: fix for multi-group
  /*
  if (this->ui_->group_radio_button->isChecked()) {
    this->temp_shape_ = this->stats_.Group1Mean() + (this->stats_.GroupDifference() * group_value);
  }
  else {
    this->temp_shape_ = this->stats_.Mean() + (e * (value * lambda));
  }
*/

  this->temp_shape_ = this->stats_.Mean() + (e * (value * lambda));

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
  if (this->ui_->group_animate_checkbox->isChecked()) {
    //this->setPregenSteps();
    std::cerr << "group animating\n";
    this->group_animate_timer_.setInterval(10);
    this->group_animate_timer_.start();
  }
  else {
    std::cerr << "group NOT animating\n";
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
  this->stats_ = ParticleShapeStatistics<3>();
}

//---------------------------------------------------------------------------
void AnalysisTool::enable_actions()
{
  this->ui_->reconstructionButton->setEnabled(
    this->session_->particles_present() && this->session_->get_groomed_present());

  this->update_group_boxes();
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
ShapeHandle AnalysisTool::get_shape(int mode, double value, double group_value)
{
  return this->create_shape_from_points(this->get_shape_points(mode, value, group_value));
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mean_shape()
{
  auto shape_points = this->get_mean_shape_points();
  ShapeHandle shape = this->create_shape_from_points(shape_points);
  if (this->get_group_difference_mode()) {
    shape->set_vectors(this->get_group_difference_vectors());
  }

  int num_points = shape_points.size() / 3;
  std::vector<Eigen::VectorXf> values;

  if (this->feature_map_ != "") {
    auto shapes = this->session_->get_shapes();
    Eigen::VectorXf sum(num_points);
    sum.setZero();
    bool ready = true;
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
    auto mean = sum / shapes.size();

    if (ready) {
      shape->set_point_features(this->feature_map_, mean);
    }
  }

  return shape;
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::create_shape_from_points(const vnl_vector<double>& points)
{
  MeshHandle mesh = this->session_->get_mesh_manager()->get_mesh(points);
  ShapeHandle shape = ShapeHandle(new Shape());
  shape->set_mesh_manager(this->session_->get_mesh_manager());
  shape->set_reconstructed_mesh(mesh);
  shape->set_global_particles(points);
  return shape;
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_error(std::string message_string)
{
  STUDIO_LOG_ERROR(QString::fromStdString(message_string));
  emit error(message_string);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_warning(std::string message_string)
{
  STUDIO_LOG_ERROR(QString::fromStdString(message_string));
  emit error(message_string);
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_message(std::string message_string)
{
  STUDIO_LOG_MESSAGE(QString::fromStdString(message_string));
  emit message(message_string);
}

//---------------------------------------------------------------------------
void AnalysisTool::set_feature_map(const std::string& feature_map)
{
  this->feature_map_ = feature_map;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_boxes()
{
  // populate the group sets
  auto group_names = this->session_->get_project()->get_group_names();

  if (group_names != this->current_group_names_) { // only update if different
    this->ui_->group_box->clear();
    this->ui_->group_box->addItem("-None-");
    for (const std::string& group : group_names) {
      this->ui_->group_box->addItem(QString::fromStdString(group));
    }
    this->current_group_names_ = group_names;
  }

  this->group_changed();
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
    int i = 0;
    while (this->ui_->group_left->currentIndex() == this->ui_->group_right->currentIndex() &&
           i < this->current_group_values_.size()) {
      this->ui_->group_right->setCurrentIndex(i++);
    }
  }

  bool groups_on = this->ui_->group_box->currentText() != "-None-";

  this->ui_->group1_button->setEnabled(groups_on);
  this->ui_->group2_button->setEnabled(groups_on);
  this->ui_->difference_button->setEnabled(groups_on);
  this->ui_->group_slider->setEnabled(groups_on);
  this->ui_->group_left->setEnabled(groups_on);
  this->ui_->group_right->setEnabled(groups_on);
  this->ui_->group_animate_checkbox->setEnabled(groups_on);

  this->group_changed();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_changed()
{
  this->stats_ready_ = false;
  this->compute_stats();
}

//---------------------------------------------------------------------------
bool AnalysisTool::groups_active()
{
  std::string group_set = this->ui_->group_box->currentText().toStdString();
  bool groups_enabled = group_set != "" && group_set != "-None-";
  return groups_enabled;
}
