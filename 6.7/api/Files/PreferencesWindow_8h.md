---
title: Studio/Data/PreferencesWindow.h

---

# Studio/Data/PreferencesWindow.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PreferencesWindow](../Classes/classshapeworks_1_1PreferencesWindow.md)** <br>Qt UI dialog to control preferences.  |




## Source code

```cpp
#pragma once

#include <QDialog>

#include "Data/Preferences.h"

class Ui_PreferencesWindow;
class QAbstractButton;

namespace shapeworks {




class PreferencesWindow : public QDialog {
  Q_OBJECT

 public:
  PreferencesWindow(QWidget* parent, Preferences& prefs);

  void set_values_from_preferences();

  void closeEvent(QCloseEvent* event) override;

 public Q_SLOTS:
  void on_mesh_cache_enabled_stateChanged(int state);
  void on_mesh_cache_memory_valueChanged(int value);
  void on_color_scheme_currentIndexChanged(int index);
  void on_pca_range_valueChanged(double value);
  void on_pca_steps_valueChanged(int value);
  void on_parallel_enabled_toggled(bool b);
  void on_num_threads_valueChanged(int i);

  void accept() override;

  void restore_defaults();

  void save_to_preferences();

 Q_SIGNALS:
  void clear_cache();
  void update_view();
  void slider_update();

 private:
  void update_labels();
  Preferences& preferences_;
  Ui_PreferencesWindow* ui_;
};

}
```


-------------------------------

Updated on 2025-09-03 at 03:28:35 +0000
