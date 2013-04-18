/*
 * Shapeworks license
 */

/**
 * @file Preferences.h
 * @brief Houses and controls application preferences.
 *
 */

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QSettings>
#include <PreferencesWindow.h>

class Preferences : public QObject
{
  Q_OBJECT;

public:

  static Preferences& Instance();

  QSettings& getSettings();

  void showWindow();
  void closeWindow();

  bool getCacheEnabled();
  void setCacheEnabled( bool enabled );

  int getCacheMemory();
  void setCacheMemory( int value );

  int getColorScheme();
  void setColorScheme( int value );

  float getGlyphQuality();
  void setGlyphQuality( float value );

  float getGlyphSize();
  void setGlyphSize( float value );

  int getNumThreads();
  void setNumThreads( int value );

  bool getParallelEnabled();
  void setParallelEnabled( bool enabled );

  float getPcaRange();
  void setPcaRange( float value );

  int getNumPcaSteps();
  void setNumPcaSteps( int value );

  int getNumRegressionSteps();
  void setNumRegressionSteps( int value );

  void restoreDefaults();

Q_SIGNALS:
  void colorSchemeChanged( int newIndex );
  void glyphPropertiesChanged();
  void threadingChangedSignal();
  void slidersChangedSignal();

private:

  // private constructor
  Preferences();
  // Stop the compiler generating methods of copy the object
  Preferences( Preferences const& copy );            // Not Implemented
  Preferences& operator=( Preferences const& copy ); // Not Implemented
  static Preferences instance;

  QSettings settings;

  PreferencesWindow preferencesWindow;
};

#endif // ifndef PREFERENCES_H
