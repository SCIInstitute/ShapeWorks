---
title: Studio/src/Job/ParticleNormalEvaluationJob.h

---

# Studio/src/Job/ParticleNormalEvaluationJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ParticleNormalEvaluationJob](../Classes/classshapeworks_1_1ParticleNormalEvaluationJob.md)**  |




## Source code

```cpp
#pragma once
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>
#include <QSharedPointer>

namespace shapeworks {

class Session;

class ParticleNormalEvaluationJob : public Job {
  Q_OBJECT
 public:
  ParticleNormalEvaluationJob(QSharedPointer<Session> session, double max_angle_degrees);
  void run() override;
  QString name() override;

  std::vector<bool> get_good_bad();

Q_SIGNALS:

  void result_ready(std::vector<bool> good_bad);

 private:
  QSharedPointer<Session> session_;
  double max_angle_degrees_;

  std::vector<bool> good_bad_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-11-16 at 06:00:37 +0000