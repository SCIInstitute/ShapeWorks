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
| class | **[shapeworks::ShapeEvaluationJob](../Classes/classshapeworks_1_1ShapeEvaluationJob.md)**  |

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

#include <ParticleShapeStatistics.h>

#include <Data/Worker.h>
#include <Job/Job.h>

namespace shapeworks {

class ShapeEvaluationJob : public Job
{
  Q_OBJECT
public:

  enum class JobType {
    CompactnessType, SpecificityType, GeneralizationType
  };

  ShapeEvaluationJob(JobType job_type, ParticleShapeStatistics stats);

  void run() override;

  QString name() override;

Q_SIGNALS:

  void report_progress(shapeworks::ShapeEvaluationJob::JobType job_type, float progress);
  void result_ready(shapeworks::ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);

private:

  void receive_progress(float progress);

  JobType job_type_;
  ParticleShapeStatistics stats_;
};
}

Q_DECLARE_METATYPE(Eigen::VectorXd);
Q_DECLARE_METATYPE(shapeworks::ShapeEvaluationJob::JobType);
```


-------------------------------

Updated on 2023-04-29 at 18:09:09 +0000
