#pragma once

#include <QObject>

#include <Data/Session.h>

namespace shapeworks {
class Optimize;
class Groom;
class OptimizeParameters;

class ShapeworksWorker : public QObject {
Q_OBJECT

public:
  enum ThreadType {
    GroomType, OptimizeType, ReconstructType
  };

  ShapeworksWorker(ThreadType type,
                   QSharedPointer<Groom> groom,
                   QSharedPointer<Optimize> optimize,
                   QSharedPointer<OptimizeParameters> optimize_parameters,
                   QSharedPointer<Session> session,
                   double maxAngle = 45.,
                   float decimationPercent = 0.3f,
                   int numClusters = 5);
  ~ShapeworksWorker();

public Q_SLOTS:
  void process();

Q_SIGNALS:
  void result_ready();
  void failure();
  void finished();

private:

  QSharedPointer<Groom> groom_;
  QSharedPointer<Optimize> optimize_;
  QSharedPointer<OptimizeParameters> optimize_parameters_;
  QSharedPointer<Session> session_;
  ThreadType type_;
  float decimation_percent_;
  double max_angle_;
  int num_clusters_;
};
}
