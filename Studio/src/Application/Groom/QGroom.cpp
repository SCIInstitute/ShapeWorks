#include "QGroom.h"

#include <tbb/parallel_for.h>

QGroom::QGroom(QObject* parent,
               std::vector<ImageType::Pointer> inputs,
               double background, double foreground,
               double sigma, size_t padding,
               size_t iterations, bool verbose)
  : QObject(parent), ShapeWorksGroom(inputs,
                                     background, foreground, sigma,
                                     padding, iterations, verbose)
{}

void QGroom::run()
{
  this->seed_.Fill(0);
  size_t ran = 0, total = this->runTools_.size() * this->images_.size();
  if (this->runTools_.count("center")) {

    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->center(static_cast<int>(i));
        }
      });

    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
  if (this->runTools_.count("isolate")) {

    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->isolate(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }

  if (this->runTools_.count("hole_fill")) {
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->hole_fill(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
  if (this->runTools_.count("auto_pad")) {
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->auto_pad(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
  if (this->runTools_.count("antialias")) {
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->antialias(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
  if (this->runTools_.count("fastmarching")) {
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->fastmarching(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
  if (this->runTools_.count("blur")) {
    tbb::parallel_for(
      tbb::blocked_range<size_t>{0, this->images_.size()},
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t i = r.begin(); i < r.end(); ++i) {
          this->blur(static_cast<int>(i));
        }
      });
    ran += this->images_.size();
    emit progress(ran * 100 / total);
  }
}
