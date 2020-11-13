#pragma once

#include <QDialog>
#include "Data/Preferences.h"

class Ui_PreferencesWindow;
class QAbstractButton;

//! Qt UI dialog to control preferences
/*!
 * The PreferenceWindow provides controls over preferences for the
 * application.  It is synchronized with the singleton Preference object
 */
class PreferencesWindow : public QDialog {
Q_OBJECT

public:
  PreferencesWindow(QWidget* parent, Preferences& prefs);

  void set_values_from_preferences();

public Q_SLOTS:
  void on_mesh_cache_enabled_stateChanged(int state);
  void on_mesh_cache_memory_valueChanged(int value);
  void on_color_scheme_currentIndexChanged(int index);
  void on_pca_range_valueChanged(double value);
  void on_pca_steps_valueChanged(int value);
  void on_parallel_enabled_toggled(bool b);
  void on_num_threads_valueChanged(int i);
  void on_caching_epsilon_valueChanged(int i);

  void restore_defaults();

Q_SIGNALS:
  void clear_cache();
  void update_view();
  void slider_update();

private:
  Preferences& preferences_;
  Ui_PreferencesWindow* ui_;
};
