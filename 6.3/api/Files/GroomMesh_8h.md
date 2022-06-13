---
title: Studio/src/Groom/GroomMesh.h

---

# Studio/src/Groom/GroomMesh.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::GroomMesh](../Classes/classshapeworks_1_1GroomMesh.md)**  |




## Source code

```cpp
#pragma once

#include <string>

#include <QWidget>
#include <QSharedPointer>
#include <QProgressDialog>
#include <QElapsedTimer>
#include <QObject>

#include <Data/Preferences.h>
#include <Groom/QGroom.h>
#include <Libs/Groom/GroomParameters.h>

class Ui_GroomMesh;

Q_DECLARE_METATYPE(std::string)

namespace shapeworks {

class Session;
class ShapeWorksStudioApp;

class GroomMesh : public QWidget {
Q_OBJECT;
public:

  GroomMesh(Preferences& prefs);
  ~GroomMesh();

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
  void error_message(QString);
  void message(QString);
  void progress(int);

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged(int state);
  void on_blur_checkbox_stateChanged(int state);
  void on_autopad_checkbox_stateChanged(int state);
  void on_skip_button_clicked();
  void on_restore_defaults_clicked();

  void update_ui();

  void domain_changed();

  void alignment_checkbox_changed(int state);
  void alignment_option_changed(int index);

  void reflect_checkbox_changed(int state);
  void reflect_column_changed(int index);
  void reflect_choice_changed(int index);
  void reflect_axis_changed(int index);


  void fill_holes_changed(int state);

  void on_run_groom_button_clicked();

  void handle_thread_complete();
  void handle_progress(int val);
  void handle_error(QString msg);

private:

  void set_ui_from_params(GroomParameters params);

  void update_page();

  void update_reflect_columns();
  void update_reflect_choices();

  Preferences& preferences_;

  QList<QThread*> threads_;

  Ui_GroomMesh* ui_;
  QSharedPointer<Session> session_;

  QSharedPointer<shapeworks::QGroom> groom_;

  QElapsedTimer timer_;

  bool groom_is_running_ = false;

  std::string current_domain_;

  QStringList reflect_columns_;

};
}
```


-------------------------------

Updated on 2022-06-13 at 01:32:51 -0600
