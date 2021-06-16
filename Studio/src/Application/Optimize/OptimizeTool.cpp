#include <iostream>

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QIntValidator>

#include <Optimize/OptimizeTool.h>
#include <Libs/Optimize/OptimizeParameters.h>

#include <Data/Preferences.h>
#include <Visualization/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/Shape.h>

#include <Optimize/QOptimize.h>

#include <ui_OptimizeTool.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi(this);

  connect(this->ui_->use_normals, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(this->ui_->procrustes, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(this->ui_->multiscale, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);

  this->ui_->number_of_particles->setToolTip("Number of correspondence points to generate");
  this->ui_->initial_relative_weighting->setToolTip(
    "Relative weighting of correspondence term during initialization");
  this->ui_->relative_weighting->setToolTip(
    "Relative weighting of correspondence term during optimization");
  this->ui_->starting_regularization->setToolTip(
    "Starting regularization of correspondence covariance matrix");
  this->ui_->ending_regularization->setToolTip(
    "Ending regularization of correspondence covariance matrix");
  this->ui_->iterations_per_split->setToolTip("Number of iterations for each particle split");
  this->ui_->optimization_iterations->setToolTip("Number of optimizations to run");
  this->ui_->use_geodesic_distance->setToolTip(
    "Use geodesic distances for sampling term: may be more effective for capturing thin features. "
    "Requires ~10x more time, and larger memory footprint. Only supported for mesh inputs");
  this->ui_->use_normals->setToolTip("Use surface normals as part of optimization");
  this->ui_->normals_strength->setToolTip("Strength of surface normals relative to position");
  this->ui_->procrustes->setToolTip("Use procrustes registration during optimization");
  this->ui_->procrustes_interval->setToolTip(
    "How often to run procrustes during optimization (0 = disabled)");
  this->ui_->procrustes_scaling->setToolTip("Use procrustes scaling");
  this->ui_->multiscale->setToolTip("Use multiscale optimization mode");
  this->ui_->multiscale_particles->setToolTip(
    "Start multiscale optimization after this many particles");

  QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
  QIntValidator* zero_and_up = new QIntValidator(0, std::numeric_limits<int>::max(), this);

  QDoubleValidator* double_validator = new QDoubleValidator(0, std::numeric_limits<double>::max(),
                                                            1000, this);

  ui_->number_of_particles->setValidator(above_zero);
  ui_->initial_relative_weighting->setValidator(double_validator);
  ui_->relative_weighting->setValidator(double_validator);
  ui_->starting_regularization->setValidator(double_validator);
  ui_->ending_regularization->setValidator(double_validator);
  ui_->iterations_per_split->setValidator(above_zero);
  ui_->optimization_iterations->setValidator(zero_and_up);
  ui_->normals_strength->setValidator(double_validator);
  ui_->procrustes_interval->setValidator(zero_and_up);
  ui_->multiscale_particles->setValidator(above_zero);

  line_edits_.push_back(ui_->number_of_particles);
  line_edits_.push_back(ui_->initial_relative_weighting);
  line_edits_.push_back(ui_->relative_weighting);
  line_edits_.push_back(ui_->starting_regularization);
  line_edits_.push_back(ui_->ending_regularization);
  line_edits_.push_back(ui_->iterations_per_split);
  line_edits_.push_back(ui_->optimization_iterations);
  line_edits_.push_back(ui_->normals_strength);
  line_edits_.push_back(ui_->procrustes_interval);
  line_edits_.push_back(ui_->multiscale_particles);

  for (QLineEdit* line_edit : line_edits_) {
    connect(line_edit, &QLineEdit::textChanged,
            this, &OptimizeTool::update_run_button);
  }
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error(std::string msg)
{
  emit error_message(msg);
  this->update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_warning(std::string msg)
{
  emit warning_message(msg);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val, QString progress_message)
{
  emit progress(val);
  emit status(progress_message.toStdString());

  auto particles = this->optimize_->GetParticles();
  this->session_->update_particles(particles);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete()
{
  this->optimization_is_running_ = false;

  auto particles = this->optimize_->GetParticles();
  this->session_->update_particles(particles);
  this->session_->calculate_reconstructed_samples();
  this->session_->get_project()->store_subjects();
  emit progress(100);

  std::string duration = QString::number(this->elapsed_timer_.elapsed() / 1000.0, 'f',
                                         1).toStdString();
  emit message("Optimize Complete.  Duration: " + duration + " seconds");
  emit optimize_complete();
  this->update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked()
{
  if (this->optimization_is_running_) {
    this->shutdown_threads();
    this->optimization_is_running_ = false;
    this->enable_actions();
    emit progress(100);
    emit message("Optimize Aborted");
    emit optimize_complete();
    return;
  }
  this->optimization_is_running_ = true;
  emit optimize_start();

  this->enable_actions();

  this->store_params();
  emit message("Please wait: running optimize step...");

  this->elapsed_timer_.start();
  this->optimize_ = QSharedPointer<QOptimize>::create();

  this->session_->clear_particles();

  this->handle_progress(1, "");
  this->optimize_parameters_ = QSharedPointer<OptimizeParameters>::create(
    this->session_->get_project());

  this->optimize_parameters_->set_load_callback(
    std::bind(&OptimizeTool::handle_load_progress, this, std::placeholders::_1));
  this->optimize_->SetFileOutputEnabled(false);

  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::OptimizeType, NULL, this->optimize_, this->optimize_parameters_,
    this->session_,
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<std::string>());

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_optimize_complete()));
  connect(this->optimize_.data(), &QOptimize::progress, this, &OptimizeTool::handle_progress);
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(message(std::string)), this, SLOT(handle_message(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  thread->start();

  this->threads_ << thread;
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_message(std::string s)
{
  emit message(s);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_restoreDefaults_clicked()
{
  this->session_->get_project()->clear_parameters(Parameters::OPTIMIZE_PARAMS);
  this->load_params();
}

//---------------------------------------------------------------------------
void OptimizeTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
}

//---------------------------------------------------------------------------
void OptimizeTool::load_params()
{
  this->setup_domain_boxes();
  auto params = OptimizeParameters(this->session_->get_project());

  this->ui_->number_of_particles->setText(QString::number(params.get_number_of_particles()[0]));

  auto domain_names = this->session_->get_project()->get_domain_names();
  for (int i = 0; i < domain_names.size(); i++) {
    if (i < this->particle_boxes_.size()) {

      int particles = 128;
      if (i < params.get_number_of_particles().size()) {
        particles = params.get_number_of_particles()[i];
      }

      this->particle_boxes_[i]->setText(QString::number(particles));
    }
  }

  this->ui_->initial_relative_weighting->setText(
    QString::number(params.get_initial_relative_weighting()));
  this->ui_->relative_weighting->setText(QString::number(params.get_relative_weighting()));
  this->ui_->starting_regularization->setText(
    QString::number(params.get_starting_regularization()));
  this->ui_->ending_regularization->setText(QString::number(params.get_ending_regularization()));
  this->ui_->iterations_per_split->setText(QString::number(params.get_iterations_per_split()));
  this->ui_->optimization_iterations->setText(
    QString::number(params.get_optimization_iterations()));

  this->ui_->use_geodesic_distance->setChecked(params.get_use_geodesic_distance());
  this->ui_->use_normals->setChecked(params.get_use_normals()[0]);
  this->ui_->normals_strength->setText(QString::number(params.get_normals_strength()));

  this->ui_->procrustes->setChecked(params.get_use_procrustes());
  this->ui_->procrustes_scaling->setChecked(params.get_use_procrustes_scaling());
  this->ui_->procrustes_interval->setText(QString::number(params.get_procrustes_interval()));

  this->ui_->multiscale->setChecked(params.get_use_multiscale());
  this->ui_->multiscale_particles->setText(QString::number(params.get_multiscale_particles()));

  this->update_ui_elements();

}

//---------------------------------------------------------------------------
void OptimizeTool::store_params()
{
  auto params = OptimizeParameters(this->session_->get_project());

  std::vector<int> num_particles;
  num_particles.push_back(this->ui_->number_of_particles->text().toInt());

  auto domain_names = this->session_->get_project()->get_domain_names();
  if (domain_names.size() > 1) {
    num_particles.clear();
    for (int i = 0; i < domain_names.size(); i++) {
      if (i < this->particle_boxes_.size()) {
        num_particles.push_back(this->particle_boxes_[i]->text().toInt());
      }
    }
  }

  params.set_number_of_particles(num_particles);
  params.set_initial_relative_weighting(this->ui_->initial_relative_weighting->text().toDouble());
  params.set_relative_weighting(this->ui_->relative_weighting->text().toDouble());
  params.set_starting_regularization(this->ui_->starting_regularization->text().toDouble());
  params.set_ending_regularization(this->ui_->ending_regularization->text().toDouble());
  params.set_iterations_per_split(this->ui_->iterations_per_split->text().toDouble());
  params.set_optimization_iterations(this->ui_->optimization_iterations->text().toDouble());

  params.set_use_geodesic_distance(this->ui_->use_geodesic_distance->isChecked());
  params.set_use_normals({this->ui_->use_normals->isChecked()});
  params.set_normals_strength(this->ui_->normals_strength->text().toDouble());

  params.set_use_procrustes(this->ui_->procrustes->isChecked());
  params.set_use_procrustes_scaling(this->ui_->procrustes_scaling->isChecked());
  params.set_procrustes_interval(this->ui_->procrustes_interval->text().toDouble());

  params.set_use_multiscale(this->ui_->multiscale->isChecked());
  params.set_multiscale_particles(this->ui_->multiscale_particles->text().toDouble());

  // always use preference value
  params.set_geodesic_cache_multiplier(this->preferences_.get_geodesic_cache_multiplier());
  params.set_optimize_output_prefix(this->preferences_.get_optimize_file_template().toStdString());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void OptimizeTool::enable_actions()
{
  this->update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::disable_actions()
{
  //this->ui_->run_optimize_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void OptimizeTool::shutdown_threads()
{
  std::cerr << "Shut Down Optimization Threads\n";
  if (!this->optimize_) {
    return;
  }
  if (this->optimize_parameters_) {
    this->optimize_parameters_->set_abort_load(true);
  }
  this->optimize_->AbortOptimization();

}

//---------------------------------------------------------------------------
void OptimizeTool::update_ui_elements()
{
  this->ui_->normals_strength->setEnabled(this->ui_->use_normals->isChecked());
  this->ui_->procrustes_scaling->setEnabled(this->ui_->procrustes->isChecked());
  this->ui_->procrustes_interval->setEnabled(this->ui_->procrustes->isChecked());
  this->ui_->multiscale_particles->setEnabled(this->ui_->multiscale->isChecked());
}

//---------------------------------------------------------------------------
void OptimizeTool::activate()
{
  this->enable_actions();

}

//---------------------------------------------------------------------------
void OptimizeTool::handle_load_progress(int count)
{
  double value = static_cast<double>(count) / this->session_->get_shapes().size() * 100.0f;
  if (value < 100) { // cannot emit 100 or the main interface will think the job is done
    emit progress(static_cast<int>(value));
  }
}

//---------------------------------------------------------------------------
bool OptimizeTool::validate_inputs()
{
  bool all_valid = true;

  std::vector<QLineEdit*> combined = this->line_edits_;

  combined.insert(combined.end(), this->particle_boxes_.begin(), this->particle_boxes_.end());

  for (QLineEdit* line_edit : combined) {
    QString text = line_edit->text();
    int pos;
    QString ss;
    if (QValidator::Acceptable != line_edit->validator()->validate(text, pos)) {
      ss = "QLineEdit {background-color: rgb(255,204,203);}";
      all_valid = false;
    }
    line_edit->setStyleSheet(ss);
  }
  return all_valid;
}

//---------------------------------------------------------------------------
void OptimizeTool::update_run_button()
{
  bool inputs_valid = this->validate_inputs();

  this->ui_->run_optimize_button->setEnabled(inputs_valid && this->session_->get_groomed_present());

  if (this->optimization_is_running_) {
    this->ui_->run_optimize_button->setText("Abort Optimize");
  }
  else {
    this->ui_->run_optimize_button->setText("Run Optimize");
  }

}

//---------------------------------------------------------------------------
void OptimizeTool::setup_domain_boxes()
{
  qDeleteAll(
    this->ui_->domain_widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
  this->particle_boxes_.clear();

  QLineEdit* last_box = this->ui_->number_of_particles;

  if (this->session_->get_project()->get_number_of_domains_per_subject() < 2) {
    this->ui_->particle_stack->setCurrentIndex(0);
    this->ui_->domain_widget->setMaximumSize(1, 1);
  }
  else {
    this->ui_->domain_widget->setMaximumSize(9999, 9999);
    auto domain_names = this->session_->get_project()->get_domain_names();
    QGridLayout* layout = new QGridLayout;
    QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
    for (int i = 0; i < domain_names.size(); i++) {
      auto label = new QLabel(QString::fromStdString(domain_names[i]), this);
      layout->addWidget(label, i, 0);
      QLineEdit* box = new QLineEdit(this);
      last_box = box;
      box->setAlignment(Qt::AlignHCenter);
      box->setValidator(above_zero);
      connect(box, &QLineEdit::textChanged,
              this, &OptimizeTool::update_run_button);

      this->particle_boxes_.push_back(box);
      layout->addWidget(box, i, 1);
    }

    delete this->ui_->domain_widget->layout();
    this->ui_->domain_widget->setLayout(layout);
    this->ui_->particle_stack->setCurrentIndex(1);
  }

  QWidget::setTabOrder(last_box, this->ui_->initial_relative_weighting);
  QWidget::setTabOrder(this->ui_->initial_relative_weighting, this->ui_->relative_weighting);
  QWidget::setTabOrder(this->ui_->relative_weighting, this->ui_->starting_regularization);
  QWidget::setTabOrder(this->ui_->starting_regularization, this->ui_->ending_regularization);
  QWidget::setTabOrder(this->ui_->ending_regularization, this->ui_->iterations_per_split);
  QWidget::setTabOrder(this->ui_->iterations_per_split, this->ui_->optimization_iterations);
  QWidget::setTabOrder(this->ui_->optimization_iterations, this->ui_->use_geodesic_distance);
  QWidget::setTabOrder(this->ui_->use_geodesic_distance, this->ui_->use_normals);
  QWidget::setTabOrder(this->ui_->use_normals, this->ui_->normals_strength);
  QWidget::setTabOrder(this->ui_->normals_strength, this->ui_->procrustes);
  QWidget::setTabOrder(this->ui_->procrustes, this->ui_->procrustes_scaling);
  QWidget::setTabOrder(this->ui_->procrustes_scaling, this->ui_->procrustes_interval);
  QWidget::setTabOrder(this->ui_->procrustes_interval, this->ui_->multiscale);
  QWidget::setTabOrder(this->ui_->multiscale, this->ui_->multiscale_particles);
  QWidget::setTabOrder(this->ui_->multiscale_particles, this->ui_->run_optimize_button);
  QWidget::setTabOrder(this->ui_->run_optimize_button, this->ui_->restoreDefaults);

}

