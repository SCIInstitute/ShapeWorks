---
title: Studio/src/Data/Preferences.h

---

# Studio/src/Data/Preferences.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[Preferences](../Classes/classPreferences.md)** <br>Application preferences.  |




## Source code

```cpp
#pragma once

#include <QSettings>
#include <QString>


class Preferences : public QObject {
Q_OBJECT;

public:

  enum OrientationMarkerType {
    medical = 0,
    triad = 1,
    none = 2
  };

  enum OrientationMarkerCorner {
    upper_right = 0,
    lower_right = 1,
    lower_left = 2,
    upper_left = 3
  };

  Preferences();

  enum { MAX_RECENT_FILES = 64 };

  void restore_defaults();

  void add_recent_file(QString file, QString path);
  QStringList get_recent_files();
  QStringList get_recent_paths();

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

  int get_memory_cache_percent();
  void set_memory_cache_percent(int value);

  int get_num_threads();
  void set_num_threads(int num_threads);

  float get_cache_epsilon();
  void set_cache_epsilon(float value);

  float get_glyph_size();
  void set_glyph_size(float value);

  float get_glyph_quality();
  void set_glyph_quality(float value);

  bool get_glyph_auto_size();
  void set_glyph_auto_size(bool value);

  float get_pca_range();
  void set_pca_range(float value);

  int get_pca_steps();
  void set_pca_steps(int value);

  void set_color_scheme(int value);
  int get_color_scheme();

  bool get_center_checked();
  void set_center_checked(bool value);

  int get_geodesic_cache_multiplier();
  void set_geodesic_cache_multiplier(int value);

  OrientationMarkerType get_orientation_marker_type();
  void set_orientation_marker_type(OrientationMarkerType type);

  OrientationMarkerCorner get_orientation_marker_corner();
  void set_orientation_marker_corner(OrientationMarkerCorner corner);

  QString get_groom_file_template();
  void set_groom_file_template(QString groom_file_template);

  QString get_optimize_file_template();
  void set_optimize_file_template(QString optimize_file_template);

Q_SIGNALS:

  void color_scheme_changed(int newIndex);
  void glyph_properties_changed();
  void threading_changed_signal();
  void sliders_changed_signal();

private:

  void update_recent_files();
  QStringList recent_files_;
  QStringList recent_paths_;

  QSettings settings_;
  bool saved_ = true;
};
```


-------------------------------

Updated on 2022-03-03 at 07:50:37 +0000
