/*
 * Shapeworks license
 */

/**
 * @file PreferencesWindow.h
 * @brief Qt UI dialog to control preferences
 *
 */

#ifndef PREFERENCES_WINDOW_H
#define PREFERENCES_WINDOW_H

#include <QDialog>
#include <Preferences.h>

class Ui_PreferencesWindow;
class QAbstractButton;

class PreferencesWindow : public QDialog
{
  Q_OBJECT

public:
  PreferencesWindow(Preferences& prefs, QWidget* parent = 0 );

  void setValuesFromPreferences();

public Q_SLOTS:
  void close_window();
  void show_window();
  void on_meshCacheEnabled_stateChanged( int state );
  void on_meshCacheMemory_valueChanged( int value );
  void on_colorScheme_currentIndexChanged( int index );
  void on_glyphSize_valueChanged( int value );
  void on_glyphQuality_valueChanged( int value );
  void on_numThreadsSlider_valueChanged( int value );
  void on_parallelEnabled_stateChanged( int state );

  void on_pcaRangeSpinBox_valueChanged( double value );
  void on_pcaStepsSpinBox_valueChanged( int value );
  void on_regressionStepsSpinBox_valueChanged( int value );

  void restoreDefaults();

signals:
  void clear_cache();
  void update_view();
  void update_pca();

private:
  void updateLabels();
  Preferences &prefs_;
  Ui_PreferencesWindow* ui;
};

#endif // ifndef PREFERENCES_WINDOW_H
