---
title: Studio/src/Data/ShapeWorksWorker.h

---

# Studio/src/Data/ShapeWorksWorker.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeworksWorker](../Classes/classshapeworks_1_1ShapeworksWorker.md)**  |




## Source code

```cpp
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
  enum ThreadType {
    GroomType, OptimizeType, ReconstructType
  };

  ShapeworksWorker(ThreadType type,
                   QSharedPointer<QGroom> groom,
                   QSharedPointer<Optimize> optimize,
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
  void failure();
  void error_message(QString);
  void warning_message(QString);
  void message(QString);
  void finished();

private:

  QSharedPointer<QGroom> groom_;
  QSharedPointer<Optimize> optimize_;
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
```


-------------------------------

Updated on 2021-11-15 at 18:13:25 +0000
