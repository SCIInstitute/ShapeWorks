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

  const Eigen::VectorXd& get_mean_areas() const { return mean_areas_; }
  const Eigen::VectorXd& get_stddev_areas() const { return stddev_areas_; }

 Q_SIGNALS:

 private:
  QSharedPointer<Session> session_;
  Eigen::VectorXd mean_areas_;
  Eigen::VectorXd stddev_areas_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-02 at 21:06:13 +0000