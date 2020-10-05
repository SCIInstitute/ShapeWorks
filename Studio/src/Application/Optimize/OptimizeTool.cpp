#include <iostream>

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>

#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>

#include <Optimize/QOptimize.h>

#include <ui_OptimizeTool.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool()
{
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi(this);

  connect(this->ui_->use_normals, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(this->ui_->procrustes, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);
  connect(this->ui_->multiscale, &QCheckBox::toggled, this, &OptimizeTool::update_ui_elements);

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
    return;
  }
  this->optimization_is_running_ = true;
  emit optimize_start();

  this->enable_actions();

  this->store_params();
  emit message("Please wait: running optimize step...");
  emit progress(1);

  this->optimize_ = new QOptimize(this);
  std::vector<unsigned int> numbers_of_particles;
  numbers_of_particles.push_back(this->ui_->number_of_particles->value());
  this->optimize_->SetFileOutputEnabled(false);

  this->optimize_->SetDomainsPerShape(1); /// only one domain per shape right now

  this->optimize_->SetNumberOfParticles(numbers_of_particles);
  this->optimize_->SetInitialRelativeWeighting(this->ui_->initial_relative_weighting->value());
  this->optimize_->SetRelativeWeighting(this->ui_->relative_weighting->value());
  this->optimize_->SetStartingRegularization(this->ui_->starting_regularization->value());
  this->optimize_->SetEndingRegularization(this->ui_->ending_regularization->value());
  this->optimize_->SetIterationsPerSplit(this->ui_->iterations_per_split->value());
  this->optimize_->SetOptimizationIterations(this->ui_->optimization_iterations->value());

  std::vector<bool> use_normals;
  std::vector<bool> use_xyz;
  std::vector<double> attr_scales;

  attr_scales.push_back(1);
  attr_scales.push_back(1);
  attr_scales.push_back(1);

  if (this->ui_->use_normals->isChecked()) {
    use_normals.push_back(1);
    use_xyz.push_back(1);
    double normals_strength = this->ui_->normals_strength->value();
    attr_scales.push_back(normals_strength);
    attr_scales.push_back(normals_strength);
    attr_scales.push_back(normals_strength);
  }
  else {
    use_normals.push_back(0);
    use_xyz.push_back(0);
  }
  this->optimize_->SetUseNormals(use_normals);
  this->optimize_->SetUseXYZ(use_xyz);
  this->optimize_->SetUseMeshBasedAttributes(this->ui_->use_normals->isChecked());
  this->optimize_->SetAttributeScales(attr_scales);


  std::vector<int> attributes_per_domain;
  this->optimize_->SetAttributesPerDomain(attributes_per_domain);

  int procrustes_interval = 0;
  if (this->ui_->procrustes->isChecked()) {
    procrustes_interval = this->ui_->procrustes_interval->value();
  }
  this->optimize_->SetProcrustesInterval(procrustes_interval);
  this->optimize_->SetProcrustesScaling(this->ui_->procrustes_scaling->isChecked());
  this->optimize_->SetVerbosity(5);

  int multiscale_particles = 0;
  if (this->ui_->multiscale->isChecked()) {
    multiscale_particles = this->ui_->multiscale_particles->value();
  }
  this->optimize_->SetUseShapeStatisticsAfter(multiscale_particles);


  // should add the images last
  auto shapes = this->session_->get_shapes();
  for (auto s : shapes) {
    auto image = s->get_groomed_image();
    if (!image) {
      emit error_message("Error loading groomed images");
      return;
    }
    this->optimize_->AddImage(image);
  }

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
  Parameters params = this->session_->get_project()->get_parameters(Parameters::OPTIMIZE_PARAMS);

  this->ui_->number_of_particles->setValue(params.get("number_of_particles", 128));
  this->ui_->initial_relative_weighting->setValue(params.get("initial_relative_weighting", 1.0));
  this->ui_->relative_weighting->setValue(params.get("relative_weighting", 1.0));
  this->ui_->starting_regularization->setValue(params.get("starting_regularization", 10.0));
  this->ui_->ending_regularization->setValue(params.get("ending_regularization", 1.0));
  this->ui_->iterations_per_split->setValue(params.get("iterations_per_split", 1000));
  this->ui_->optimization_iterations->setValue(params.get("optimization_iterations", 1000));

  this->ui_->use_normals->setChecked(params.get("use_normals", false));
  this->ui_->normals_strength->setValue(params.get("normals_strength", 10));

  this->ui_->procrustes->setChecked(params.get("procrustes", false));
  this->ui_->procrustes_scaling->setChecked(params.get("procrustes_scaling", false));
  this->ui_->procrustes_interval->setValue(params.get("procrustes_interval", 0));

  this->ui_->multiscale->setChecked(params.get("multiscale", false));
  this->ui_->multiscale_particles->setValue(params.get("multiscale_particles", 32));

  this->update_ui_elements();

}

//---------------------------------------------------------------------------
void OptimizeTool::store_params()
{
  Parameters params = this->session_->get_project()->get_parameters(Parameters::OPTIMIZE_PARAMS);
  params.set("number_of_particles", this->ui_->number_of_particles->value());
  params.set("initial_relative_weighting", this->ui_->initial_relative_weighting->value());
  params.set("relative_weighting", this->ui_->relative_weighting->value());
  params.set("starting_regularization", this->ui_->starting_regularization->value());
  params.set("ending_regularization", this->ui_->ending_regularization->value());
  params.set("iterations_per_split", this->ui_->iterations_per_split->value());
  params.set("optimization_iterations", this->ui_->optimization_iterations->value());

  params.set("use_normals", this->ui_->use_normals->isChecked());
  params.set("normals_strength", this->ui_->normals_strength->value());

  params.set("procrustes", this->ui_->procrustes->isChecked());
  params.set("procrustes_scaling", this->ui_->procrustes_scaling->isChecked());
  params.set("procrustes_interval", this->ui_->procrustes_interval->value());

  params.set("multiscale", this->ui_->multiscale->isChecked());
  params.set("multiscale_particles", this->ui_->multiscale_particles->value());

  this->session_->get_project()->set_parameters(Parameters::OPTIMIZE_PARAMS, params);
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

//---------------------------------------------------------------------------
void OptimizeTool::update_ui_elements()
{
  this->ui_->normals_strength->setEnabled(this->ui_->use_normals->isChecked());
  this->ui_->procrustes_scaling->setEnabled(this->ui_->procrustes->isChecked());
  this->ui_->procrustes_interval->setEnabled(this->ui_->procrustes->isChecked());
  this->ui_->multiscale_particles->setEnabled(this->ui_->multiscale->isChecked());
}
