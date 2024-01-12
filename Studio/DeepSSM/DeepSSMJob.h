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
  enum class SplitType { TRAIN, VAL, TEST };

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

  std::vector<int> get_split(SplitType split_type);

  QString get_prep_message();

 private:
  void update_prep_message(PrepStep step);

  QSharedPointer<Session> session_;
  ProjectHandle project_;

  DeepSSMTool::ToolMode tool_mode_;

  QString prep_message_;
  PrepStep prep_step_{GROOM_TRAINING};

  // mutex
  std::mutex mutex_;
};
}  // namespace shapeworks
