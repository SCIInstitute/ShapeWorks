#ifndef __QSHAPEWORKSGROOM_H__
#define __QSHAPEWORKSGROOM_H__

#include <ShapeWorksGroom.h>
#include <QObject>

class QGroom : public QObject, public ShapeWorksGroom {
  Q_OBJECT;
public:
  QGroom(QObject *parent = nullptr,
    std::vector<ImageType::Pointer> inputs = std::vector<ImageType::Pointer>(),
    double background = 0., double foreground = 0.,
    double sigma = 2.0,
    double sigmaFastMarch = 0.0,
    double iso_value = 0.5,
    size_t padding = 0, size_t iterations = 100,
    bool verbose = false);
signals:
  void progress(int);
public:
  virtual void run();
};

#endif