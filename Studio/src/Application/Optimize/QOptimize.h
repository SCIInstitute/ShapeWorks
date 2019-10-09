#ifndef __QSHAPEWORKSOPTIMIZE_H__
#define __QSHAPEWORKSOPTIMIZE_H__

#include <Optimize/ShapeWorksOptimize.h>
#include <Groom/ShapeWorksGroom.h>
#include <QObject>

//! Wraps ShapeWorksOptimize as a QObject
class QOptimize : public QObject, public ShapeWorksOptimize {
  Q_OBJECT;
public:
  QOptimize(QObject* parent = nullptr);
protected:
  virtual void iterateCallback(itk::Object* caller, const itk::EventObject &) override;
signals:
  void progress(int);
};

#endif // ifndef __QSHAPEWORKSOPTIMIZE_H__
