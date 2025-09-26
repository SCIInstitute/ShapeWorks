---
title: Libs/Application/DeepSSM/DeepSSMJob.h

---

# Libs/Application/DeepSSM/DeepSSMJob.h



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

#include <mutex>
#include <Job/Job.h>
#include <Project/Project.h>

#include <QObject>

namespace shapeworks {


class DeepSSMJob : public Job {
  Q_OBJECT;

 public:
  enum class JobType {
    DeepSSM_PrepType = 0,
    DeepSSM_AugmentationType = 1,
    DeepSSM_TrainingType = 2,
    DeepSSM_TestingType = 3
  };

  enum PrepStep {
    NOT_STARTED = 0,
    GROOM_TRAINING = 1,
    OPTIMIZE_TRAINING = 2,
    OPTIMIZE_VALIDATION = 3,
    GROOM_IMAGES = 4,
    DONE = 5
  };

  enum class SplitType { TRAIN, VAL, TEST };

  DeepSSMJob(std::shared_ptr<Project> project, DeepSSMJob::JobType tool_mode,
             DeepSSMJob::PrepStep prep_step = DeepSSMJob::NOT_STARTED);
  ~DeepSSMJob();

  void run() override;

  QString name() override;

  void run_prep();
  void run_augmentation();
  void run_training();
  void run_testing();

  void python_message(std::string str);

  static std::vector<int> get_split(ProjectHandle project, DeepSSMJob::SplitType split_type);

  void set_prep_step(DeepSSMJob::PrepStep step) {
    std::lock_guard<std::mutex> lock(mutex_);
    prep_step_ = step;
  }

 private:
  void update_prep_stage(DeepSSMJob::PrepStep step);
  void process_test_results();

  std::shared_ptr<Project> project_;

  DeepSSMJob::JobType job_type_;

  QString prep_message_;
  DeepSSMJob::PrepStep prep_step_{DeepSSMJob::NOT_STARTED};

  // mutex
  std::mutex mutex_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-09-26 at 16:33:40 +0000
