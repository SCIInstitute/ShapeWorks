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
  QSharedPointer<Project> project) : type_(type), groom_(groom), project_(project) {}

ShapeworksWorker::~ShapeworksWorker() {}

void ShapeworksWorker::process() {
  switch (this->type_) {
  case ShapeworksWorker::Groom:
    this->groom_.run();
    break;
  }
  emit result_ready();
}
