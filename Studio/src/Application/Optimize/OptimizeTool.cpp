#include <iostream>

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>

#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Optimize/QOptimize.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool()
{
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi(this);
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

  if (local.size() > 1 && global.size() > 1) {
    this->session_->update_points(local, true);
    this->session_->update_points(global, false);
  }

  QApplication::processEvents();
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete()
{
  this->optimization_is_running_ = false;

  auto local = this->optimize_->GetLocalPoints();
  auto global = this->optimize_->GetGlobalPoints();
  this->session_->update_points(local, true);
  this->session_->update_points(global, false);
  this->session_->set_reconstructed_present(
    local.size() == global.size() && global.size() > 1);
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
    return;
  }
  this->optimization_is_running_ = true;
  emit optimize_start();

  this->enable_actions();

  this->store_settings();
  emit message("Please wait: running optimize step...");
  emit progress(1);

  auto shapes = this->session_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  std::vector<std::string> groomed_filenames;
  for (auto s : shapes) {
    imgs.push_back(s->get_groomed_image());
    groomed_filenames.push_back(s->get_groomed_filename_with_path().toStdString());
  }

  this->optimize_ = new QOptimize(this);

  std::vector<unsigned int> numbers_of_particles;
  numbers_of_particles.push_back(this->ui_->number_of_particles->value());
  this->optimize_->SetFileOutputEnabled(false);
  this->optimize_->SetImages(imgs);
  this->optimize_->SetStartingRegularization(this->ui_->starting_regularization->value());
  this->optimize_->SetEndingRegularization(this->ui_->ending_regularization->value());
  this->optimize_->SetIterationsPerSplit(this->ui_->iterations->value());
  this->optimize_->SetNumberOfParticles(numbers_of_particles);
  this->optimize_->SetOptimizationIterations(this->ui_->optimization_iterations->value());
  this->optimize_->SetProcrustesInterval(this->ui_->procrustes_interval->value());
  this->optimize_->SetRelativeWeighting(this->ui_->weight->value());
  this->optimize_->SetVerbosity(5);

  QThread* thread = new QThread;
  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::OptimizeType, NULL, this->optimize_, this->session_,
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<std::vector<itk::Point<double>>>(),
    std::vector<ImageType::Pointer>());
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
  this->session_->get_project()->set_settings(Parameters::OPTIMIZE_PARAMS, settings);
  // now load those settings
  this->load_settings();
}

//---------------------------------------------------------------------------
void OptimizeTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
}

//---------------------------------------------------------------------------
void OptimizeTool::load_settings()
{
  Parameters settings = this->session_->get_project()->get_settings(Parameters::OPTIMIZE_PARAMS);

  this->ui_->weight->setValue(settings.get("relative_weighting", 1.0));
  this->ui_->number_of_particles->setValue(settings.get("number_of_particles", 128));
  this->ui_->starting_regularization->setValue(settings.get("starting_regularization", 10.0));
  this->ui_->ending_regularization->setValue(settings.get("ending_regularization", 1.0));
  this->ui_->iterations->setValue(settings.get("iterations_per_split", 1000));
  this->ui_->optimization_iterations->setValue(settings.get("optimization_iterations", 1000));
  this->ui_->procrustes_interval->setValue(settings.get("procrustes_interval", 0));
}

//---------------------------------------------------------------------------
void OptimizeTool::store_settings()
{
  Parameters settings = this->session_->get_project()->get_settings(Parameters::OPTIMIZE_PARAMS);
  settings.set("relative_weighting", this->ui_->weight->value());
  settings.set("number_of_particles", this->ui_->number_of_particles->value());
  settings.set("starting_regularization", this->ui_->starting_regularization->value());
  settings.set("ending_regularization", this->ui_->ending_regularization->value());
  settings.set("iterations_per_split", this->ui_->iterations->value());
  settings.set("optimization_iterations", this->ui_->optimization_iterations->value());
  settings.set("procrustes_interval", this->ui_->procrustes_interval->value());

  this->session_->get_project()->set_settings(Parameters::OPTIMIZE_PARAMS, settings);
}

//---------------------------------------------------------------------------
void OptimizeTool::enable_actions()
{
  //this->ui_->run_optimize_button->setEnabled(true);
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
  std::cerr << "Shut Down Optimization Threads";
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
    //this->threads_[i]->exit();

    //std::cerr << "Terminate!\n";
    //this->threads_[i]->terminate();
    //this->threads_[i]->wait(1000);
    //  }
  }
  for (size_t i = 0; i < this->threads_.size(); i++) {
    //delete this->threads_[i];
  }
}
