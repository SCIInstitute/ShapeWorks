---
title: Studio/Analysis/ShapeEvaluationJob.h

---

# Studio/Analysis/ShapeEvaluationJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md)** <br>The [ShapeEvaluationJob]() class is a worker class that computes shape evaluation metrics of compactness, specificity, and generalization. It runs asynchronously using the [Job](../Classes/classshapeworks_1_1Job.md) and [Worker]() interfaces.  |

## Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_DECLARE_METATYPE](../Files/ShapeEvaluationJob_8h.md#function-q-declare-metatype)**(Eigen::VectorXd ) |
| | **[Q_DECLARE_METATYPE](../Files/ShapeEvaluationJob_8h.md#function-q-declare-metatype)**(shapeworks::ShapeEvaluationJob::JobType ) |


## Functions Documentation

### function Q_DECLARE_METATYPE

```cpp
Q_DECLARE_METATYPE(
    Eigen::VectorXd 
)
```


### function Q_DECLARE_METATYPE

```cpp
Q_DECLARE_METATYPE(
    shapeworks::ShapeEvaluationJob::JobType 
)
```




## Source code

```cpp
#pragma once

#include <Data/Worker.h>
#include <Job/Job.h>
#include <ParticleShapeStatistics.h>

#include <QSharedPointer>

namespace shapeworks {

class Session;

class ShapeEvaluationJob : public Job {
  Q_OBJECT
 public:
  enum class JobType { CompactnessType, SpecificityType, GeneralizationType };

  ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats, QSharedPointer<Session> session);

  void run() override;

  QString name() override;

 Q_SIGNALS:

  void report_progress(shapeworks::ShapeEvaluationJob::JobType job_type, float progress);
  void result_ready(shapeworks::ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);

 private:
  void receive_progress(float progress);
  void prep_meshes();

  JobType job_type_;
  ParticleShapeStatistics stats_;
  QSharedPointer<Session> session_;
};
}  // namespace shapeworks

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationJob::JobType);
```


-------------------------------

Updated on 2024-07-10 at 18:20:43 +0000
