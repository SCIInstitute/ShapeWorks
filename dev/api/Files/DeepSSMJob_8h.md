---
title: Studio/DeepSSM/DeepSSMJob.h

---

# Studio/DeepSSM/DeepSSMJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md)** <br>Qt Wrapper for DeepSSM.  |




## Source code

```cpp
#pragma once

#include <DeepSSM/DeepSSMTool.h>
#include <Job/Job.h>
#include <Project/Project.h>

#include <QObject>

namespace shapeworks {


class DeepSSMJob : public Job {
  Q_OBJECT;

 public:

  enum PrepStep {
    GROOM_TRAINING = 0,
    OPTIMIZE_TRAINING = 1,
    GROOM_TRAINING_IMAGES = 2,
    GROOM_VAL_IMAGES = 3,
    OPTIMIZE_VALIDATION = 4,
    DONE = 5
  };

  DeepSSMJob(QSharedPointer<Session> session, DeepSSMTool::ToolMode tool_mode);
  ~DeepSSMJob();

  void run() override;

  QString name() override;

  void run_prep();
  void run_augmentation();
  void run_training();
  void run_testing();

  void python_message(std::string str);

  QString get_prep_message();

 private:
  void update_prep_message(PrepStep step);
  void process_test_results();

  QSharedPointer<Session> session_;
  ProjectHandle project_;

  DeepSSMTool::ToolMode tool_mode_;

  QString prep_message_;
  PrepStep prep_step_{GROOM_TRAINING};

  // mutex
  std::mutex mutex_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-02-06 at 21:07:31 +0000
