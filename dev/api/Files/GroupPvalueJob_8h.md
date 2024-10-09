---
title: Studio/Job/GroupPvalueJob.h

---

# Studio/Job/GroupPvalueJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::GroupPvalueJob](../Classes/classshapeworks_1_1GroupPvalueJob.md)**  |




## Source code

```cpp
#pragma once
#include <Job/Job.h>

#include <ParticleShapeStatistics.h>

namespace shapeworks {

class GroupPvalueJob : public Job {
  Q_OBJECT
public:

  GroupPvalueJob(ParticleShapeStatistics stats);
  void run() override;
  QString name() override;

  Eigen::VectorXd get_group_pvalues();

private:

  ParticleShapeStatistics stats_;
  Eigen::VectorXd group_pvalues_;

};
}
```


-------------------------------

Updated on 2024-10-09 at 17:00:46 +0000
