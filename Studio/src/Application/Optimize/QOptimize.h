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
  virtual ~QOptimize();

protected:
  virtual void SetIterationCommand() override;
  virtual void iterateCallback(itk::Object* caller, const itk::EventObject &) override;
signals:
  void progress(int);

private:

  itk::MemberCommand<QOptimize>::Pointer iterate_command_;
};

#endif // ifndef __QSHAPEWORKSOPTIMIZE_H__
