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
    } catch (std::exception e) {
      emit error_message(std::string("Error") + e.what());
    }
    break;
  }
  emit result_ready();
}
