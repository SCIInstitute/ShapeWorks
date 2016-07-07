#include "QGroom.h"

QGroom::QGroom(QObject * parent,
  std::vector<ImageType::Pointer> inputs,
  double background, double foreground,
  double sigma, size_t padding,
  size_t iterations, bool verbose)
  : QObject(parent), ShapeWorksGroom(inputs,
    background, foreground, sigma,
     padding, iterations, verbose) {}

void QGroom::run() {
  this->seed_.Fill(0);
  size_t ran = 0, total = this->runTools_.size() * this->images_.size();
  if (this->runTools_.count("center")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->center(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("isolate")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->isolate(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("hole_fill")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->hole_fill(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("auto_pad")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->auto_pad(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("antialias")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->antialias(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("blur")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->blur(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
  if (this->runTools_.count("fastmarching")) {
    for (size_t i = 0; i < this->images_.size(); i++) {
      this->fastmarching(static_cast<int>(i));
      emit progress(++ran * 100 / total);
    }
  }
}
