#pragma once

#include <QObject>

#include <Data/Session.h>

namespace shapeworks {
class Optimize;
class QGroom;
class OptimizeParameters;

class ShapeworksWorker : public QObject {
Q_OBJECT

public:
  enum ThreadType { GroomType, OptimizeType, ReconstructType };
  ShapeworksWorker(ThreadType type, QSharedPointer<shapeworks::QGroom> groom,
                   shapeworks::Optimize* optimize,
                   QSharedPointer<OptimizeParameters> optimize_parameters,
                   QSharedPointer<Session> session,
                   std::vector<std::vector<itk::Point<double>>> local_pts =
                   std::vector<std::vector<itk::Point<double>>>(),
                   std::vector<std::vector<itk::Point<double>>> global_pts =
                   std::vector<std::vector<itk::Point<double>>>(),
                   std::vector<std::string> distance_transform =
                   std::vector<std::string>(),
                   double maxAngle = 45.,
                   float decimationPercent = 0.3f,
                   int numClusters = 5);
  ~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void error_message(std::string);
  void warning_message(std::string);
  void step_made(int val);
  void finished();
  void message(std::string);

private:
  QSharedPointer<shapeworks::QGroom> groom_;
  shapeworks::Optimize* optimize_;
  QSharedPointer<OptimizeParameters> optimize_parameters_;
  QSharedPointer<Session> session_;
  ThreadType type_;
  std::vector<std::vector<itk::Point<double>>> local_pts_;
  std::vector<std::vector<itk::Point<double>>> global_pts_;
  std::vector<std::string> distance_transform_;
  float decimation_percent_;
  double max_angle_;
  int num_clusters_;
};
}