#include <iostream>

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QTemporaryDir>

#include <Optimize/OptimizeTool.h>
#include <Libs/Optimize/OptimizeParameters.h>

#include <Visualization/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>

#include <Optimize/QOptimize.h>

#include <vtkPLYWriter.h>

#include <ui_OptimizeTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool()
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
  this->ui_->use_normals->setToolTip("Use surface normals as part of optimization");
  this->ui_->normals_strength->setToolTip("Strength of surface normals relative to position");
  this->ui_->procrustes->setToolTip("Use procrustes registration during optimization");
  this->ui_->procrustes_interval->setToolTip("How often to run procrustes during optimization");
  this->ui_->procrustes_scaling->setToolTip("Use procrustes scaling");
  this->ui_->multiscale->setToolTip("Use multiscale optimization mode");
  this->ui_->multiscale_particles->setToolTip(
    "Start multiscale optimization after this many particles");
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error(std::string msg)
{
  emit error_message(msg);
  this->ui_->run_optimize_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_warning(std::string msg)
{
  emit warning_message(msg);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val)
{
  emit progress(static_cast<size_t>(val));

  auto local = this->optimize_->GetLocalPoints();
  auto global = this->optimize_->GetGlobalPoints();

  if (local.size() > 0 && global.size() > 0) {
    this->session_->update_points(local, true);
    this->session_->update_points(global, false);
  }
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete()
{
  this->optimization_is_running_ = false;

  auto local = this->optimize_->GetLocalPoints();
  auto global = this->optimize_->GetGlobalPoints();
  this->session_->update_points(local, true);
  this->session_->update_points(global, false);
  //this->session_->set_reconstructed_present(
//    local.size() == global.size() && global.size() > 1);
  this->session_->calculate_reconstructed_samples();
  this->session_->get_project()->store_subjects();
  emit progress(100);
  emit message("Optimize Complete");
  emit optimize_complete();
  this->ui_->run_optimize_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked()
{
  if (this->optimization_is_running_) {
    this->shutdown_threads();
    this->optimization_is_running_ = false;
    this->enable_actions();
    emit progress(100);
    return;
  }
  this->optimization_is_running_ = true;
  emit optimize_start();

  this->enable_actions();

  this->store_params();
  emit message("Please wait: running optimize step...");
  emit progress(1);

  this->optimize_ = new QOptimize(this);

  try {
    OptimizeParameters params(this->session_->get_project());
    params.set_up_optimize(this->optimize_);
  } catch (std::exception& e) {
    emit error_message(std::string("Error running optimize: ") + e.what());
    this->optimization_is_running_ = false;
    this->enable_actions();
    return;
  }

  this->optimize_->SetFileOutputEnabled(false);

  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::OptimizeType, NULL, this->optimize_, this->session_,
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<std::string>());

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_optimize_complete()));
  connect(this->optimize_, SIGNAL(progress(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(message(std::string)), this, SLOT(handle_message(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
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
  // store a set of blank settings
  Parameters settings;
  this->session_->get_project()->set_parameters(Parameters::OPTIMIZE_PARAMS, settings);
  // now load those settings
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
  auto params = OptimizeParameters(this->session_->get_project());

  this->ui_->number_of_particles->setValue(params.get_number_of_particles()[0]);
  this->ui_->initial_relative_weighting->setValue(params.get_initial_relative_weighting());
  this->ui_->relative_weighting->setValue(params.get_relative_weighting());
  this->ui_->starting_regularization->setValue(params.get_starting_regularization());
  this->ui_->ending_regularization->setValue(params.get_ending_regularization());
  this->ui_->iterations_per_split->setValue(params.get_iterations_per_split());
  this->ui_->optimization_iterations->setValue(params.get_optimization_iterations());

  this->ui_->use_normals->setChecked(params.get_use_normals()[0]);
  this->ui_->normals_strength->setValue(params.get_normals_strength());

  this->ui_->procrustes->setChecked(params.get_use_procrustes());
  this->ui_->procrustes_scaling->setChecked(params.get_use_procrustes_scaling());
  this->ui_->procrustes_interval->setValue(params.get_procrustes_interval());

  this->ui_->multiscale->setChecked(params.get_use_multiscale());
  this->ui_->multiscale_particles->setValue(params.get_multiscale_particles());

  this->update_ui_elements();

}

//---------------------------------------------------------------------------
void OptimizeTool::store_params()
{
  auto params = OptimizeParameters(this->session_->get_project());

  params.set_number_of_particles({this->ui_->number_of_particles->value()});
  params.set_initial_relative_weighting(this->ui_->initial_relative_weighting->value());
  params.set_relative_weighting(this->ui_->relative_weighting->value());
  params.set_starting_regularization(this->ui_->starting_regularization->value());
  params.set_ending_regularization(this->ui_->ending_regularization->value());
  params.set_iterations_per_split(this->ui_->iterations_per_split->value());
  params.set_optimization_iterations(this->ui_->optimization_iterations->value());

  params.set_use_normals({this->ui_->use_normals->isChecked()});
  params.set_normals_strength(this->ui_->normals_strength->value());

  params.set_use_procrustes(this->ui_->procrustes->isChecked());
  params.set_use_procrustes_scaling(this->ui_->procrustes_scaling->isChecked());
  params.set_procrustes_interval(this->ui_->procrustes_interval->value());

  params.set_use_multiscale(this->ui_->multiscale->isChecked());
  params.set_multiscale_particles(this->ui_->multiscale_particles->value());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void OptimizeTool::enable_actions()
{
  this->ui_->run_optimize_button->setEnabled(this->session_->get_groomed_present());

  if (this->optimization_is_running_) {
    this->ui_->run_optimize_button->setText("Abort Optimize");
  }
  else {
    this->ui_->run_optimize_button->setText("Run Optimize");
  }
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
  this->optimize_->AbortOptimization();

  for (size_t i = 0; i < this->threads_.size(); i++) {
    if (this->threads_[i]->isRunning()) {
      std::cerr << "waiting...\n";
      this->threads_[i]->wait(1000);
      std::cerr << "done waiting...\n";
    }
  }
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
}