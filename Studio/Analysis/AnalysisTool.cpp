// qt
#include <QMessageBox>
#include <QThread>

// shapeworks
#include <Analysis/AnalysisTool.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Job/GroupPvalueJob.h>
#include <Job/NetworkAnalysisJob.h>
#include <Job/ParticleNormalEvaluationJob.h>
#include <Job/StatsGroupLDAJob.h>
#include <Libs/Application/Job/PythonWorker.h>
#include <Logging.h>
#include <QMeshWarper.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <Utils/AnalysisUtils.h>
#include <jkqtplotter/graphs/jkqtpscatter.h>
#include <jkqtplotter/jkqtplotter.h>
#include <ui_AnalysisTool.h>

#include <QClipboard>
#include <QMenu>

#include "ParticleAreaPanel.h"
#include "ShapeScalarPanel.h"

namespace shapeworks {

const std::string AnalysisTool::MODE_ALL_SAMPLES_C("all samples");
const std::string AnalysisTool::MODE_MEAN_C("mean");
const std::string AnalysisTool::MODE_PCA_C("pca");
const std::string AnalysisTool::MODE_SINGLE_SAMPLE_C("single sample");
const std::string AnalysisTool::MODE_REGRESSION_C("regression");

constexpr auto MESH_WARP_TEMPLATE_INDEX = "mesh_warp_template_index";

//---------------------------------------------------------------------------
//! Helper to extract x,y,z from x,y,z,scalar
static Eigen::VectorXd extract_shape_only(Eigen::VectorXd values) {
  // need to drop every 4th value
  Eigen::VectorXd particles(values.size() - values.size() / 4);
  int j = 0;
  for (int i = 0; i < values.size(); i++) {
    if (i % 4 != 3) {
      particles[j++] = values[i];
    }
  }
  return particles;
}

//---------------------------------------------------------------------------
//! Helper to extract scalar from x,y,z,scalar
static Eigen::VectorXd extract_scalar_only(Eigen::VectorXd values) {
  Eigen::VectorXd scalars(values.size() / 4);
  int j = 0;
  for (int i = 0; i < values.size(); i++) {
    if (i % 4 == 3) {
      scalars[j++] = values[i];
    }
  }
  return scalars;
}

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_AnalysisTool;
  ui_->setupUi(this);

  particle_area_panel_ = new ParticleAreaPanel(this);
  layout()->addWidget(particle_area_panel_);

  shape_scalar_panel_ = new ShapeScalarPanel(this);
  layout()->addWidget(shape_scalar_panel_);

  auto spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout()->addItem(spacer);

#ifdef Q_OS_MACOS
  ui_->tabWidget->tabBar()->setMinimumWidth(200);
  ui_->tabWidget->setStyleSheet(
      "QTabBar::tab:selected { border-radius: 4px; background-color: #3784f7; color: white; }");
  ui_->tabWidget->tabBar()->setElideMode(Qt::TextElideMode::ElideNone);
#endif

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

  // multi-level PCA settings
  connect(ui_->mcaLevelBetweenButton, &QPushButton::clicked, this, &AnalysisTool::pca_update);
  connect(ui_->mcaLevelWithinButton, &QPushButton::clicked, this, &AnalysisTool::pca_update);
  connect(ui_->vanillaPCAButton, &QPushButton::clicked, this, &AnalysisTool::pca_update);

  // shape/scalar
  connect(ui_->pca_scalar_only, &QCheckBox::clicked, this, &AnalysisTool::change_pca_analysis_type);
  connect(ui_->pca_scalar_shape_only, &QCheckBox::clicked, this, &AnalysisTool::change_pca_analysis_type);
  connect(ui_->pca_shape_and_scalar, &QCheckBox::clicked, this, &AnalysisTool::change_pca_analysis_type);
  connect(ui_->pca_scalar_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &AnalysisTool::change_pca_analysis_type);
  connect(ui_->pca_predict_scalar, &QCheckBox::clicked, this, &AnalysisTool::change_pca_analysis_type);
  connect(ui_->pca_predict_shape, &QCheckBox::clicked, this, &AnalysisTool::change_pca_analysis_type);

  // group animation
  connect(ui_->group_animate_checkbox, &QCheckBox::stateChanged, this,
          &AnalysisTool::handle_group_animate_state_changed);
  connect(&group_animate_timer_, &QTimer::timeout, this, &AnalysisTool::handle_group_timer);

  connect(ui_->group_combo, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::update_group_values);
  connect(ui_->group_left, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::group_changed);
  connect(ui_->group_right, qOverload<int>(&QComboBox::currentIndexChanged), this, &AnalysisTool::group_changed);
  connect(ui_->group_p_values_checkbox, &QPushButton::clicked, this, &AnalysisTool::group_p_values_clicked);

  connect(ui_->pca_group_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &AnalysisTool::update_pca_group_options);
  connect(ui_->pca_group_list, &QListWidget::itemChanged, this, &AnalysisTool::handle_pca_group_list_item_changed);

  // network analysis
  connect(ui_->network_analysis_button, &QPushButton::clicked, this, &AnalysisTool::network_analysis_clicked);
  connect(ui_->network_analysis_display, &QCheckBox::stateChanged, this, &AnalysisTool::update_view);
  connect(ui_->network_analysis_option, &QRadioButton::clicked, this, &AnalysisTool::update_view);
  connect(ui_->network_spm1d_option, &QRadioButton::clicked, this, &AnalysisTool::update_view);

  connect(ui_->reference_domain, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &AnalysisTool::handle_alignment_changed);

  connect(ui_->tabWidget, &QTabWidget::currentChanged, this, &AnalysisTool::handle_tab_changed);

  ui_->surface_open_button->setChecked(false);
  ui_->metrics_open_button->setChecked(false);

  /// TODO nothing there yet (regression tab)
  ui_->tabWidget->removeTab(3);

  for (auto button : {ui_->distance_transform_radio_button, ui_->mesh_warping_radio_button, ui_->legacy_radio_button}) {
    connect(button, &QRadioButton::clicked, this, &AnalysisTool::reconstruction_method_changed);
  }

  connect(ui_->run_good_bad, &QPushButton::clicked, this, &AnalysisTool::run_good_bad_particles);

  ui_->reconstruction_options->hide();
  ui_->mesh_warp_options->hide();
  ui_->particles_open_button->toggle();
  ui_->particles_progress->hide();

  ui_->lda_graph->hide();
  ui_->lda_hint_label->hide();
  group_lda_job_ = QSharedPointer<StatsGroupLDAJob>::create();
  connect(group_lda_job_.data(), &StatsGroupLDAJob::progress, this, &AnalysisTool::handle_lda_progress);
  connect(group_lda_job_.data(), &StatsGroupLDAJob::finished, this, &AnalysisTool::handle_lda_complete);

  connect(ui_->show_difference_to_mean, &QPushButton::clicked, this, &AnalysisTool::show_difference_to_mean_clicked);

  connect(ui_->group_analysis_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &AnalysisTool::group_analysis_combo_changed);

  // when one is checked, turn the other off, connect these first
  connect(ui_->show_difference_to_predicted_scalar, &QPushButton::clicked, this, [this]() {
    bool checked = ui_->show_difference_to_predicted_scalar->isChecked();
    if (checked) {
      ui_->show_predicted_scalar->setChecked(false);
    }
  });
  connect(ui_->show_predicted_scalar, &QPushButton::clicked, this, [this]() {
    if (ui_->show_predicted_scalar->isChecked()) {
      ui_->show_difference_to_predicted_scalar->setChecked(false);
    }
  });

  connect(ui_->show_difference_to_predicted_scalar, &QPushButton::clicked, this,
          &AnalysisTool::handle_samples_predicted_scalar_options);
  connect(ui_->show_predicted_scalar, &QPushButton::clicked, this,
          &AnalysisTool::handle_samples_predicted_scalar_options);
  connect(ui_->show_absolute_difference, &QPushButton::clicked, this,
          &AnalysisTool::handle_samples_predicted_scalar_options);

  // add a right click menu to the samples table allowing the user to copy the table to the clipboard
  ui_->samples_table->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui_->samples_table, &QTableWidget::customContextMenuRequested, this,
          &AnalysisTool::samples_table_context_menu);

  // disable editing of the table
  ui_->samples_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

  // only connect one so that we only get one signal
  connect(ui_->distance_method_particle, &QRadioButton::toggled, this, &AnalysisTool::handle_distance_method_changed);

  // mesh warp options
  connect(ui_->mesh_warp_median_button, &QPushButton::clicked, this, &AnalysisTool::mesh_warp_median_clicked);
  connect(ui_->mesh_warp_sample_spinbox, qOverload<int>(&QSpinBox::valueChanged), this,
          &AnalysisTool::mesh_warp_sample_changed);
  connect(ui_->mesh_warp_run_button, &QPushButton::clicked, this, &AnalysisTool::mesh_warp_run_clicked);
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
bool AnalysisTool::get_group_difference_mode() { return groups_active() && ui_->difference_button->isChecked(); }

//---------------------------------------------------------------------------
std::vector<Shape::Point> AnalysisTool::get_group_difference_vectors() {
  std::vector<Shape::Point> vecs;

  auto num_points = stats_.get_mean().size() / 3;
  for (unsigned int i = 0; i < num_points; i++) {
    Shape::Point tmp;
    tmp.x = stats_.get_group2_mean()[i * 3] - stats_.get_group1_mean()[i * 3];
    tmp.y = stats_.get_group2_mean()[i * 3 + 1] - stats_.get_group1_mean()[i * 3 + 1];
    tmp.z = stats_.get_group2_mean()[i * 3 + 2] - stats_.get_group1_mean()[i * 3 + 2];
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
QStringList AnalysisTool::get_checked_pca_groups() {
  QStringList checked_items;

  // Get the number of items in the list
  int count = ui_->pca_group_list->count();

  // Iterate through all items
  for (int i = 0; i < count; ++i) {
    QListWidgetItem* item = ui_->pca_group_list->item(i);

    // Check if the item is checked
    if (item && item->checkState() == Qt::Checked) {
      // Add the text of the checked item to our list
      checked_items.append(item->text());
    }
  }

  return checked_items;
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
  particle_area_panel_->set_session(session);
  shape_scalar_panel_->set_session(session);

  // reset to original
  ui_->mesh_warping_radio_button->setChecked(true);
  ui_->difference_button->setChecked(false);
  ui_->group_p_values_checkbox->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  update_difference_particles();
  ui_->pcaSlider->setValue(0);
  ui_->group_slider->setValue(10);

  ui_->show_predicted_scalar->setChecked(false);
  ui_->show_difference_to_predicted_scalar->setChecked(false);
  handle_samples_predicted_scalar_options();

  connect(ui_->show_good_bad, &QCheckBox::toggled, session_.data(), &Session::set_show_good_bad_particles);
}

//---------------------------------------------------------------------------
QSharedPointer<Session> AnalysisTool::get_session() { return session_; }

//---------------------------------------------------------------------------
void AnalysisTool::set_app(ShapeWorksStudioApp* app) { app_ = app; }

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
bool AnalysisTool::groups_on() { return ui_->group_combo->currentText() != "-None-"; }

//---------------------------------------------------------------------------
void AnalysisTool::handle_analysis_options() {
  if (ui_->tabWidget->currentWidget() == ui_->samples_tab) {
    ui_->pcaAnimateCheckBox->setChecked(false);
    ui_->pcaAnimateCheckBox->setEnabled(false);
    ui_->pcaModeSpinBox->setEnabled(false);
    pca_animate_timer_.stop();
    group_animate_timer_.stop();
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
    group_animate_timer_.stop();
    auto domain_names = session_->get_project()->get_domain_names();
    bool multiple_domains = domain_names.size() > 1;
    if (multiple_domains) {
      ui_->vanillaPCAButton->setEnabled(true);
      ui_->mcaLevelWithinButton->setEnabled(true);
      ui_->mcaLevelBetweenButton->setEnabled(true);
      ui_->vanillaPCAButton->setChecked(true);
    }
  } else {
    // regression mode
    ui_->sampleSpinBox->setEnabled(false);
    ui_->medianButton->setEnabled(false);
    ui_->pcaSlider->setEnabled(false);
    ui_->pcaAnimateCheckBox->setEnabled(false);
    ui_->pcaModeSpinBox->setEnabled(false);
    pca_animate_timer_.stop();
    group_animate_timer_.stop();
  }

  update_difference_particles();

  Q_EMIT analysis_mode_changed();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_median() {
  if (!compute_stats()) {
    return;
  }
  ui_->sampleSpinBox->setValue(stats_.compute_median_shape(-32));  //-32 = both groups
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
  ui_->group_p_values_checkbox->setChecked(false);
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
  ui_->group_p_values_checkbox->setChecked(false);
  update_difference_particles();
  Q_EMIT update_view();
}

//-----------------------------------------------------------------------------
void AnalysisTool::on_difference_button_clicked() {
  ui_->group_slider->setValue(ui_->group_slider->minimum());
  ui_->mean_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
  ui_->group_p_values_checkbox->setChecked(false);
  ui_->group_animate_checkbox->setChecked(false);
  ui_->difference_button->setChecked(true);
  update_difference_particles();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_p_values_clicked() {
  if (!ui_->group_p_values_checkbox->isChecked()) {
    Q_EMIT update_view();
    return;
  }
  ui_->group_slider->setValue(10);
  ui_->mean_button->setChecked(false);
  ui_->group1_button->setChecked(false);
  ui_->group2_button->setChecked(false);
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

//---------------------------------------------------------------------------
void AnalysisTool::network_analysis_clicked() {
  if (ui_->network_feature->currentText().isEmpty()) {
    QMessageBox::warning(this, "Network Analysis", "Project must have a scalar feature for network analysis.");
    // SW_WARN("Project must have a scalar features for network analysis");
    return;
  }
  network_analysis_job_ =
      QSharedPointer<NetworkAnalysisJob>::create(session_->get_project(), ui_->group_combo->currentText().toStdString(),
                                                 ui_->network_feature->currentText().toStdString());
  network_analysis_job_->set_pvalue_of_interest(ui_->network_pvalue_of_interest->text().toDouble());
  network_analysis_job_->set_pvalue_threshold(ui_->network_pvalue_threshold->text().toDouble());
  network_analysis_job_->set_num_iterations(ui_->network_iterations->text().toInt());

  connect(network_analysis_job_.data(), &NetworkAnalysisJob::progress, this,
          &AnalysisTool::handle_network_analysis_progress);
  connect(network_analysis_job_.data(), &NetworkAnalysisJob::finished, this,
          &AnalysisTool::handle_network_analysis_complete);
  app_->get_py_worker()->run_job(network_analysis_job_);
}

//-----------------------------------------------------------------------------
bool AnalysisTool::compute_stats() {
  if (stats_ready_) {
    return true;
  }

  if (session_->is_loading()) {
    return false;
  }

  if (session_->get_non_excluded_shapes().size() == 0 || !session_->particles_present()) {
    return false;
  }

  SW_DEBUG("Compute Stats!");
  compute_reconstructed_domain_transforms();

  ui_->pcaModeSpinBox->setMaximum(std::max<double>(1, session_->get_non_excluded_shapes().size() - 1));

  std::vector<Eigen::VectorXd> points;
  std::vector<int> group_ids;

  std::string group_set = ui_->group_combo->currentText().toStdString();
  std::string left_group = ui_->group_left->currentText().toStdString();
  std::string right_group = ui_->group_right->currentText().toStdString();

  auto pca_groups = get_checked_pca_groups();

  group1_list_.clear();
  group2_list_.clear();

  auto domain_names = session_->get_project()->get_domain_names();
  unsigned int domains_per_shape = domain_names.size();
  number_of_particles_array_.resize(domains_per_shape);

  for (auto& shape : session_->get_non_excluded_shapes()) {
    Eigen::VectorXd particles;
    stats_.set_num_values_per_particle(3);
    if (pca_shape_only_mode()) {
      particles = shape->get_global_correspondence_points();
    } else if (pca_scalar_only_mode()) {
      stats_.set_num_values_per_particle(1);
      shape->get_reconstructed_meshes(true);
      std::string target_feature = ui_->pca_scalar_combo->currentText().toStdString();
      shape->load_feature(DisplayMode::Reconstructed, target_feature);
      particles = shape->get_point_features(ui_->pca_scalar_combo->currentText().toStdString());
    } else {  // shape and scalar (x,y,z, and scalar)
      stats_.set_num_values_per_particle(4);
      std::string target_feature = ui_->pca_scalar_combo->currentText().toStdString();
      auto positions = shape->get_global_correspondence_points();
      shape->get_reconstructed_meshes(true);
      shape->load_feature(DisplayMode::Reconstructed, target_feature);
      auto scalars = particles = shape->get_point_features(target_feature);
      // combine positions and scalars, interleave 3 positions for every scalar
      particles.resize(positions.size() + scalars.size());
      int num_particles = positions.size() / 3;
      for (int i = 0; i < num_particles; i++) {
        particles[i * 4] = positions[i * 3];
        particles[i * 4 + 1] = positions[i * 3 + 1];
        particles[i * 4 + 2] = positions[i * 3 + 2];
      }
      for (int i = 0; i < scalars.size(); i++) {
        particles[i * 4 + 3] = scalars[i];
      }
    }

    if (particles.size() == 0) {
      continue;  // skip any that don't have particles
    }

    if (groups_active()) {
      auto group = shape->get_subject()->get_group_value(group_set);
      if (group == left_group) {
        points.push_back(particles);
        group_ids.push_back(1);
        group1_list_.push_back(shape);
      } else if (group == right_group) {
        points.push_back(particles);
        group_ids.push_back(2);
        group2_list_.push_back(shape);
      } else {
        // we don't include it
      }
    } else if (pca_groups_active()) {
      auto group = shape->get_subject()->get_group_value(ui_->pca_group_combo->currentText().toStdString());
      // see if it is in the list of groups
      bool found = false;
      for (auto&& pca_group : pca_groups) {
        if (group == pca_group.toStdString()) {
          found = true;
          break;
        }
      }
      if (found) {
        points.push_back(particles);
        group_ids.push_back(1);
      } else {
        // excluded
      }
    } else {
      points.push_back(particles);
      group_ids.push_back(1);
    }

    auto local_particles = shape->get_particles().get_local_particles();
    if (local_particles.size() != domains_per_shape) {
      SW_ERROR("Inconsistency in number of particles size");
      return false;
    }
    for (unsigned int i = 0; i < domains_per_shape; i++) {
      number_of_particles_array_[i] = local_particles[i].size() / 3;
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

  stats_.import_points(points, group_ids);
  // MCA needs to know number of particles per domain/object
  stats_.set_num_particles_per_domain(number_of_particles_array_);
  if (domains_per_shape > 1) {
    stats_.compute_multi_level_analysis_statistics(points, domains_per_shape);
  }
  stats_.compute_modes();
  if (domains_per_shape > 1) {
    stats_.compute_relative_pose_modes_for_mca();
    stats_.compute_shape_dev_modes_for_mca();
  }
  update_difference_particles();
  if (ui_->metrics_open_button->isChecked()) {
    compute_shape_evaluations();
  }

  stats_ready_ = true;

  ///  Set this to true to export long format sample data (e.g. for import into R)
  const bool export_long_format = false;

  if (export_long_format && groups_active()) {
    auto feature_names = session_->get_project()->get_feature_names();
    std::ofstream file;
    file.open("/tmp/stats.csv");
    file << "subject,group,particle,x,y,z";
    for (int i = 0; i < feature_names.size(); i++) {
      file << "," << feature_names[i];
    }
    file << "\n";

    int shape_id = 0;
    Q_FOREACH (ShapeHandle shape, session_->get_non_excluded_shapes()) {
      auto group = shape->get_subject()->get_group_value(group_set);
      auto particles = shape->get_particles();
      auto points = particles.get_world_points(0);
      int point_id = 0;
      for (int j = 0; j < points.size(); j++) {
        auto point = points[j];
        file << shape_id << "," << group << "," << point_id++ << "," << point[0] << "," << point[1] << "," << point[2];

        for (int i = 0; i < feature_names.size(); i++) {
          auto scalars = shape->get_point_features(feature_names[i]);
          file << "," << scalars[j];
        }
        file << "\n";
      }

      shape_id++;
    }
    file.close();
  }

  return true;
}

//-----------------------------------------------------------------------------
Particles AnalysisTool::get_mean_shape_points() {
  if (!compute_stats()) {
    return Particles();
  }

  if (ui_->pca_scalar_only->isChecked()) {
    return convert_from_combined(construct_mean_shape());
  }

  if (ui_->group1_button->isChecked() || ui_->difference_button->isChecked()) {
    return convert_from_combined(stats_.get_group1_mean());
  } else if (ui_->group2_button->isChecked()) {
    return convert_from_combined(stats_.get_group2_mean());
  } else if (ui_->mean_button->isChecked()) {
    return convert_from_combined(stats_.get_mean());
  }

  if (groups_active()) {
    auto group_ratio = get_group_ratio();
    temp_shape_ = stats_.get_group1_mean() + (stats_.get_group_difference() * group_ratio);
    return convert_from_combined(temp_shape_);
  }

  return convert_from_combined(stats_.get_mean());
}

//-----------------------------------------------------------------------------
Particles AnalysisTool::get_shape_points(int mode, double value) {
  if (!compute_stats() || stats_.get_eigen_vectors().size() <= 1) {
    return Particles();
  }
  if (mode + 2 > stats_.get_eigen_values().size()) {
    mode = stats_.get_eigen_values().size() - 2;
  }

  unsigned int m = stats_.get_eigen_vectors().cols() - (mode + 1);
  m = std::clamp<unsigned int>(m, 0, stats_.get_eigen_vectors().cols() - 1);

  Eigen::VectorXd e = stats_.get_eigen_vectors().col(m);

  double lambda = sqrt(stats_.get_eigen_values()[m]);

  pca_labels_changed(QString::number(value, 'g', 2), QString::number(stats_.get_eigen_values()[m]),
                     QString::number(value * lambda));

  std::vector<double> vals;
  for (int i = stats_.get_eigen_values().size() - 1; i > 0; i--) {
    vals.push_back(stats_.get_eigen_values()[i]);
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

  temp_shape_ = stats_.get_mean() + (e * (value * lambda));

  auto positions = temp_shape_;

  computed_scalars_ = Eigen::VectorXd();
  if (pca_shape_plus_scalar_mode()) {
    positions = extract_shape_only(temp_shape_);
    computed_scalars_ = extract_scalar_only(temp_shape_);
  } else if (pca_scalar_only_mode()) {
    computed_scalars_ = temp_shape_;
    if (ui_->pca_predict_shape->isChecked()) {
      positions = ShapeScalarJob::predict_shape(session_, QString::fromStdString(feature_map_), computed_scalars_);
    } else {
      positions = construct_mean_shape();
    }
  }

  return convert_from_combined(positions);
}

//---------------------------------------------------------------------------
Particles AnalysisTool::get_multi_level_shape_points(int mode, double value, McaMode level) {
  // Get Shape Points for Multi-Level Analysis
  Eigen::MatrixXd eigenvectors;
  std::vector<double> eigenvalues;
  if (level == McaMode::Within) {
    eigenvectors = stats_.get_eigenvectors_shape_dev();
    eigenvalues = stats_.get_eigenvalues_shape_dev();
  } else if (level == McaMode::Between) {
    eigenvectors = stats_.get_eigenvectors_rel_pos();
    eigenvalues = stats_.get_eigenvalues_rel_pose();
  }
  if (!compute_stats() || eigenvectors.size() <= 1) {
    return Particles();
  }
  if (mode + 2 > eigenvalues.size()) {
    mode = eigenvalues.size() - 2;
  }
  unsigned int m = eigenvectors.cols() - (mode + 1);
  Eigen::VectorXd e = eigenvectors.col(m);
  double lambda = sqrt(eigenvalues[m]);
  pca_labels_changed(QString::number(value, 'g', 2), QString::number(eigenvalues[m]), QString::number(value * lambda));
  std::vector<double> vals;
  for (int i = eigenvalues.size() - 1; i > 0; i--) {
    vals.push_back(eigenvalues[i]);
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

  unsigned int D = stats_.get_domains_per_shape();
  unsigned int sz = stats_.get_mean().size();
  if (level == McaMode::Within) {
    // Morphological Variations
    temp_shape_mca = stats_.get_mean() + (e * (value * lambda));
  } else if (level == McaMode::Between) {
    // Relative Pose Variations
    Eigen::VectorXd e_between;
    e_between.resize(sz);
    for (unsigned int i = 0; i < D; i++) {
      int num_points = number_of_particles_array_[i];
      int row = 0;
      for (int idx = 0; idx < i; idx++) {
        row += (3 * number_of_particles_array_[idx]);
      }
      for (unsigned int j = 0; j < num_points; j++) {
        e_between(row + (j * 3)) = e(i * 3);
        e_between(row + (j * 3) + 1) = e(i * 3 + 1);
        e_between(row + (j * 3) + 2) = e(i * 3 + 2);
      }
    }
    temp_shape_mca = stats_.get_mean() + (e_between * (value * lambda));
  }
  return convert_from_combined(temp_shape_mca);
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mode_shape(int mode, double value) {
  return create_shape_from_points(get_shape_points(mode, value));
}
//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mca_mode_shape(int mode, double value, McaMode level) {
  return create_shape_from_points(get_multi_level_shape_points(mode, value, level));
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_current_shape() {
  int pca_mode = get_pca_mode();
  double pca_value = get_pca_value();
  auto mca_level = get_mca_level();
  if (mca_level == AnalysisTool::McaMode::Vanilla) {
    return get_mode_shape(pca_mode, pca_value);
  } else {
    return get_mca_mode_shape(pca_mode, pca_value, mca_level);
  }
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
  ui_->network_iterations->setText(QString::number(static_cast<double>(params.get("network_iterations", 10000))));
  double pvalue_of_interest = params.get("network_pvalue_of_interest", 0.05);
  ui_->network_pvalue_of_interest->setText(QString::number(pvalue_of_interest));
  ui_->network_pvalue_threshold->setText(
      QString::number(static_cast<double>(params.get("network_pvalue_threshold", 0.05))));

  ui_->mesh_warp_sample_spinbox->setValue(params.get(MESH_WARP_TEMPLATE_INDEX, -1));
  update_group_boxes();

  ui_->group_combo->setCurrentText(QString::fromStdString(params.get("current_group", "-None-")));
  ui_->pca_group_combo->setCurrentText(QString::fromStdString(params.get("current_pca_group", "-None-")));
}

//---------------------------------------------------------------------------
void AnalysisTool::store_settings() {
  Parameters params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  params.set("reconstruction_clusters", ui_->numClusters->value());
  params.set("reconstruction_decimation", ui_->meshDecimation->value());
  params.set("reconstruction_max_angle", ui_->maxAngle->value());
  params.set("network_iterations", ui_->network_iterations->text().toStdString());
  params.set("network_pvalue_of_interest", ui_->network_pvalue_of_interest->text().toStdString());
  params.set("network_pvalue_threshold", ui_->network_pvalue_threshold->text().toStdString());

  session_->get_project()->set_parameters(Parameters::ANALYSIS_PARAMS, params);
}

//---------------------------------------------------------------------------
void AnalysisTool::shutdown() {
  pca_animate_timer_.stop();
  group_animate_timer_.stop();

  for (const auto& worker : workers_) {
    if (worker) {
      worker->stop();
    }
  }
}

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

  if ((stats_.matrix().rows() > 10000 || stats_.matrix().cols() > 10000) && !large_particle_disclaimer_waived_) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Are you sure?",
                                  "Shape evaluation on large numbers of particles or samples may exhaust "
                                  "system memory and cause ShapeWorksStudio to become unresponsive, "
                                  "are you sure you want to continue?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      skip_evals_ = true;
    }
    large_particle_disclaimer_waived_ = true;
  }

  std::vector<ShapeEvaluationJob::JobType> job_types = {ShapeEvaluationJob::JobType::CompactnessType,
                                                        ShapeEvaluationJob::JobType::GeneralizationType,
                                                        ShapeEvaluationJob::JobType::SpecificityType};

  if (skip_evals_) {
    // compactness isn't ever a problem.
    job_types = {ShapeEvaluationJob::JobType::CompactnessType};
  }

  stats_.set_particle_to_surface_mode(ui_->distance_method_surface->isChecked());

  // stop any running jobs
  for (const auto& worker : workers_) {
    if (worker) {
      worker->stop();
    }
  }

  for (auto job_type : job_types) {
    switch (job_type) {
      case ShapeEvaluationJob::JobType::CompactnessType:
        SW_DEBUG("job type: compactness");
        break;
      case ShapeEvaluationJob::JobType::GeneralizationType:
        SW_DEBUG("job type: gen job");
        break;
      case ShapeEvaluationJob::JobType::SpecificityType:
        SW_DEBUG("job type: spec job");
        break;
      default:
        SW_DEBUG("job type: unknown");
    }

    QPointer<Worker> worker = Worker::create_worker();
    workers_.push_back(worker);
    auto job = QSharedPointer<ShapeEvaluationJob>::create(job_type, stats_, session_);
    connect(job.data(), &ShapeEvaluationJob::result_ready, this, &AnalysisTool::handle_eval_thread_complete);
    connect(job.data(), &ShapeEvaluationJob::report_progress, this, &AnalysisTool::handle_eval_thread_progress);
    worker->run_job(job);
  }

  evals_ready_ = true;
}

//---------------------------------------------------------------------------
AnalysisTool::GroupAnalysisType AnalysisTool::get_group_analysis_type() {
  if (!groups_on()) {
    return GroupAnalysisType::None;
  } else {
    return static_cast<GroupAnalysisType>(ui_->group_analysis_combo->currentIndex());
  }
}

//---------------------------------------------------------------------------
bool AnalysisTool::pca_scalar_only_mode() { return ui_->pca_scalar_only->isChecked(); }

//---------------------------------------------------------------------------
bool AnalysisTool::pca_shape_plus_scalar_mode() { return ui_->pca_shape_and_scalar->isChecked(); }

//---------------------------------------------------------------------------
bool AnalysisTool::pca_shape_only_mode() { return ui_->pca_scalar_shape_only->isChecked(); }

//---------------------------------------------------------------------------
void AnalysisTool::on_tabWidget_currentChanged() { handle_analysis_options(); }

//---------------------------------------------------------------------------
void AnalysisTool::on_pcaSlider_valueChanged() {
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();
  Q_EMIT pca_update();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_group_slider_valueChanged() {
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

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

  QApplication::processEvents();
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
AnalysisTool::McaMode AnalysisTool::get_mca_level() const {
  bool vanilla_pca = ui_->vanillaPCAButton->isChecked();
  bool between = ui_->mcaLevelBetweenButton->isChecked();
  bool within = ui_->mcaLevelWithinButton->isChecked();
  if (vanilla_pca) {
    return McaMode::Vanilla;
  } else if (within) {
    return McaMode::Within;
  } else if (between) {
    return McaMode::Between;
  }
  return McaMode::Vanilla;
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
  // MLCA
  auto domain_names = session_->get_project()->get_domain_names();
  bool multiple_domains = domain_names.size() > 1;
  if (multiple_domains) {
    ui_->vanillaPCAButton->setChecked(true);
    ui_->mcaLevelWithinButton->setEnabled(false);
    ui_->mcaLevelWithinButton->setEnabled(false);
    ui_->mcaLevelBetweenButton->setEnabled(false);
  }

  ui_->network_progress_widget->hide();
  ui_->group_analysis_combo->setCurrentIndex(0);
  group_analysis_combo_changed();

  particle_area_panel_->reset();
  shape_scalar_panel_->reset();
  stats_ = ParticleShapeStatistics();
  evals_ready_ = false;
  stats_ready_ = false;

  ui_->pca_scalar_combo->clear();
  if (session_) {
    for (const auto& feature : session_->get_project()->get_feature_names()) {
      ui_->pca_scalar_combo->addItem(QString::fromStdString(feature));
    }
  }
  bool has_scalars = ui_->pca_scalar_combo->count() > 0;
  if (!has_scalars) {
    ui_->show_difference_to_predicted_scalar->setChecked(false);
  }
  ui_->show_difference_to_predicted_scalar->setEnabled(has_scalars);
  ui_->show_predicted_scalar->setEnabled(has_scalars);

  ui_->shape_scalar_groupbox->setVisible(ui_->pca_scalar_combo->count() > 0);
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

  if (session_->particles_present()) {
    auto domain_types = session_->get_groomed_domain_types();
    bool image_domain = domain_types.size() > 0 && domain_types[0] == DomainType::Image;
    ui_->distance_transform_radio_button->setEnabled(session_->particles_present() && session_->get_groomed_present() &&
                                                     image_domain);

    ui_->mesh_warping_radio_button->setEnabled(session_->particles_present() && session_->get_groomed_present());

    if (!ui_->mesh_warping_radio_button->isEnabled()) {
      ui_->legacy_radio_button->setChecked(true);
    }
    reconstruction_method_changed();
  }

  bool network_ready = network_analysis_job_ && network_analysis_job_->get_tvalues().rows() > 0;

  ui_->network_analysis_option->setEnabled(network_ready);
  ui_->network_spm1d_option->setEnabled(network_ready);
  ui_->network_analysis_display->setEnabled(network_ready);

  update_group_boxes();
  ui_->sampleSpinBox->setMaximum(session_->get_num_shapes() - 1);
  ui_->mesh_warp_sample_spinbox->setMaximum(session_->get_num_shapes() - 1);
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
  } else if (mode == "mca") {
    ui_->tabWidget->setCurrentWidget(ui_->pca_tab);
    ui_->vanillaPCAButton->setChecked(true);
    ui_->mcaLevelWithinButton->setChecked(false);
    ui_->mcaLevelBetweenButton->setChecked(false);
  }
}

//---------------------------------------------------------------------------
ShapeHandle AnalysisTool::get_mean_shape() {
  auto shape_points = get_mean_shape_points();
  ShapeHandle shape = create_shape_from_points(shape_points);

  if (get_group_analysis_type() == GroupAnalysisType::Pvalues) {
    if (groups_on() && ui_->group_p_values_checkbox->isChecked() && group_pvalue_job_ &&
        group_pvalue_job_->get_group_pvalues().rows() > 0) {
      shape->set_point_features("p_values", group_pvalue_job_->get_group_pvalues());
      shape->set_override_feature("p_values");
    }
  }

  if (get_group_analysis_type() == GroupAnalysisType::NetworkAnalysis) {
    if (groups_on() && ui_->network_analysis_display->isChecked() && network_analysis_job_ &&
        network_analysis_job_->get_tvalues().rows() > 0) {
      if (ui_->network_analysis_option->isChecked()) {
        shape->set_point_features("t_values", network_analysis_job_->get_tvalues());
        shape->set_override_feature("t_values");
      } else {
        shape->set_point_features("spm_values", network_analysis_job_->get_spm_values());
        shape->set_override_feature("spm_values");
      }
    }
  }

  if (particle_area_panel_->get_display_particle_area()) {
    shape->set_point_features(particle_area_panel_->get_computed_value_name(),
                              particle_area_panel_->get_computed_values());
    shape->set_override_feature(particle_area_panel_->get_computed_value_name());
  }

  int num_points = shape_points.get_combined_global_particles().size() / 3;
  std::vector<Eigen::VectorXd> values;

  if (feature_map_ != "") {
    auto shapes = session_->get_non_excluded_shapes();
    Eigen::VectorXd sum(num_points);
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
      Eigen::VectorXd mean = sum / values.size();

      if (ready) {
        shape->set_point_features(feature_map_, mean);
      }
    } else {
      Eigen::VectorXd sum_left(num_points);
      sum_left.setZero();
      Eigen::VectorXd sum_right(num_points);
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
      Eigen::VectorXd left_mean = sum_left / static_cast<double>(group1_list_.size());

      Q_FOREACH (auto shape, group2_list_) {
        shape->load_feature(DisplayMode::Reconstructed, feature_map_);
        auto value = shape->get_point_features(feature_map_);
        if (value.rows() != sum.rows()) {
          ready = false;
        } else {
          sum_right = sum_right + value;
        }
      }
      Eigen::VectorXd right_mean = sum_right / static_cast<double>(group2_list_.size());

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

  if (feature_map_ != "") {
    if (ui_->pca_scalar_shape_only->isChecked() && ui_->pca_predict_scalar->isChecked()) {
      auto scalars = ShapeScalarJob::predict_scalars(session_, QString::fromStdString(feature_map_),
                                                     points.get_combined_global_particles());
      shape->set_point_features(feature_map_, scalars);
    } else {
      shape->set_point_features(feature_map_, computed_scalars_);
    }
  }
  return shape;
}

//---------------------------------------------------------------------------
void AnalysisTool::set_feature_map(const std::string& feature_map) { feature_map_ = feature_map; }

//---------------------------------------------------------------------------
std::string AnalysisTool::get_display_feature_map() {
  if (session_->get_show_difference_vectors()) {
    return "surface_difference";
  }

  if (get_group_analysis_type() == GroupAnalysisType::Pvalues) {
    if (ui_->group_p_values_checkbox->isChecked() && group_pvalue_job_ &&
        group_pvalue_job_->get_group_pvalues().rows() > 0) {
      return "p_values";
    }
  }

  if (get_group_analysis_type() == GroupAnalysisType::NetworkAnalysis) {
    if (ui_->network_analysis_display->isChecked() && network_analysis_job_ &&
        network_analysis_job_->get_tvalues().rows() > 0) {
      if (ui_->network_analysis_option->isChecked()) {
        return "t_values";
      } else {
        return "spm_values";
      }
    }
  }

  if (particle_area_panel_->get_display_particle_area()) {
    if (get_analysis_mode() == AnalysisTool::MODE_ALL_SAMPLES_C ||
        get_analysis_mode() == AnalysisTool::MODE_SINGLE_SAMPLE_C) {
      return "particle_area";
    } else {
      return particle_area_panel_->get_computed_value_name();
    }
  }

  if (get_analysis_mode() == AnalysisTool::MODE_ALL_SAMPLES_C &&
      ui_->show_difference_to_predicted_scalar->isChecked()) {
    return "predicted_scalar_diff";
  }

  if (get_analysis_mode() == AnalysisTool::MODE_ALL_SAMPLES_C && ui_->show_predicted_scalar->isChecked()) {
    return "predicted_scalar";
  }

  return feature_map_;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_boxes() {
  // populate the group sets
  auto group_names = session_->get_project()->get_group_names();

  ui_->group_widget->setEnabled(!group_names.empty());
  ui_->pca_group_box->setVisible(!group_names.empty());

  if (group_names != current_group_names_) {  // only update if different
    ui_->group_combo->clear();
    ui_->pca_group_combo->clear();
    ui_->group_combo->addItem("-None-");
    ui_->pca_group_combo->addItem("-None-");
    for (const std::string& group : group_names) {
      ui_->group_combo->addItem(QString::fromStdString(group));
      ui_->pca_group_combo->addItem(QString::fromStdString(group));
    }

    current_group_names_ = group_names;
    group_changed();
  }
  update_pca_group_options();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_group_values() {
  block_group_change_ = true;
  stats_ready_ = false;

  auto values = session_->get_project()->get_group_values(ui_->group_combo->currentText().toStdString());

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

  ui_->group1_button->setEnabled(groups_on());
  ui_->group2_button->setEnabled(groups_on());
  ui_->difference_button->setEnabled(groups_on());
  // ui_->group_p_values_box->setEnabled(groups_on());
  ui_->group_slider->setEnabled(groups_on());
  ui_->group_left->setEnabled(groups_on());
  ui_->group_right->setEnabled(groups_on());
  ui_->group_animate_checkbox->setEnabled(groups_on());
  ui_->group_analysis_combo->setEnabled(groups_on());
  group_analysis_combo_changed();
  // ui_->network_analysis_box->setEnabled(groups_on());

  if (!groups_on()) {
    ui_->group_p_values_checkbox->setChecked(false);
  }

  block_group_change_ = false;
  group_changed();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_domain_alignment_box() {
  auto domain_names = session_->get_project()->get_domain_names();

  bool multiple_domains = domain_names.size() > 1;
  ui_->reference_domain_widget->setVisible(multiple_domains);
  ui_->mca_groupBox->setVisible(multiple_domains);
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
void AnalysisTool::update_pca_group_options() {
  auto values = session_->get_project()->get_group_values(ui_->pca_group_combo->currentText().toStdString());

  if (values != current_pca_group_values_) {
    // populate group values
    ui_->pca_group_list->clear();
    for (const std::string& value : values) {
      QString item = QString::fromStdString(value);
      // add checkable item
      ui_->pca_group_list->addItem(item);
      auto item_widget = ui_->pca_group_list->item(ui_->pca_group_list->count() - 1);
      item_widget->setFlags(item_widget->flags() | Qt::ItemIsUserCheckable);
      item_widget->setCheckState(Qt::Checked);
    }
    stats_ready_ = false;
    compute_stats();
  }

  int count = ui_->pca_group_list->count();
  // clamp the items to 2-10
  count = std::max(2, std::min(count, 10));

  // resize the list widget to fit the items
  ui_->pca_group_list->setMinimumHeight(ui_->pca_group_list->sizeHintForRow(0) * count + 2);
  ui_->pca_group_list->setMaximumHeight(ui_->pca_group_list->sizeHintForRow(0) * count + 2);

  current_pca_group_values_ = values;
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_pca_group_list_item_changed() {
  stats_ready_ = false;
  compute_stats();
  // update display
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::update_lda_graph() {
  if (groups_active()) {
    if (!lda_computed_ && !group_lda_job_running_) {
      group_lda_job_running_ = true;
      // ui_->lda_panel->show();
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
  if (!stats_ready_) {
    return;
  }

  if (session_->get_num_shapes() < 1) {
    return;
  }

  auto shapes = session_->get_non_excluded_shapes();
  if (shapes.size() < 1) {
    SW_ERROR("No shapes available for difference computation");
    return;
  }

  // start with a copy from the first shape so that the sizes of domains are already filled out
  Particles target = shapes[0]->get_particles();
  Eigen::VectorXd all_particles = target.get_combined_global_particles();
  Eigen::VectorXd mean = get_mean_shape_particles();

  if (get_group_difference_mode()) {
    mean = stats_.get_group2_mean();
  }

  if (all_particles.size() != mean.size()) {
    SW_LOG_ONLY("Inconsistent number of values in particle vector, {} vs. {}", all_particles.size(), mean.size());
    return;
  }

  for (unsigned int i = 0; i < mean.size(); i++) {
    all_particles[i] = mean[i];
  }

  target.set_combined_global_particles(all_particles);

  session_->set_show_difference_vectors(get_group_difference_mode() || ui_->show_difference_to_mean->isChecked());

  session_->set_difference_particles(target);
}

//---------------------------------------------------------------------------
Eigen::VectorXd AnalysisTool::get_mean_shape_particles() {
  if (pca_shape_plus_scalar_mode()) {
    return extract_shape_only(stats_.get_mean());
  } else if (pca_scalar_only_mode()) {
    return construct_mean_shape();
  }
  return stats_.get_mean();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_changed() {
  if (block_group_change_) {
    return;
  }
  stats_ready_ = false;
  group_pvalue_job_ = nullptr;
  lda_computed_ = false;
  compute_stats();
}

//---------------------------------------------------------------------------
bool AnalysisTool::groups_active() {
  // check that the group tab is active
  if (ui_->tabWidget->currentWidget() != ui_->mean_tab) {
    return false;
  }

  std::string group_set = ui_->group_combo->currentText().toStdString();
  bool groups_enabled = group_set != "" && group_set != "-None-";
  return groups_enabled;
}

//---------------------------------------------------------------------------
bool AnalysisTool::pca_groups_active() {
  if (ui_->tabWidget->currentWidget() != ui_->pca_tab) {
    return false;
  }

  std::string group_set = ui_->pca_group_combo->currentText().toStdString();
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
    compute_shape_evaluations();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::initialize_mesh_warper() {
  if (session_->particles_present() && session_->get_groomed_present()) {
    if (!compute_stats()) {
      return;
    }

    auto params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);

    int template_shape = params.get(MESH_WARP_TEMPLATE_INDEX, -1);
    if (template_shape < 0 || template_shape >= session_->get_num_shapes()) {
      template_shape = stats_.compute_median_shape(-32);  //-32 = both groups
      ui_->mesh_warp_sample_spinbox->setValue(template_shape);
    }

    auto shapes = session_->get_non_excluded_shapes();

    if (template_shape < 0 || template_shape >= shapes.size()) {
      SW_ERROR("Unable to set reference mesh, invalid template shape");
      return;
    }
    std::shared_ptr<Shape> median_shape = shapes[template_shape];

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
  SW_DEBUG("eval thread is complete");

  switch (job_type) {
    case ShapeEvaluationJob::JobType::CompactnessType:
      eval_compactness_ = data;
      ui_->compactness_progress_widget->hide();
      ui_->compactness_graph->show();
      AnalysisUtils::create_plot(ui_->compactness_graph, data, "Compactness", "Number of Modes", "Explained Variance");
      break;
    case ShapeEvaluationJob::JobType::SpecificityType:
      eval_specificity_ = data;
      ui_->specificity_progress_widget->hide();
      ui_->specificity_graph->show();
      AnalysisUtils::create_plot(ui_->specificity_graph, data, "Specificity", "Number of Modes", "Specificity");
      break;
    case ShapeEvaluationJob::JobType::GeneralizationType:
      eval_generalization_ = data;
      ui_->generalization_progress_widget->hide();
      ui_->generalization_graph->show();
      AnalysisUtils::create_plot(ui_->generalization_graph, data, "Generalization", "Number of Modes",
                                 "Generalization");
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
  new_alignment -= 2;  // minus two for local and global that come first (local == -1, global == -2)
  if (new_alignment == session_->get_current_alignment()) {
    return;
  }

  session_->set_current_alignment(static_cast<AlignmentType>(new_alignment));

  Q_FOREACH (ShapeHandle shape, session_->get_non_excluded_shapes()) {
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    if (session_->get_current_alignment() == AlignmentType::Local) {
      transform = nullptr;
    } else if (session_->get_current_alignment() == AlignmentType::Global) {
      auto domain_names = session_->get_project()->get_domain_names();
      transform = shape->get_groomed_transform(domain_names.size());
    } else {
      transform = shape->get_groomed_transform(new_alignment);
    }

    shape->set_alignment_type(new_alignment);
    shape->set_particle_transform(transform);
  }

  evals_ready_ = false;
  group_changed();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_distance_method_changed() {
  evals_ready_ = false;
  compute_shape_evaluations();
}

//---------------------------------------------------------------------------
void AnalysisTool::run_good_bad_particles() {
  auto* worker = Worker::create_worker();
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
  lda_computed_ = true;

  QString left_group = ui_->group_left->currentText();
  QString right_group = ui_->group_right->currentText();

  group_lda_job_->plot(ui_->lda_graph, left_group, right_group);
  ui_->lda_graph->setVisible(true);
  ui_->lda_hint_label->setVisible(true);
}

void AnalysisTool::handle_network_analysis_progress(int progress) {
  if (progress > 0) {
    ui_->network_progress->setMaximum(100);
  } else {
    ui_->network_progress->setMaximum(0);
  }

  bool show_progress = progress < 1;

  ui_->network_progress_widget->setVisible(show_progress);
  ui_->network_analysis_button->setEnabled(!show_progress);
  ui_->network_progress->setValue(progress * 100);
  ui_->network_progress->update();
}

void AnalysisTool::handle_network_analysis_complete() {
  handle_network_analysis_progress(100);
  SW_LOG("Network analysis complete");
  enable_actions(true);
  Q_EMIT progress(100);
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::show_difference_to_mean_clicked() {
  update_difference_particles();
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::group_analysis_combo_changed() {
  int index = ui_->group_analysis_combo->currentIndex();
  if (!groups_active()) {
    index = 0;
  }
  if (index == 0) {  // none
    ui_->group_analysis_stacked_widget->setVisible(false);
    ui_->group_analysis_stacked_widget->setEnabled(false);
  } else {
    ui_->group_analysis_stacked_widget->setCurrentIndex(index - 1);
    ui_->group_analysis_stacked_widget->setVisible(true);
    ui_->group_analysis_stacked_widget->setEnabled(true);
    if (ui_->group_analysis_stacked_widget->currentWidget() == ui_->lda_page) {
      update_lda_graph();
    }
  }
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::change_pca_analysis_type() {
  stats_ready_ = false;
  evals_ready_ = false;
  stats_ = ParticleShapeStatistics();
  ShapeScalarJob::clear_model();

  ui_->pca_predict_scalar->setEnabled(ui_->pca_scalar_shape_only->isChecked());
  ui_->pca_predict_shape->setEnabled(ui_->pca_scalar_only->isChecked());

  if (ui_->pca_predict_scalar->isChecked()) {
    // set the feature map to the target feature
    session_->set_feature_map(ui_->pca_scalar_combo->currentText().toStdString());
  }

  compute_stats();
  Q_EMIT pca_update();
}

//---------------------------------------------------------------------------
Eigen::VectorXd AnalysisTool::construct_mean_shape() {
  if (session_->get_non_excluded_shapes().empty()) {
    return Eigen::VectorXd();
  }

  Eigen::VectorXd sum_shape =
      Eigen::VectorXd::Zero(session_->get_non_excluded_shapes()[0]->get_global_correspondence_points().size());

  for (auto& shape : session_->get_non_excluded_shapes()) {
    Eigen::VectorXd particles = shape->get_global_correspondence_points();
    sum_shape += particles;
  }

  Eigen::VectorXd mean_shape = sum_shape / session_->get_non_excluded_shapes().size();
  return mean_shape;
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_samples_predicted_scalar_options() {
  bool show_difference_to_predicted_scalar = ui_->show_difference_to_predicted_scalar->isChecked();
  ui_->samples_plot->setVisible(show_difference_to_predicted_scalar);
  ui_->samples_table->setVisible(show_difference_to_predicted_scalar);
  ui_->show_absolute_difference->setEnabled(show_difference_to_predicted_scalar);

  if (ui_->show_difference_to_predicted_scalar->isChecked() || ui_->show_predicted_scalar->isChecked()) {
    // iterate over all samples, predict scalars, compute the difference and store as a new scalar field
    auto shapes = session_->get_non_excluded_shapes();

    int num_particles = shapes[0]->get_particles().get_total_number_of_particles();

    Eigen::VectorXd diffs(shapes.size() * num_particles);
    diffs.setZero();

    ui_->samples_table->clear();
    ui_->samples_table->setRowCount(shapes.size());
    QStringList headers = {"subject", "mean_diff", "std_dev", "max_diff", "min_diff"};
    ui_->samples_table->setColumnCount(headers.size());
    ui_->samples_table->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < shapes.size(); i++) {
      auto& shape = shapes[i];
      auto particles = shape->get_particles();
      auto target_feature = ui_->pca_scalar_combo->currentText();
      if (target_feature == "") {
        return;
      }
      auto predicted =
          ShapeScalarJob::predict_scalars(session_, target_feature, particles.get_combined_global_particles());

      if (predicted.size() == 0) {
        // This indicates that an error occurred, which should already have been reported
        return;
      }

      // load the mesh and feature
      shape->get_meshes(session_->get_display_mode(), true);
      shape->load_feature(session_->get_display_mode(), target_feature.toStdString());
      Eigen::VectorXd scalars = shape->get_point_features(target_feature.toStdString());

      if (scalars.size() != predicted.size()) {
        SW_LOG_ONLY("Inconsistent number of values in particle vector, {} vs. {}", scalars.size(), predicted.size());
        return;
      }
      // compute difference
      Eigen::VectorXd diff = predicted - scalars;
      if (ui_->show_absolute_difference->isChecked()) {
        diff = diff.array().abs();
      }
      shape->set_point_features("predicted_scalar_diff", diff);
      shape->set_point_features("predicted_scalar", predicted);

      // now iterate over point values and compute the mean, max, and std deviation
      double mean = diff.mean();
      double max = diff.maxCoeff();
      double min = diff.minCoeff();

      // place in the diffs vector
      diffs.segment(i * num_particles, num_particles) = diff;

      // Calculate the squared differences
      diff = diff.array().square();
      // Calculate the variance
      double variance = diff.sum() / (predicted.size() - 1);
      // Calculate the standard deviation
      double std_dev = sqrt(variance);

      ui_->samples_table->setItem(
          i, 0, new QTableWidgetItem(QString::fromStdString(shape->get_subject()->get_display_name())));
      ui_->samples_table->setItem(i, 1, new QTableWidgetItem(QString::number(mean)));
      ui_->samples_table->setItem(i, 2, new QTableWidgetItem(QString::number(std_dev)));
      ui_->samples_table->setItem(i, 3, new QTableWidgetItem(QString::number(max)));
      ui_->samples_table->setItem(i, 4, new QTableWidgetItem(QString::number(min)));
    }

    AnalysisUtils::create_box_plot(ui_->samples_plot, diffs, "Difference to Predicted Scalar", "Sample");
  }
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void AnalysisTool::samples_table_context_menu() {
  QMenu menu;
  QAction* action = menu.addAction("Copy to Clipboard");
  connect(action, &QAction::triggered, this, &AnalysisTool::samples_table_copy_to_clipboard);
  menu.exec(QCursor::pos());
}

//---------------------------------------------------------------------------
void AnalysisTool::samples_table_copy_to_clipboard() {
  QTableWidget* table = ui_->samples_table;
  QString text;
  // start with headers
  for (int i = 0; i < table->columnCount(); i++) {
    text += table->horizontalHeaderItem(i)->text();
    if (i < table->columnCount() - 1) {
      text += ",";
    }
  }
  text += "\n";
  for (int i = 0; i < table->rowCount(); i++) {
    for (int j = 0; j < table->columnCount(); j++) {
      auto item = table->item(i, j);
      if (item) {
        text += item->text();
      }
      if (j < table->columnCount() - 1) {
        text += ",";
      }
    }
    text += "\n";
  }
  QApplication::clipboard()->setText(text);
}

//---------------------------------------------------------------------------
void AnalysisTool::mesh_warp_median_clicked() {
  ui_->mesh_warp_sample_spinbox->setValue(stats_.compute_median_shape(-32));  //-32 = both groups
}

//---------------------------------------------------------------------------
void AnalysisTool::mesh_warp_sample_changed() {
  int index = ui_->mesh_warp_sample_spinbox->value();
  auto params = session_->get_project()->get_parameters(Parameters::ANALYSIS_PARAMS);
  params.set(MESH_WARP_TEMPLATE_INDEX, index);
  session_->get_project()->set_parameters(Parameters::ANALYSIS_PARAMS, params);
  auto shapes = session_->get_non_excluded_shapes();
  if (index < 0 || index >= shapes.size()) {
    ui_->template_mesh_name_label->setText("");
    return;
  }
  ui_->template_mesh_name_label->setText(QString::fromStdString(shapes[index]->get_subject()->get_display_name()));
}

//---------------------------------------------------------------------------
void AnalysisTool::mesh_warp_run_clicked() {
  session_->handle_clear_cache();
  initialize_mesh_warper();
  Q_EMIT reconstruction_complete();
}

//---------------------------------------------------------------------------
void AnalysisTool::handle_tab_changed() {
  stats_ready_ = false;
  compute_stats();
}

//---------------------------------------------------------------------------
void AnalysisTool::reconstruction_method_changed() {
  ui_->reconstruction_options->setVisible(ui_->distance_transform_radio_button->isChecked());
  ui_->mesh_warp_options->setVisible(ui_->mesh_warping_radio_button->isChecked());
  std::string method = MeshGenerator::RECONSTRUCTION_LEGACY_C;
  if (ui_->distance_transform_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_DISTANCE_TRANSFORM_C;
  } else if (ui_->mesh_warping_radio_button->isChecked()) {
    method = MeshGenerator::RECONSTRUCTION_MESH_WARPER_C;
  }

  auto previous_method = session_->get_mesh_manager()->get_mesh_generator()->get_reconstruction_method();
  if (previous_method != method) {
    session_->get_mesh_manager()->get_mesh_generator()->set_reconstruction_method(method);
    session_->handle_clear_cache();
    if (method == MeshGenerator::RECONSTRUCTION_MESH_WARPER_C) {
      initialize_mesh_warper();
    }
    Q_EMIT reconstruction_complete();
  }
}

//---------------------------------------------------------------------------
void AnalysisTool::set_active(bool active) {
  if (!active) {
    ui_->pcaAnimateCheckBox->setChecked(false);
    ui_->group_animate_checkbox->setChecked(false);
    pca_animate_timer_.stop();
    group_animate_timer_.stop();
  } else {
    auto features = session_->get_project()->get_feature_names();
    ui_->network_feature->clear();
    // convert to QStringList
    QStringList qfeatures;
    for (auto& feature : features) {
      qfeatures.append(QString::fromStdString(feature));
    }
    ui_->network_feature->addItems(qfeatures);
  }
  active_ = active;
  update_interface();
}

//---------------------------------------------------------------------------
bool AnalysisTool::get_active() { return active_; }

//---------------------------------------------------------------------------
Particles AnalysisTool::convert_from_combined(const Eigen::VectorXd& points) {
  Particles particles;
  if (session_->get_non_excluded_shapes().empty()) {
    return particles;
  }
  auto base = session_->get_non_excluded_shapes()[0]->get_particles();

  auto worlds = base.get_world_particles();
  int idx = 0;
  for (int d = 0; d < worlds.size(); d++) {
    Eigen::VectorXd new_world(worlds[d].size());

    if (idx + new_world.size() > points.size()) {
      SW_LOG_ONLY("Inconsistent number of values in particle vector");
      return {};
    }

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
  auto shapes = session_->get_non_excluded_shapes();
  if (session_->get_current_alignment() == AlignmentType::Local) {
    reconstruction_transforms_.resize(session_->get_domains_per_shape());
    for (int domain = 0; domain < session_->get_domains_per_shape(); domain++) {
      int num_shapes = shapes.size();
      // create a new transform
      auto transform = vtkSmartPointer<vtkTransform>::New();
      // get a pointer to the transform's data
      double* values = transform->GetMatrix()->GetData();

      for (int i = 0; i < shapes.size(); i++) {
        auto base = shapes[i]->get_alignment(0);
        auto offset = shapes[i]->get_alignment(domain);

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

}  // namespace shapeworks
