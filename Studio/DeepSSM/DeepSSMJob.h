#pragma once

#include <DeepSSM/DeepSSMTool.h>
#include <Job/Job.h>
#include <Project/Project.h>

#include <QObject>

namespace shapeworks {

//! Qt Wrapper for DeepSSM
/*!
 * The DeepSSMJob class wraps the functionality for DeepSSM as a Studio Job object
 *
 */
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
