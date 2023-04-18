---
title: Studio/Groom/GroomTool.h

---

# Studio/Groom/GroomTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::GroomTool](../Classes/classshapeworks_1_1GroomTool.md)**  |




## Source code

```cpp
#pragma once

#include <Data/Preferences.h>
#include <Groom/Groom.h>
#include <Groom/GroomParameters.h>
#include <Data/Telemetry.h>

#include <QElapsedTimer>
#include <QObject>
#include <QProgressDialog>
#include <QSharedPointer>
#include <QWidget>
#include <string>

class Ui_GroomTool;

Q_DECLARE_METATYPE(std::string)

namespace shapeworks {

class Session;
class ShapeWorksStudioApp;

class GroomTool : public QWidget {
  Q_OBJECT;

 public:
  GroomTool(Preferences& prefs, Telemetry& telemetry);
  ~GroomTool();

  void set_session(QSharedPointer<Session> session);

  void activate();

  void load_params();
  void store_params();

  void disable_actions();
  void enable_actions();

  void shutdown_threads();

 Q_SIGNALS:
  void groom_start();
  void groom_complete();
  void progress(int);

 public Q_SLOTS:

  void on_antialias_checkbox_stateChanged(int state);
  void on_blur_checkbox_stateChanged(int state);
  void on_autopad_checkbox_stateChanged(int state);
  void skip_grooming_toggled();
  void on_restore_defaults_clicked();

  void update_ui();

  void domain_changed();

  void alignment_checkbox_changed(int state);
  void alignment_option_changed(int index);

  void reflect_checkbox_changed(int state);
  void reflect_column_changed(int index);
  void reflect_choice_changed(int index);
  void reflect_axis_changed(int index);

  void on_run_groom_button_clicked();

  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error(QString msg);

 private:
  void set_ui_from_params(GroomParameters params);

  void update_page();
  void update_domain_box();
  void apply_to_all_domains_changed();

  void update_reflect_columns();
  void update_reflect_choices();

  Preferences& preferences_;
  Telemetry& telemetry_;

  QList<QThread*> threads_;

  Ui_GroomTool* ui_;
  QSharedPointer<Session> session_;

  QSharedPointer<shapeworks::Groom> groom_;

  QElapsedTimer timer_;

  bool groom_is_running_ = false;

  std::string current_domain_;

  QStringList reflect_columns_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-04-18 at 16:27:17 +0000
