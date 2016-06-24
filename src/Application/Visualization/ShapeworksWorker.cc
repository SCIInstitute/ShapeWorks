/*
 * Shapeworks license
 */

#include <Visualization/ShapeworksWorker.h>
#include <QProcess>
#include <QMessageBox>
#include <sstream>
#include <fstream>
#include <iostream>

ShapeworksWorker::ShapeworksWorker(ThreadType type, 
  ShapeWorksGroom* groom,
  ShapeWorksOptimize* optimize,
  QSharedPointer<Project> project,
  std::vector<std::vector<itk::Point<float> > > local_pts,
  std::vector<std::vector<itk::Point<float> > > global_pts,
  std::vector<ImageType::Pointer> distance_transform,
  double maxAngle,
  float decimationPercent,
  int numClusters) : type_(type), groom_(groom),
  optimize_(optimize), project_(project), 
  local_pts_(local_pts), global_pts_(global_pts), 
  distance_transform_(distance_transform), 
  decimationPercent_(decimationPercent), maxAngle_(maxAngle),
  numClusters_(numClusters)
  {}

ShapeworksWorker::~ShapeworksWorker() {}

void ShapeworksWorker::process() {
  switch (this->type_) {
  case ShapeworksWorker::Groom:
    try {
      this->groom_->run();
    } catch (std::exception e) {
      emit error_message(std::string("Error: ") + e.what());
      return;
    }
    break;
  case ShapeworksWorker::Optimize:
    try {
      this->optimize_->run();
      emit message(std::string("Warping optimizations to mean space..."));
      this->project_->get_mesh_manager()->initializeReconstruction(
        this->optimize_->localPoints(), 
        this->optimize_->globalPoints(), this->optimize_->getImages(),
        this->maxAngle_,
        this->decimationPercent_,
        this->numClusters_);
    } catch (std::exception e) {
      emit error_message(std::string("Error: ") + e.what());
      return;
    }
    break;
  case ShapeworksWorker::Reconstruct:
    try {
      this->project_->get_mesh_manager()->initializeReconstruction(
        this->local_pts_, this->global_pts_, this->distance_transform_,
        this->maxAngle_, this->decimationPercent_,
        this->numClusters_);
    } catch (std::exception e) {
      emit error_message(std::string("Error: ") + e.what());
      return;
    }
    break;
  }
  emit result_ready();
}
