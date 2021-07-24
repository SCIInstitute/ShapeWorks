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
  void run();

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

  void initialize_python();
  void finalize_python();
  bool python_initialized_ = false;

};
}
