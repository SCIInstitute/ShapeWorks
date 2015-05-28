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

class Preferences : public QObject
{
  Q_OBJECT;

public:
  Preferences();

  QSettings& getSettings();
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

// default values
  const bool DEFAULT_CACHE_ENABLED;
  const bool DEFAULT_PARALLEL_ENABLED;
  const int DEFAULT_CACHE_MEMORY;
  const int DEFAULT_COLOR_SCHEME;
  const float DEFAULT_GLYPH_SIZE;
  const float DEFAULT_GLYPH_QUALITY;
  const float DEFAULT_NUM_THREADS;
  const float DEFAULT_PCA_RANGE;
  const int DEFAULT_PCA_STEPS;
  const int DEFAULT_REGRESSION_STEPS;
  QSettings settings;

};

#endif // ifndef PREFERENCES_H
