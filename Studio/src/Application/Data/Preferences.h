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
  void delete_entry(std::string entry);
  void add_recent_file(QString file);
  QStringList get_recent_files();
  bool not_saved();
  void set_saved(bool saved = true);

  bool has_entry(QString name);

  std::map<std::string, QVariant> get_project_preferences();

  template<typename T>
  T get_preference(std::string name, T default_val)
  {
    return this->settings_.value(QString::fromStdString(name),
                                 QVariant(default_val)).template value<T>();
  }

  template<typename T>
  void set_preference(std::string name, T value)
  {
    this->settings_.setValue(QString::fromStdString(name), QVariant(value));
    this->saved_ = false;
  }

  QByteArray get_window_geometry();
  void set_window_geometry(QByteArray geometry);

  QByteArray get_window_state();
  void set_window_state(QByteArray state);

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

  float get_glyph_size();
  void set_glyph_size(float value);

Q_SIGNALS:
  void color_scheme_changed(int newIndex);
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

private:

  QSettings settings_;
  bool saved_ = true;
};
