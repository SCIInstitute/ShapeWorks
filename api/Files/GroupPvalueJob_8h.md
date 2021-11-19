---
title: Studio/src/Job/GroupPvalueJob.h

---

# Studio/src/Job/GroupPvalueJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

  Eigen::VectorXf get_group_pvalues();

private:

  ParticleShapeStatistics stats_;
  Eigen::VectorXf group_pvalues_;

};
}
```


-------------------------------

Updated on 2021-11-19 at 20:39:00 +0000
