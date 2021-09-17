#pragma once

#include <QObject>
#include <Libs/Project/Project.h>
#include <Job/Job.h>
#include <DeepSSM/DeepSSMTool.h>

namespace shapeworks {

//! Qt Wrapper for DeepSSM
/*!
 * The DeepSSMJob class wraps the functionality for DeepSSM as a Studio Job object
 *
 */
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
