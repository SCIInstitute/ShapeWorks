#ifndef STUDIO_APPLICATION_PREFERENCES_H
#define STUDIO_APPLICATION_PREFERENCES_H

#include <QSettings>
#include <QString>

//! Application preferences
/*!
 * The Preferences singleton controls all preferences for the application.
 * Values persist via the QSettings class.
 */
class Preferences : public QObject
{
  Q_OBJECT;

public:

  //--- general preferences

  /// Main window size
  QSize get_main_window_size();
  void set_main_window_size( QSize size );

  /// ShapeWorksGroom executable location
  QString get_groom_location();
  void set_groom_location( QString location );

  /// ShapeWorksRun executable location
  QString get_optimize_location();
  void set_optimize_location( QString location );

  /// last directory used (open/save)
  QString get_last_directory();
  void set_last_directory( QString location );

  //--- analysis preferences

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

  /// restore all default values
  void restoreDefaults();

  int getSmoothingAmount();
  void setSmoothingAmount(int value);

  float getCacheEpsilon();
  void setCacheEpsilon(float value);

  float getSpacing();
  void setSpacing(float value);

  int getNeighborhood();
  void setNeighborhood(int value);

Q_SIGNALS:
  void color_scheme_changed( int newIndex );
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

public:

  enum { MAX_RECENT_FILES = 4 };
  Preferences();

private:
  /// the QSettings object
  QSettings settings;

};

#endif // ifndef STUDIO_APPLICATION_PREFERENCES_H
