---
title: Studio/Optimize/OptimizeTool.h

---

# Studio/Optimize/OptimizeTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::OptimizeTool](../Classes/classshapeworks_1_1OptimizeTool.md)**  |




## Source code

```cpp
#pragma once

#include <QSharedPointer>
#include <QWidget>
#include <QProgressDialog>
#include <QElapsedTimer>

#include <itkPoint.h>

#include <Data/Preferences.h>
#include <Data/Telemetry.h>

class Ui_OptimizeTool;

class QLineEdit;

namespace shapeworks {
class QOptimize;
class OptimizeParameters;
class Session;


class OptimizeTool : public QWidget {
Q_OBJECT;

public:

  OptimizeTool(Preferences& prefs, Telemetry& telemetry);
  ~OptimizeTool();

  void set_session(QSharedPointer<Session> session);

  void activate();

  void load_params();
  void store_params();

  void enable_actions();
  void disable_actions();

  void shutdown_threads();

public Q_SLOTS:

  void on_run_optimize_button_clicked();
  void on_restoreDefaults_clicked();
  void handle_optimize_complete();
  void handle_optimize_failed();
  void handle_progress(int val, QString message);
  void handle_error(QString);
  void handle_warning(QString);
  void handle_message(QString);

  void update_ui_elements();

  bool validate_inputs();

Q_SIGNALS:
  void optimize_start();
  void optimize_complete();

  void progress(int);
  void status(std::string);

private:

  void setup_domain_boxes();

  void update_run_button();

  void handle_load_progress(int count);

  std::vector<QLineEdit*> particle_boxes_;

  Preferences& preferences_;
  Telemetry& telemetry_;


  std::vector<QLineEdit*> line_edits_;

  QList<QThread*> threads_;
  bool optimization_is_running_ = false;
  QSharedPointer<QOptimize> optimize_;
  QSharedPointer<OptimizeParameters> optimize_parameters_;
  QSharedPointer<Session> session_;
  QElapsedTimer elapsed_timer_;

  Ui_OptimizeTool* ui_;
};
}
```


-------------------------------

Updated on 2024-01-27 at 17:49:28 +0000
