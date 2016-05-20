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
  QSharedPointer<Project> project) : type_(type), groom_(groom), 
  optimize_(optimize), project_(project) {}

ShapeworksWorker::~ShapeworksWorker() {}

void ShapeworksWorker::process() {
  switch (this->type_) {
  case ShapeworksWorker::Groom:
    try {
      this->groom_->run();
    } catch (std::exception e) {
      emit error_message(std::string("Error") + e.what());
    }
    break;
  case ShapeworksWorker::Optimize:
    try {
      this->optimize_->run();
      emit message("Warping optimizations to mean space...");
      this->project_->get_mesh_manager()->initializeReconstruction(
        this->optimize_->localPoints(), 
        this->optimize_->globalPoints(), this->optimize_->getImages());
    } catch (std::exception e) {
      emit error_message(std::string("Error") + e.what());
    }
    break;
  }
  emit result_ready();
}
