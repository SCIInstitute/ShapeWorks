#include "QGroom.h"

QGroom::QGroom(QObject * parent,
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground,
  double sigma, double sigmaFastMarch, 
  double iso_value, size_t padding,
  size_t iterations, bool verbose)
  : QObject(parent), ShapeWorksGroom(inputs,
    background, sigma, sigmaFastMarch, iso_value,
    foreground, verbose, padding, iterations) {}

void QGroom::run() {
  this->seed_.Fill(0);
  size_t ran = 0;
  if (this->runTools_.count("center")) {
    this->center();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("isolate")) {
    this->isolate();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("hole_fill")) {
    this->hole_fill();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("auto_crop")) {
    this->auto_crop();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("auto_pad")) {
    this->auto_pad();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("antialias")) {
    this->antialias();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("blur")) {
    this->blur();
    emit progress(++ran * 100 / this->runTools_.size());
  }
  if (this->runTools_.count("fastmarching")) {
    this->fastmarching();
    emit progress(++ran * 100 / this->runTools_.size());
  }
}
