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
  ShapeWorksGroom& groom,
  ShapeWorksOptimize& optimize,
  QSharedPointer<Project> project) : type_(type), groom_(groom), 
  optimize_(optimize), project_(project) {}

ShapeworksWorker::~ShapeworksWorker() {}

void ShapeworksWorker::process() {
  switch (this->type_) {
  case ShapeworksWorker::Groom:
    this->groom_.run();
    break;
  case ShapeworksWorker::Optimize:
    this->optimize_.run();
    break;
  }
  emit result_ready();
}
