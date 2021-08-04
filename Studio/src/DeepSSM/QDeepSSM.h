#pragma once

#include <QObject>
#include <Libs/Project/Project.h>

namespace shapeworks {

//! Qt Wrapper for DeepSSM
/*!
 * The QDeepSSM class wraps the functionality for DeepSSM as a QObject
 *
 */
class QDeepSSM : public QObject {

Q_OBJECT;

public:

  QDeepSSM(ProjectHandle project);
  ~QDeepSSM();
  void run_augmentation();
  void run_training();
  void run_testing();

  void python_message(std::string str);

protected:
  // override update_progress to emit q_signal
  void update_progress();

Q_SIGNALS:
  void progress(int);
  void message(QString);
  void error(QString);

private:
  ProjectHandle project_;

  enum class FileType {
    ID,
    IMAGE,
    PARTICLES
  };

  enum class SplitType {
    TRAIN,
    TEST
  };

  std::vector<std::string> get_list(FileType file_type, SplitType split_type);

};
}
