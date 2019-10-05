#ifndef __QSHAPEWORKSOPTIMIZE_H__
#define __QSHAPEWORKSOPTIMIZE_H__

#include <Optimize/ShapeWorksOptimize.h>
#include <Groom/ShapeWorksGroom.h>
#include <QObject>

//! Wraps ShapeWorksOptimize as a QObject
class QOptimize : public QObject, public ShapeWorksOptimize {
  Q_OBJECT;
public:
  QOptimize(QObject* parent = nullptr,
            std::vector<ImageType::Pointer> inputs =
              std::vector<ImageType::Pointer>(),
            std::vector<std::array<itk::Point<double>, 3 >> cutPlanes =
              std::vector<std::array<itk::Point<double>, 3 >>(),
            size_t numScales = 1,
            std::vector<double> start_reg = std::vector<double>(),
            std::vector<double> end_reg = std::vector<double>(),
            std::vector<unsigned int> iters = std::vector<unsigned int>(),
            std::vector<double> decay_span = std::vector<double>(),
            std::vector<size_t> procrustes_interval = std::vector<size_t>(),
            double weight = 1.0,
            bool verbose = false);
protected:
  virtual void iterateCallback(itk::Object* caller, const itk::EventObject &) override;
signals:
  void progress(int);
};

#endif // ifndef __QSHAPEWORKSOPTIMIZE_H__
