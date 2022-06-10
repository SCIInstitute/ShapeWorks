// qt
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QThread>
#include <QTimer>

// shapeworks
#include <Libs/Optimize/OptimizeParameters.h>

// studio
#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/ShapeWorksWorker.h>
#include <Interface/Style.h>
#include <Optimize/OptimizeTool.h>
#include <Optimize/QOptimize.h>
#include <ui_OptimizeTool.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_OptimizeTool;
  ui_->setupUi(this);

  connect(ui_->optimize_open_button, &QPushButton::toggled, ui_->optimize_content, &QWidget::setVisible);
  connect(ui_->optimize_header, &QPushButton::clicked, ui_->optimize_open_button, &QPushButton::toggle);

  connect(ui_->use_normals, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(ui_->procrustes, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(ui_->multiscale, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);

  ui_->number_of_particles->setToolTip("Number of correspondence points to generate");
  ui_->initial_relative_weighting->setToolTip("Relative weighting of correspondence term during initialization");
  ui_->relative_weighting->setToolTip("Relative weighting of correspondence term during optimization");
  ui_->starting_regularization->setToolTip("Starting regularization of correspondence covariance matrix");
  ui_->ending_regularization->setToolTip("Ending regularization of correspondence covariance matrix");
  ui_->iterations_per_split->setToolTip("Number of iterations for each particle split");
  ui_->optimization_iterations->setToolTip("Number of optimizations to run");
  ui_->use_geodesic_distance->setToolTip(
      "Use geodesic distances for sampling term: may be more effective for capturing thin features. "
      "Requires ~10x more time, and larger memory footprint. Only supported for mesh inputs");
  ui_->use_normals->setToolTip("Use surface normals as part of optimization");
  ui_->normals_strength->setToolTip("Strength of surface normals relative to position");
  ui_->procrustes->setToolTip("Use procrustes registration during optimization");
  ui_->procrustes_interval->setToolTip("How often to run procrustes during optimization (0 = disabled)");
  ui_->procrustes_scaling->setToolTip("Use procrustes scaling");
  ui_->procrustes_rotation_translation->setToolTip("Use procrustes rotation and translation");
  ui_->multiscale->setToolTip("Use multiscale optimization mode");
  ui_->multiscale_particles->setToolTip("Start multiscale optimization after this many particles");
  ui_->use_landmarks->setToolTip("Use landmarks as initial particles");
  ui_->narrow_band->setToolTip(
      "Narrow band around distance transforms.  "
      "This value should only be changed if an error occurs "
      "during optimization suggesting that it should be increased.  "
      "It has no effect on the optimization");

  QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
  QIntValidator* zero_and_up = new QIntValidator(0, std::numeric_limits<int>::max(), this);

  QDoubleValidator* double_validator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 1000, this);

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
  ui_->narrow_band->setValidator(double_validator);

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
  line_edits_.push_back(ui_->narrow_band);

  for (QLineEdit* line_edit : line_edits_) {
    connect(line_edit, &QLineEdit::textChanged, this, &OptimizeTool::update_run_button);
  }

  Style::apply_normal_button_style(ui_->restoreDefaults);
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool() {}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error(QString msg) {
  emit error_message(msg);
  update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_warning(QString msg) { emit warning_message(msg); }

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val, QString progress_message) {
  if (!optimization_is_running_) {
    return;
  }

  emit progress(val);
  emit status(progress_message);

  auto particles = optimize_->GetParticles();
  session_->update_particles(particles);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete() {
  optimization_is_running_ = false;

  auto particles = optimize_->GetParticles();
  session_->update_particles(particles);

  auto procrustes_transforms = optimize_->GetProcrustesTransforms();
  session_->update_procrustes_transforms(procrustes_transforms);

  session_->calculate_reconstructed_samples();
  session_->get_project()->store_subjects();
  emit progress(100);

  QString duration = QString::number(elapsed_timer_.elapsed() / 1000.0, 'f', 1);
  emit message("Optimize Complete.  Duration: " + duration + " seconds");
  emit optimize_complete();
  update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_failed() {
  optimization_is_running_ = false;
  emit progress(100);

  QString duration = QString::number(elapsed_timer_.elapsed() / 1000.0, 'f', 1);
  emit message("Optimize Failed.  Duration: " + duration + " seconds");
  emit optimize_complete();
  update_run_button();
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked() {
  // ensure someone doesn't accidentally abort right after clicking RUN
  ui_->run_optimize_button->setEnabled(false);

  if (optimization_is_running_) {
    ui_->run_optimize_button->setText("Aborting...");
    optimization_is_running_ = false;
    shutdown_threads();
    emit message("Aborted Optimize");
    return;
  }

  if (session_->get_filename() == "") {
    emit error_message("Project must be saved before running Optimize");
    ui_->run_optimize_button->setEnabled(true);
    return;
  } else {
    session_->save_project(session_->get_filename());
  }

  optimization_is_running_ = true;
  emit optimize_start();

  store_params();
  emit message("Please wait: running optimize step...");

  elapsed_timer_.start();
  optimize_ = QSharedPointer<QOptimize>::create();

  session_->clear_particles();

  handle_progress(1, "");
  optimize_parameters_ = QSharedPointer<OptimizeParameters>::create(session_->get_project());

  optimize_parameters_->set_load_callback(std::bind(&OptimizeTool::handle_load_progress, this, std::placeholders::_1));
  optimize_->SetFileOutputEnabled(false);

  ShapeworksWorker* worker =
      new ShapeworksWorker(ShapeworksWorker::OptimizeType, NULL, optimize_, optimize_parameters_, session_,
                           std::vector<std::vector<itk::Point<double>>>(),
                           std::vector<std::vector<itk::Point<double>>>(), std::vector<std::string>());

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_optimize_complete()));
  connect(worker, &ShapeworksWorker::failure, this, &OptimizeTool::handle_optimize_failed);
  connect(optimize_.data(), &QOptimize::progress, this, &OptimizeTool::handle_progress);
  connect(worker, &ShapeworksWorker::error_message, this, &OptimizeTool::handle_error);
  connect(worker, &ShapeworksWorker::message, this, &OptimizeTool::handle_message);
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  thread->start();

  threads_ << thread;

  // re-enable after 2 seconds
  QTimer::singleShot(1000, this, [&]() { update_run_button(); });
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_message(QString s) { emit message(s); }

//---------------------------------------------------------------------------
void OptimizeTool::on_restoreDefaults_clicked() {
  session_->get_project()->clear_parameters(Parameters::OPTIMIZE_PARAMS);
  load_params();
}

//---------------------------------------------------------------------------
void OptimizeTool::set_session(QSharedPointer<Session> session) { session_ = session; }

//---------------------------------------------------------------------------
void OptimizeTool::load_params() {
  setup_domain_boxes();
  auto params = OptimizeParameters(session_->get_project());

  ui_->number_of_particles->setText(QString::number(params.get_number_of_particles()[0]));

  auto domain_names = session_->get_project()->get_domain_names();
  for (int i = 0; i < domain_names.size(); i++) {
    if (i < particle_boxes_.size()) {
      int particles = 128;
      if (i < params.get_number_of_particles().size()) {
        particles = params.get_number_of_particles()[i];
      }

      particle_boxes_[i]->setText(QString::number(particles));
    }
  }

  ui_->initial_relative_weighting->setText(QString::number(params.get_initial_relative_weighting()));
  ui_->relative_weighting->setText(QString::number(params.get_relative_weighting()));
  ui_->starting_regularization->setText(QString::number(params.get_starting_regularization()));
  ui_->ending_regularization->setText(QString::number(params.get_ending_regularization()));
  ui_->iterations_per_split->setText(QString::number(params.get_iterations_per_split()));
  ui_->optimization_iterations->setText(QString::number(params.get_optimization_iterations()));

  ui_->use_geodesic_distance->setChecked(params.get_use_geodesic_distance());
  ui_->use_normals->setChecked(params.get_use_normals()[0]);
  ui_->normals_strength->setText(QString::number(params.get_normals_strength()));

  ui_->procrustes->setChecked(params.get_use_procrustes());
  ui_->procrustes_scaling->setChecked(params.get_use_procrustes_scaling());
  ui_->procrustes_rotation_translation->setChecked(params.get_use_procrustes_rotation_translation());
  ui_->procrustes_interval->setText(QString::number(params.get_procrustes_interval()));

  ui_->multiscale->setChecked(params.get_use_multiscale());
  ui_->multiscale_particles->setText(QString::number(params.get_multiscale_particles()));
  ui_->use_landmarks->setChecked(params.get_use_landmarks());
  ui_->narrow_band->setText(QString::number(params.get_narrow_band()));

  update_ui_elements();
}

//---------------------------------------------------------------------------
void OptimizeTool::store_params() {
  auto params = OptimizeParameters(session_->get_project());

  std::vector<int> num_particles;
  num_particles.push_back(ui_->number_of_particles->text().toInt());

  auto domain_names = session_->get_project()->get_domain_names();
  if (domain_names.size() > 1) {
    num_particles.clear();
    for (int i = 0; i < domain_names.size(); i++) {
      if (i < particle_boxes_.size()) {
        num_particles.push_back(particle_boxes_[i]->text().toInt());
      }
    }
  }

  params.set_number_of_particles(num_particles);
  params.set_initial_relative_weighting(ui_->initial_relative_weighting->text().toDouble());
  params.set_relative_weighting(ui_->relative_weighting->text().toDouble());
  params.set_starting_regularization(ui_->starting_regularization->text().toDouble());
  params.set_ending_regularization(ui_->ending_regularization->text().toDouble());
  params.set_iterations_per_split(ui_->iterations_per_split->text().toDouble());
  params.set_optimization_iterations(ui_->optimization_iterations->text().toDouble());

  params.set_use_geodesic_distance(ui_->use_geodesic_distance->isChecked());
  params.set_use_normals({ui_->use_normals->isChecked()});
  params.set_normals_strength(ui_->normals_strength->text().toDouble());

  params.set_use_procrustes(ui_->procrustes->isChecked());
  params.set_use_procrustes_scaling(ui_->procrustes_scaling->isChecked());
  params.set_use_procrustes_rotation_translation(ui_->procrustes_rotation_translation->isChecked());
  params.set_procrustes_interval(ui_->procrustes_interval->text().toDouble());

  params.set_use_multiscale(ui_->multiscale->isChecked());
  params.set_multiscale_particles(ui_->multiscale_particles->text().toDouble());
  params.set_use_landmarks(ui_->use_landmarks->isChecked());

  // always use preference value
  params.set_geodesic_cache_multiplier(preferences_.get_geodesic_cache_multiplier());
  params.set_optimize_output_prefix(preferences_.get_optimize_file_template().toStdString());

  params.set_narrow_band(ui_->narrow_band->text().toDouble());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void OptimizeTool::enable_actions() { update_run_button(); }

//---------------------------------------------------------------------------
void OptimizeTool::disable_actions() {
  // ui_->run_optimize_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void OptimizeTool::shutdown_threads() {
  std::cerr << "Shut Down Optimization Threads\n";
  if (!optimize_) {
    return;
  }
  if (optimize_parameters_) {
    optimize_parameters_->set_abort_load(true);
  }
  optimize_->AbortOptimization();
}

//---------------------------------------------------------------------------
void OptimizeTool::update_ui_elements() {
  ui_->normals_strength->setEnabled(ui_->use_normals->isChecked());
  ui_->procrustes_scaling->setEnabled(ui_->procrustes->isChecked());
  ui_->procrustes_rotation_translation->setEnabled(ui_->procrustes->isChecked());
  ui_->procrustes_interval->setEnabled(ui_->procrustes->isChecked());
  ui_->multiscale_particles->setEnabled(ui_->multiscale->isChecked());
}

//---------------------------------------------------------------------------
void OptimizeTool::activate() { enable_actions(); }

//---------------------------------------------------------------------------
void OptimizeTool::handle_load_progress(int count) {
  if (!optimization_is_running_) {
    return;
  }

  double value = static_cast<double>(count) / session_->get_shapes().size() * 100.0f;
  if (value < 100) {  // cannot emit 100 or the main interface will think the job is done
    emit progress(static_cast<int>(value));
  }
}

//---------------------------------------------------------------------------
bool OptimizeTool::validate_inputs() {
  bool all_valid = true;

  std::vector<QLineEdit*> combined = line_edits_;

  combined.insert(combined.end(), particle_boxes_.begin(), particle_boxes_.end());

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
void OptimizeTool::update_run_button() {
  bool inputs_valid = validate_inputs();
  ui_->run_optimize_button->setEnabled(inputs_valid && session_->get_groomed_present());

  if (optimization_is_running_) {
    Style::apply_abort_button_style(ui_->run_optimize_button);
    ui_->run_optimize_button->setText("Abort Optimize");
  } else {
    Style::apply_normal_button_style(ui_->run_optimize_button);
    ui_->run_optimize_button->setText("Run Optimize");
  }
}

//---------------------------------------------------------------------------
void OptimizeTool::setup_domain_boxes() {
  qDeleteAll(ui_->domain_widget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly));
  particle_boxes_.clear();

  QLineEdit* last_box = ui_->number_of_particles;

  if (session_->get_project()->get_number_of_domains_per_subject() < 2) {
    ui_->particle_stack->setCurrentIndex(0);
    ui_->domain_widget->setMaximumSize(1, 1);
  } else {
    ui_->domain_widget->setMaximumSize(9999, 9999);
    auto domain_names = session_->get_project()->get_domain_names();
    QGridLayout* layout = new QGridLayout;
    QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
    for (int i = 0; i < domain_names.size(); i++) {
      auto label = new QLabel(QString::fromStdString(domain_names[i]), this);
      layout->addWidget(label, i, 0);
      QLineEdit* box = new QLineEdit(this);
      last_box = box;
      box->setAlignment(Qt::AlignHCenter);
      box->setValidator(above_zero);
      connect(box, &QLineEdit::textChanged, this, &OptimizeTool::update_run_button);

      particle_boxes_.push_back(box);
      layout->addWidget(box, i, 1);
    }

    delete ui_->domain_widget->layout();
    ui_->domain_widget->setLayout(layout);
    ui_->particle_stack->setCurrentIndex(1);
  }

  QWidget::setTabOrder(last_box, ui_->initial_relative_weighting);
  QWidget::setTabOrder(ui_->initial_relative_weighting, ui_->relative_weighting);
  QWidget::setTabOrder(ui_->relative_weighting, ui_->starting_regularization);
  QWidget::setTabOrder(ui_->starting_regularization, ui_->ending_regularization);
  QWidget::setTabOrder(ui_->ending_regularization, ui_->iterations_per_split);
  QWidget::setTabOrder(ui_->iterations_per_split, ui_->optimization_iterations);
  QWidget::setTabOrder(ui_->optimization_iterations, ui_->use_geodesic_distance);
  QWidget::setTabOrder(ui_->use_geodesic_distance, ui_->use_normals);
  QWidget::setTabOrder(ui_->use_normals, ui_->normals_strength);
  QWidget::setTabOrder(ui_->normals_strength, ui_->procrustes);
  QWidget::setTabOrder(ui_->procrustes, ui_->procrustes_scaling);
  QWidget::setTabOrder(ui_->procrustes_scaling, ui_->procrustes_rotation_translation);
  QWidget::setTabOrder(ui_->procrustes_rotation_translation, ui_->procrustes_interval);
  QWidget::setTabOrder(ui_->procrustes_interval, ui_->multiscale);
  QWidget::setTabOrder(ui_->multiscale, ui_->multiscale_particles);
  QWidget::setTabOrder(ui_->multiscale_particles, ui_->use_landmarks);
  QWidget::setTabOrder(ui_->use_landmarks, ui_->narrow_band);
  QWidget::setTabOrder(ui_->narrow_band, ui_->run_optimize_button);
  QWidget::setTabOrder(ui_->run_optimize_button, ui_->restoreDefaults);
}
