#include <iostream>

#include <itkeigen/Eigen/Dense>
#include <itkeigen/Eigen/Sparse>

#include <QFileDialog>
#include <QThread>
#include <QMessageBox>

#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeWorksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Optimize/QOptimize.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs)
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

  if (local.size() > 2 && global.size() > 2) {
    this->project_->update_points(local, true);
    this->project_->update_points(global, false);
  }

  QApplication::processEvents();
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete()
{
  this->optimization_is_running_ = false;

  auto local = this->optimize_->GetLocalPoints();
  auto global = this->optimize_->GetGlobalPoints();
  this->project_->update_points(local, true);
  this->project_->update_points(global, false);
  this->project_->set_reconstructed_present(
    local.size() == global.size() && global.size() > 1);
  this->project_->calculate_reconstructed_samples();
  emit progress(100);
  emit message("Optimize Complete");
  emit optimize_complete();
  this->ui_->run_optimize_button->setEnabled(true);

  /// TODO: studio
  ///this->ui_->reconstructionButton->setEnabled(this->project_->reconstructed_present());
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked()
{
  if (this->optimization_is_running_) {
    this->shutdown_threads();
    this->optimization_is_running_ = false;
    this->enableActions();
    return;
  }
  this->optimization_is_running_ = true;
  emit optimize_start();

  this->enableActions();

  this->update_preferences();
  emit message("Please wait: running optimize step...");
  emit progress(1);
  //this->ui_->run_optimize_button->setEnabled(false);
  /// TODO: studio
  ///this->ui_->reconstructionButton->setEnabled(false);
  auto shapes = this->project_->get_shapes();
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
  this->optimize_->SetCutPlanes(this->cutPlanes_);
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
    ShapeworksWorker::OptimizeType, NULL, this->optimize_, this->project_,
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
void OptimizeTool::on_meshDecimation_valueChanged(double v)
{
  this->preferences_.set_preference("optimize_decimation", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_numClusters_valueChanged(int v)
{
  this->preferences_.set_preference("optimize_clusters", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_weight_valueChanged(double v)
{
  this->preferences_.set_preference("optimize_weight", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_maxAngle_valueChanged(double v)
{
  this->preferences_.set_preference("optimize_maxAngle", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_starting_regularization_valueChanged(double v)
{
  this->preferences_.set_preference("optimize_start_reg", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_ending_regularization_valueChanged(double v)
{
  this->preferences_.set_preference("optimize_end_reg", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_iterations_valueChanged(int v)
{
  this->preferences_.set_preference("optimize_iters", v);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_decay_span_valueChanged(int v)
{
  this->preferences_.set_preference("optimize_decay_span",
                                    static_cast<double>(v));
}

//---------------------------------------------------------------------------
void OptimizeTool::on_procrustes_interval_valueChanged(int v)
{
  this->preferences_.set_preference("optimize_procrustes_interval",
                                    static_cast<double>(v));
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_message(std::string s)
{
  emit message(s);
}

//---------------------------------------------------------------------------
void OptimizeTool::loadCutPlanesFile(std::string file)
{
  std::ifstream planes(file.c_str());
  if (!planes.good()) {
    emit error_message("Cannot open file: " + file);
    this->ui_->cutPlanesFile->setText("None Selected");
    this->cutPlanes_.clear();
    return;
  }
  std::vector<std::array<itk::Point<double>, 3 >> cutPlanes;
  while (planes.good()) {
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    planes >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    if (!planes.fail()) {
      itk::Point<double> p1, p2, p3;
      p1[0] = x1; p1[1] = y1; p1[2] = z1;
      p2[0] = x2; p2[1] = y2; p2[2] = z2;
      p3[0] = x3; p3[1] = y3; p3[2] = z3;
      cutPlanes.push_back({ { p1, p2, p3 } });
    }
  }
  planes.close();
  if (cutPlanes.empty() || (cutPlanes.size() > 1 &&
                            cutPlanes.size() != this->project_->get_shapes().size())) {
    emit error_message(std::string("Error reading cutting plane file. Must have 1 ") +
                       "set of 3 points, or X set of 3 points, where X is number of samples.");
    this->ui_->cutPlanesFile->setText("None Selected");
    this->cutPlanes_.clear();
    return;
  }
  emit message("Successfully read in cut planes file: " + file);
  this->cutPlanes_ = cutPlanes;
}

//---------------------------------------------------------------------------
void OptimizeTool::on_cutPlanesFile_editingFinished()
{
  //get the cutting planes from file if possible
  auto file = this->ui_->cutPlanesFile->text().toStdString();
  if (file.empty() || file == "None Selected") {
    this->ui_->cutPlanesFile->setText("None Selected");
    this->cutPlanes_.clear();
    return;
  }
  this->loadCutPlanesFile(file);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_cutPlanesFileButton_clicked()
{
  std::string file;
  QString filename;
  filename = QFileDialog::getOpenFileName(this,
                                          tr("Import Clipping Planes File..."),
                                          this->preferences_.get_preference("Main/last_directory",
                                                                            QString()),
                                          tr("Text files (*.txt)"));
  if (filename.isEmpty()) {
    return;
  }
  this->preferences_.set_preference("Main/last_directory",
                                    QDir().absoluteFilePath(filename));
  file = filename.toStdString();
  this->ui_->cutPlanesFile->setText(QString::fromStdString(file));
  this->loadCutPlanesFile(file);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_restoreDefaults_clicked()
{
  this->preferences_.delete_entry("optimize_clusters");
  this->preferences_.delete_entry("optimize_decimation");
  this->preferences_.delete_entry("optimize_particles");
  this->preferences_.delete_entry("optimize_start_reg");
  this->preferences_.delete_entry("optimize_end_reg");
  this->preferences_.delete_entry("optimize_iters");
  this->preferences_.delete_entry("optimize_weight");
  this->preferences_.delete_entry("optimize_maxAngle");
  this->preferences_.delete_entry("optimize_decay_span");
  this->preferences_.delete_entry("optimize_procrustes_interval");
  this->preferences_.delete_entry("optimize_iters_optimization");

  this->preferences_.restore_defaults();
  this->set_preferences();
  auto optimize_particles = this->preferences_.get_preference("optimize_particles", 128);
  this->ui_->number_of_particles->setValue(optimize_particles);
  qApp->processEvents();
  this->preferences_.set_saved(false);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_number_of_particles_valueChanged(int val)
{
  this->preferences_.set_preference("optimize_particles", val);
}

//---------------------------------------------------------------------------
void OptimizeTool::set_project(QSharedPointer<Project> project)
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void OptimizeTool::set_preferences(bool setScales)
{
  this->ui_->weight->setValue(
    this->preferences_.get_preference("optimize_weight",
                                      this->ui_->weight->value()));
  this->ui_->number_of_particles->setValue(
    this->preferences_.get_preference("optimize_particles",
                                      this->ui_->number_of_particles->value()));
  this->ui_->starting_regularization->setValue(
    this->preferences_.get_preference("optimize_start_reg",
                                      this->ui_->starting_regularization->value()));
  this->ui_->ending_regularization->setValue(
    this->preferences_.get_preference("optimize_end_reg",
                                      this->ui_->ending_regularization->value()));
  this->ui_->iterations->setValue(
    this->preferences_.get_preference("optimize_iters",
                                      this->ui_->iterations->value()));
  this->ui_->optimization_iterations->setValue(
    this->preferences_.get_preference("optimize_iters_optimization",
                                      this->ui_->optimization_iterations->value()));
  this->ui_->procrustes_interval->setValue(
    this->preferences_.get_preference("optimize_procrustes_interval",
                                      this->ui_->procrustes_interval->value()));
}

//---------------------------------------------------------------------------
void OptimizeTool::update_preferences()
{
  this->preferences_.set_preference("optimize_weight",
                                    this->ui_->weight->value());
  this->preferences_.set_preference("optimize_particles",
                                    this->ui_->number_of_particles->value());
  this->preferences_.set_preference("optimize_start_reg",
                                    this->ui_->starting_regularization->value());
  this->preferences_.set_preference("optimize_end_reg",
                                    this->ui_->ending_regularization->value());
  this->preferences_.set_preference("optimize_iters",
                                    this->ui_->iterations->value());
  this->preferences_.set_preference("optimize_iters_optimization",
                                    this->ui_->optimization_iterations->value());
  this->preferences_.set_preference("optimize_procrustes_interval",
                                    this->ui_->procrustes_interval->value());
}

//---------------------------------------------------------------------------
void OptimizeTool::enableActions()
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
void OptimizeTool::disableActions()
{
  //this->ui_->run_optimize_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void OptimizeTool::setCutPlanesFile(std::string file)
{
  this->ui_->cutPlanesFile->setText(QString::fromStdString(file));
  this->loadCutPlanesFile(file);
}

//---------------------------------------------------------------------------
std::string OptimizeTool::getCutPlanesFile()
{
  auto name = this->ui_->cutPlanesFile->text().toStdString();
  if (name == "None Selected") {
    return "";
  }
  return name;
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
