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
  void restore_defaults();
  void add_recent_file(QString file);
  QStringList get_recent_files();
  bool not_saved();
  void set_saved();
  std::map<std::string, QVariant> getAllPreferences();
  double get_preference(std::string name, double default_val);
  int get_preference(std::string name, int default_val);
  QString get_preference(std::string name, QString default_val);

  template<typename T>
  void set_preference(std::string name, T value) {
    this->settings_.setValue(QString::fromStdString(name), value);
    if (!this->defaults_.count(name)) {
      this->defaults_.insert(std::make_pair(name, value));
    } else {
      auto entry = this->defaults_.find(name);
      entry->second = value;
    }
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
  std::map<std::string, QVariant> defaults_;
  bool saved_;
};

#endif // ifndef STUDIO_APPLICATION_PREFERENCES_H
