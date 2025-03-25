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
  DeepSSMJob(QSharedPointer<Session> session, DeepSSMTool::ToolMode tool_mode,
             DeepSSMTool::PrepStep prep_step = DeepSSMTool::NOT_STARTED);
  ~DeepSSMJob();

  void run() override;

  QString name() override;

  void run_prep();
  void run_augmentation();
  void run_training();
  void run_testing();

  void python_message(std::string str);

 private:
  void update_prep_stage(DeepSSMTool::PrepStep step);
  void process_test_results();

  QSharedPointer<Session> session_;
  ProjectHandle project_;

  DeepSSMTool::ToolMode tool_mode_;

  QString prep_message_;
  DeepSSMTool::PrepStep prep_step_{DeepSSMTool::NOT_STARTED};

  // mutex
  std::mutex mutex_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-03-25 at 08:57:11 +0000
