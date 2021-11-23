---
title: Studio/src/Data/PreferencesWindow.h

---

# Studio/src/Data/PreferencesWindow.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[PreferencesWindow](../Classes/classPreferencesWindow.md)** <br>Qt UI dialog to control preferences.  |




## Source code

```cpp
#pragma once

#include <QDialog>
#include "Data/Preferences.h"

class Ui_PreferencesWindow;
class QAbstractButton;


class PreferencesWindow : public QDialog {
Q_OBJECT

public:
  PreferencesWindow(QWidget* parent, Preferences& prefs);

  void set_values_from_preferences();

  void closeEvent(QCloseEvent *event) override;

public Q_SLOTS:
  void on_mesh_cache_enabled_stateChanged(int state);
  void on_mesh_cache_memory_valueChanged(int value);
  void on_color_scheme_currentIndexChanged(int index);
  void on_pca_range_valueChanged(double value);
  void on_pca_steps_valueChanged(int value);
  void on_parallel_enabled_toggled(bool b);
  void on_num_threads_valueChanged(int i);
  void on_caching_epsilon_valueChanged(int i);

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
```


-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000
