---
title: Studio/Job/ParticleAreaJob.h

---

# Studio/Job/ParticleAreaJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleAreaJob](../Classes/classshapeworks_1_1ParticleAreaJob.md)**  |




## Source code

```cpp
#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

#include <QSharedPointer>

class vtkLookupTable;

namespace shapeworks {

class Session;

class ParticleAreaJob : public Job {
  Q_OBJECT
 public:
  ParticleAreaJob(QSharedPointer<Session> session);
  void run() override;
  QString name() override { return "Particle Normal Evaluation"; }

  const Eigen::VectorXf& get_mean_areas() const { return mean_areas_; }
  const Eigen::VectorXf& get_stddev_areas() const { return stddev_areas_; }

 Q_SIGNALS:

 private:
  QSharedPointer<Session> session_;
  Eigen::VectorXf mean_areas_;
  Eigen::VectorXf stddev_areas_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-11-09 at 17:05:58 +0000
