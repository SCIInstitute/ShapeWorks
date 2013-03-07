#ifndef PREFERENCES_WINDOW_H
#define PREFERENCES_WINDOW_H

#include <QDialog>

class Ui_PreferencesWindow;
class QAbstractButton;

class PreferencesWindow : public QDialog
{
  Q_OBJECT

public:
  PreferencesWindow( QWidget* parent = 0 );

public Q_SLOTS:
  void on_modelCacheEnabled_stateChanged( int state );
  void on_modelCacheMemory_valueChanged( int value );
  void on_colorScheme_currentIndexChanged( int index );
  void on_glyphSize_valueChanged( int value );
  void on_glyphQuality_valueChanged( int value );

  void restoreDefaults();

private:
  void setValuesFromPreferences();
  void updateLabels();
  Ui_PreferencesWindow* ui;
};

#endif // ifndef PREFERENCES_WINDOW_H
