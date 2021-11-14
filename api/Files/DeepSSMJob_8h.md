---
title: Studio/src/DeepSSM/DeepSSMJob.h

---

# Studio/src/DeepSSM/DeepSSMJob.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DeepSSMJob](../Classes/classshapeworks_1_1DeepSSMJob.md)** <br>Qt Wrapper for DeepSSM.  |




## Source code

```cpp
#pragma once

#include <QObject>
#include <Libs/Project/Project.h>
#include <Job/Job.h>
#include <DeepSSM/DeepSSMTool.h>

namespace shapeworks {


class DeepSSMJob : public Job {

  Q_OBJECT;

public:

  enum class FileType {
    ID,
    IMAGE,
    PARTICLES
  };

  enum class SplitType {
    TRAIN,
    TEST
  };

  DeepSSMJob(ProjectHandle project, DeepSSMTool::ToolMode tool_mode);
  ~DeepSSMJob();

  void run() override;

  QString name() override;

  void run_augmentation();
  void run_training();
  void run_testing();

  void python_message(std::string str);

  std::vector<std::string> get_list(FileType file_type, SplitType split_type);

private:
  ProjectHandle project_;

  DeepSSMTool::ToolMode tool_mode_;
};
}
```


-------------------------------

Updated on 2021-11-14 at 17:52:11 +0000
