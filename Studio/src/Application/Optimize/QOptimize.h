#pragma once

#include <Optimize/ShapeWorksOptimize.h>
#include <Libs/Optimize/Optimize.h>
#include <QObject>
#include <QMutex>
#include <QElapsedTimer>

//! Wraps ShapeWorksOptimize as a QObject
class QOptimize : public QObject, public Optimize {
  Q_OBJECT;

public:
  QOptimize(QObject* parent = nullptr);
  virtual ~QOptimize();

  std::vector<std::vector<itk::Point<double>>> GetLocalPoints() override;
  std::vector<std::vector<itk::Point<double>>> GetGlobalPoints() override;

protected:
  virtual void SetIterationCommand() override;
  virtual void IterateCallback(itk::Object* caller, const itk::EventObject &) override;

signals:
  void progress(int);

private:

  itk::MemberCommand<QOptimize>::Pointer iterate_command_;

  // for concurrent access
  QMutex qmutex;

  QElapsedTimer time_since_last_update_;

  size_t reportInterval_ = 0;
  size_t procrustesCounter_ = 0;
  size_t totalIters_ = 0;

};
