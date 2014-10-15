#ifndef STUDIO_APPLICATION_PREFERENCES_H
#define STUDIO_APPLICATION_PREFERENCES_H

#include <QSettings>
#include <QString>
#include <Application/PreferencesWindow.h>

//! Application preferences
/*!
 * The Preferences singleton controls all preferences for the application.
 * Values persist via the QSettings class.
 */
class Preferences : public QObject
{
  Q_OBJECT;

public:

  /// get the singleton instance
  static Preferences& Instance();

  /// show the PreferencesWindow
  void show_window();

  /// close the PreferencesWindow
  void close_window();

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

  bool get_cache_enabled();
  void set_cache_enabled( bool enabled );

  int get_cache_memory();
  void setCacheMemory( int value );

  int get_color_scheme();
  void set_color_scheme( int value );

  float get_glyph_quality();
  void set_glyph_quality( float value );

  float get_glyph_size();
  void set_glyph_size( float value );

  int get_num_threads();
  void set_num_threads( int value );

  bool get_parallel_enabled();
  void set_parallel_enabled( bool enabled );

  float get_pca_range();
  void set_pca_range( float value );

  int get_num_pca_steps();
  void set_num_pca_steps( int value );

  int get_num_regression_rteps();
  void set_num_regression_steps( int value );

  /// restore all default values
  void restore_defaults();

Q_SIGNALS:
  void color_scheme_changed( int newIndex );
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

private:

  /// private constructor
  Preferences();

  /// stop the compiler generating methods of copy the object
  Preferences( Preferences const& copy );            // not implemented
  Preferences& operator=( Preferences const& copy ); // not implemented

  /// singleton
  static Preferences instance;

  /// the QSettings object
  QSettings settings;

  /// the preferences user interface
  PreferencesWindow preferences_window;
};

#endif // ifndef STUDIO_APPLICATION_PREFERENCES_H
