#pragma once

#ifndef Q_MOC_RUN
#include <Analyze/Particles.h>
#include <Optimize/Optimize.h>
#endif
#include <QElapsedTimer>
#include <QMutex>
#include <QObject>

namespace shapeworks {

//! Wraps Optimize as a QObject
class QOptimize : public QObject, public Optimize {
  Q_OBJECT;

 public:
  QOptimize(QObject* parent = nullptr);
  virtual ~QOptimize();

  std::vector<std::vector<itk::Point<double>>> GetLocalPoints() override;
  std::vector<std::vector<itk::Point<double>>> GetGlobalPoints() override;

  std::vector<Particles> GetParticles();

  std::vector<std::vector<std::vector<double>>> GetProcrustesTransforms() override;

  void UpdateExportablePoints() override;

 protected:
  virtual void SetIterationCallback() override;
  virtual void IterateCallback(itk::Object* caller, const itk::EventObject&) override;

 Q_SIGNALS:
  void progress(int, QString);

 private:
  itk::MemberCommand<QOptimize>::Pointer iterate_command_;

  // for concurrent access
  QMutex qmutex_;

  QElapsedTimer time_since_last_update_;
};

}  // namespace shapeworks
