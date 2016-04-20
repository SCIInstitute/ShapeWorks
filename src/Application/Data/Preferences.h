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

  enum { MAX_RECENT_FILES = 4 };
  Preferences();
  void restore_defaults(bool force = false);
  void add_recent_file(QString file);
  QStringList get_recent_files();
  bool not_saved();
  void set_saved();
  std::map<std::string, QVariant> getAllPreferences();
  template<typename T>
  T get_preference(std::string name, T default_val) {
    return this->settings_.value(QString::fromStdString(name), 
      QVariant(default_val)).template value<T>();
  }

  template<typename T>
  void set_preference(std::string name, T value) {
    this->settings_.setValue(QString::fromStdString(name), QVariant(value));
    this->saved_ = false;
  }

Q_SIGNALS:
  void color_scheme_changed( int newIndex );
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

private:
  /// the QSettings object
  QSettings settings_;
  bool saved_;
};

#endif // ifndef STUDIO_APPLICATION_PREFERENCES_H
