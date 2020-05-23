#pragma once

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

  enum { MAX_RECENT_FILES = 4 };
  Preferences();
  void restore_defaults(bool force = false);

  void add_recent_file(QString file);
  QStringList get_recent_files();
  bool not_saved();
  void set_saved(bool saved = true);



  QByteArray get_window_geometry();
  void set_window_geometry(QByteArray geometry);

  QByteArray get_window_state();
  void set_window_state(QByteArray state);

  QString get_last_directory();
  void set_last_directory(QString value);

  bool get_cache_enabled();
  void set_cache_enabled(bool value);

  bool get_parallel_enabled();
  void set_parallel_enabled(bool value);

  //! default 25 percent
  int get_memory_cache_percent();
  //! 0-100
  void set_memory_cache_percent(int value);

  int get_num_threads();
  void set_num_threads(int num_threads);

  float get_cache_epsilon();
  void set_cache_epsilon(float value);


  float get_glyph_size();
  void set_glyph_size(float value);

  float get_glyph_quality();
  void set_glyph_quality(float value);

  float get_pca_range();
  void set_pca_range(float value);

  int get_pca_steps();
  void set_pca_steps(int value);


  void set_color_scheme(int value);
  int get_color_scheme();

  bool get_center_checked();
  void set_center_checked(bool value);

Q_SIGNALS:

  void color_scheme_changed(int newIndex);
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

private:

  QSettings settings_;
  bool saved_ = true;
};
