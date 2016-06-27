#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeworksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs) {
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi( this );
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error(std::string msg) {
  emit error_message(msg);
  this->ui_->run_optimize_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_warning(std::string msg) {
  emit warning_message(msg);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val) {
  emit progress(static_cast<size_t>(val));
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_optimize_complete() {
  auto local = this->optimize_->localPoints();
  auto global = this->optimize_->globalPoints();
  this->project_->load_points(local, true);
  this->project_->load_points(global, false);
  this->project_->set_reconstructed_present(
    local.size() == global.size() && global.size() > 1);
  this->project_->calculate_reconstructed_samples();
  emit progress(100);
  emit message("Optimize Complete");
  emit optimize_complete();
  this->ui_->run_optimize_button->setEnabled(true);
  this->ui_->reconstructionButton->setEnabled(
    this->project_->reconstructed_present());
}

void OptimizeTool::handle_reconstruction_complete() {
  this->project_->handle_clear_cache();
  this->project_->calculate_reconstructed_samples();
  emit progress(100);
  emit message("Reconstruction Complete");
  emit reconstruction_complete();
  this->ui_->run_optimize_button->setEnabled(true);
  this->ui_->reconstructionButton->setEnabled(
    this->project_->reconstructed_present());
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked() {
  this->update_preferences();
  emit message("Please wait: running optimize step...");
  emit progress(1);
  this->ui_->run_optimize_button->setEnabled(false);
  this->ui_->reconstructionButton->setEnabled(false);
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (auto s : shapes) {
    imgs.push_back(s->get_groomed_image());
  }
  auto scales = static_cast<size_t>(std::log2(this->ui_->number_of_particles->value()) + 1);
  this->optimize_ = new QOptimize(this, imgs, this->cutPlanes_, scales,
    std::vector<double>(scales,this->ui_->starting_regularization->value()),
    std::vector<double>(scales, this->ui_->ending_regularization->value()),
    std::vector<unsigned int>(scales, this->ui_->iterations->value()),
    std::vector<double>(scales, this->ui_->decay_span->value()),
    this->ui_->weight->value(), true);

  QThread *thread = new QThread;
  ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Optimize, NULL, this->optimize_, this->project_,
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<ImageType::Pointer>(),
    this->ui_->maxAngle->value(),
    this->ui_->meshDecimation->value(), 
    this->ui_->numClusters->value());
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_optimize_complete()));
  connect(this->optimize_, SIGNAL(progress(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(message(std::string)), this, SLOT(handle_message(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
}

void OptimizeTool::on_meshDecimation_valueChanged(double v) {
  this->preferences_.set_preference("optimize_decimation", v);
}

void OptimizeTool::on_numClusters_valueChanged(int v) {
  this->preferences_.set_preference("optimize_clusters", v);
}

void OptimizeTool::on_weight_valueChanged(double v) {
  this->preferences_.set_preference("optimize_weight", v);
}

void OptimizeTool::on_maxAngle_valueChanged(double v) {
  this->preferences_.set_preference("optimize_maxAngle", v);
}

void OptimizeTool::on_starting_regularization_valueChanged(double v) {
  this->preferences_.set_preference("optimize_start_reg", v);
}

void OptimizeTool::on_ending_regularization_valueChanged(double v) {
  this->preferences_.set_preference("optimize_end_reg", v);
}

void OptimizeTool::on_iterations_valueChanged(int v) {
  this->preferences_.set_preference("optimize_iters", v);
}

void OptimizeTool::on_decay_span_valueChanged(int v) {
  this->preferences_.set_preference("optimize_decay_span",
    static_cast<double>(v));
}

void OptimizeTool::handle_message(std::string s) {
  emit message(s);
}

void OptimizeTool::loadCutPlanesFile(std::string file) {
  std::ifstream planes(file.c_str());
  if (!planes.good()) {
    emit error_message("Cannot open file: " + file);
    this->ui_->cutPlanesFile->setText("None Selected");
    this->cutPlanes_.clear();
    return;
  }
  std::vector<std::array<itk::Point<float>, 3 > > cutPlanes;
  while (planes.good()) {
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    planes >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    if (!planes.fail()) {
      itk::Point<float> p1, p2, p3;
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

void OptimizeTool::on_cutPlanesFile_editingFinished() {
  //get the cutting planes from file if possible
  auto file = this->ui_->cutPlanesFile->text().toStdString();
  if (file.empty()) {
    this->ui_->cutPlanesFile->setText("None Selected");
    this->cutPlanes_.clear();
    return;
  }
  this->loadCutPlanesFile(file);
}

void OptimizeTool::on_reconstructionButton_clicked() {
  this->update_preferences();
  emit message("Please wait: running reconstruction step...");
  emit progress(5);
  this->ui_->run_optimize_button->setEnabled(false);
  this->ui_->reconstructionButton->setEnabled(false);
  QThread *thread = new QThread;
  std::vector<std::vector<itk::Point<float> > > local, global;
  std::vector<ImageType::Pointer> images;
  auto shapes = this->project_->get_shapes();
  local.resize(shapes.size());
  global.resize(shapes.size());
  images.resize(shapes.size());
  size_t ii = 0;
  for (auto &s : shapes) {
    auto l = s->get_local_correspondence_points();
    auto g = s->get_global_correspondence_points();
    for (size_t i = 0; i < l.size(); i+=3) {
      itk::Point<float> pt, pt2;
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
  ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Reconstruct, NULL, NULL, this->project_,
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

void OptimizeTool::on_cutPlanesFileButton_clicked() {
  std::string file;
  QString filename;
  filename = QFileDialog::getOpenFileName(this,
    tr("Import Clipping Planes File..."),
    this->preferences_.get_preference("Main/last_directory", QString()),
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

void OptimizeTool::on_restoreDefaults_clicked() {
  this->preferences_.delete_entry("optimize_clusters");
  this->preferences_.delete_entry("optimize_decimation");
  this->preferences_.delete_entry("optimize_particles");
  this->preferences_.delete_entry("optimize_start_reg");
  this->preferences_.delete_entry("optimize_end_reg");
  this->preferences_.delete_entry("optimize_iters");
  this->preferences_.delete_entry("optimize_weight");
  this->preferences_.delete_entry("optimize_maxAngle");
  this->preferences_.delete_entry("optimize_decay_span");
  this->preferences_.restore_defaults();
  this->set_preferences();
  auto optimize_particles = this->preferences_.get_preference("optimize_particles", 128);
  this->ui_->number_of_particles->setValue(optimize_particles);
  qApp->processEvents();
  this->preferences_.set_saved(false);
}

void OptimizeTool::on_number_of_particles_valueChanged(int val) {
  this->preferences_.set_preference("optimize_particles", val);
}

//---------------------------------------------------------------------------
void OptimizeTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

void OptimizeTool::set_preferences(bool setScales) {
  this->ui_->numClusters->setValue(
    this->preferences_.get_preference("optimize_clusters",
      this->ui_->numClusters->value()));
  this->ui_->meshDecimation->setValue(
    this->preferences_.get_preference("optimize_decimation",
      this->ui_->meshDecimation->value()));
  this->ui_->maxAngle->setValue(
    this->preferences_.get_preference("optimize_maxAngle",
      this->ui_->maxAngle->value()));
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
  this->ui_->decay_span->setValue(
    this->preferences_.get_preference("optimize_decay_span",
      this->ui_->decay_span->value()));
}

void OptimizeTool::update_preferences() {
  this->preferences_.set_preference("optimize_clusters",
    this->ui_->numClusters->value());
  this->preferences_.set_preference("optimize_decimation",
      this->ui_->meshDecimation->value());
  this->preferences_.set_preference("optimize_maxAngle",
    this->ui_->maxAngle->value());
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
  this->preferences_.set_preference("optimize_decay_span",
    this->ui_->decay_span->value());


}

void OptimizeTool::enableActions() {
  this->ui_->run_optimize_button->setEnabled(true);
}

void OptimizeTool::disableActions() {
  this->ui_->run_optimize_button->setEnabled(false);
}

void OptimizeTool::setCutPlanesFile(std::string file) {
  this->ui_->cutPlanesFile->setText(QString::fromStdString(file));
  this->loadCutPlanesFile(file);
}

std::string OptimizeTool::getCutPlanesFile() {
  auto name = this->ui_->cutPlanesFile->text().toStdString();
  if (name.find_last_of(".txt") == std::string::npos) {
    return "";
  }
  return name;
}
