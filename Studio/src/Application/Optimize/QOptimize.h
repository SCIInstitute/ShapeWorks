#pragma once

#include <Optimize/ShapeWorksOptimize.h>
#include <QObject>
#include <QMutex>
#include <QElapsedTimer>

//! Wraps ShapeWorksOptimize as a QObject
class QOptimize : public QObject, public ShapeWorksOptimize {
  Q_OBJECT;

public:
  QOptimize(QObject* parent = nullptr);
  virtual ~QOptimize();

  std::vector<std::vector<itk::Point<double>>> localPoints() override;
  std::vector<std::vector<itk::Point<double>>> globalPoints() override;


protected:
  virtual void SetIterationCommand() override;
  virtual void iterateCallback(itk::Object* caller, const itk::EventObject &) override;

signals:
  void progress(int);

private:

  itk::MemberCommand<QOptimize>::Pointer iterate_command_;

  // for concurrent access
  QMutex mutex;

  QElapsedTimer time_since_last_update_;

};
