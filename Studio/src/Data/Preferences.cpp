// qt
#include <Data/Preferences.h>

#include <QFile>
#include <QFileInfo>
#include <QSize>
#include <QThread>
#include <iostream>

//-----------------------------------------------------------------------------
Preferences::Preferences() : settings_("Scientific Computing and Imaging Institute", "ShapeWorksStudio") {
  settings_.setFallbacksEnabled(false);
}

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_geometry() {
  return settings_.value("Studio/window_geometry", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_geometry(QByteArray geometry) { settings_.setValue("Studio/window_geometry", geometry); }

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_state() {
  return settings_.value("Studio/window_state", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_state(QByteArray state) { settings_.setValue("Studio/window_state", state); }

//-----------------------------------------------------------------------------
QString Preferences::get_last_directory() { return settings_.value("Studio/last_directory", "").toString(); }

//-----------------------------------------------------------------------------
void Preferences::set_last_directory(QString value) { settings_.setValue("Studio/last_directory", value); }

//-----------------------------------------------------------------------------
bool Preferences::get_cache_enabled() { return settings_.value("Studio/cache_enabled", true).toBool(); }

//-----------------------------------------------------------------------------
void Preferences::set_cache_enabled(bool value) { settings_.setValue("Studio/cache_enabled", value); }

//-----------------------------------------------------------------------------
bool Preferences::get_parallel_enabled() { return settings_.value("Studio/parallel_enabled", true).toBool(); }

//-----------------------------------------------------------------------------
void Preferences::set_parallel_enabled(bool value) { settings_.setValue("Studio/parallel_enabled", value); }

//-----------------------------------------------------------------------------
int Preferences::get_memory_cache_percent() { return settings_.value("Studio/memory_cache_percent", 25).toInt(); }

//-----------------------------------------------------------------------------
void Preferences::set_memory_cache_percent(int value) { settings_.setValue("Studio/memory_cache_percent", value); }

//-----------------------------------------------------------------------------
int Preferences::get_num_threads() {
  return settings_.value("Studio/num_threads", QThread::idealThreadCount()).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_num_threads(int num_threads) { settings_.setValue("Studio/num_threads", num_threads); }

//-----------------------------------------------------------------------------
float Preferences::get_glyph_size() { return settings_.value("Project/glyph_size", 5.0).toFloat(); }

//-----------------------------------------------------------------------------
void Preferences::set_glyph_size(float value) { settings_.setValue("Project/glyph_size", value); }

//-----------------------------------------------------------------------------
float Preferences::get_glyph_quality() { return settings_.value("Project/glyph_quality", 10.0).toFloat(); }

//-----------------------------------------------------------------------------
void Preferences::set_glyph_quality(float value) { settings_.setValue("Project/glyph_quality", value); }

//-----------------------------------------------------------------------------
bool Preferences::get_glyph_auto_size() { return settings_.value("Project/glyph_auto_size", true).toBool(); }

//-----------------------------------------------------------------------------
void Preferences::set_glyph_auto_size(bool value) { settings_.setValue("Project/glyph_auto_size", value); }

//-----------------------------------------------------------------------------
float Preferences::get_pca_range() { return settings_.value("Analysis/pca_range", 2.0).toFloat(); }

//-----------------------------------------------------------------------------
void Preferences::set_pca_range(float value) { settings_.setValue("Analysis/pca_range", value); }

//-----------------------------------------------------------------------------
int Preferences::get_pca_steps() { return settings_.value("Analysis/pca_steps", 20).toInt(); }

//-----------------------------------------------------------------------------
void Preferences::set_pca_steps(int value) { settings_.setValue("Analysis/pca_steps", value); }

//-----------------------------------------------------------------------------
void Preferences::set_color_scheme(int value) { settings_.setValue("Studio/color_scheme", value); }

//-----------------------------------------------------------------------------
int Preferences::get_color_scheme() { return settings_.value("Studio/color_scheme", 0).toInt(); }

//-----------------------------------------------------------------------------
bool Preferences::get_center_checked() { return settings_.value("Studio/center_checked", true).toBool(); }

//-----------------------------------------------------------------------------
void Preferences::set_center_checked(bool value) { settings_.setValue("Studio/center_checked", value); }

//-----------------------------------------------------------------------------
bool Preferences::not_saved() { return !saved_; }

//-----------------------------------------------------------------------------
void Preferences::set_saved(bool saved) { saved_ = saved; }

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_files() {
  update_recent_files();
  return recent_files_;
}

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_paths() {
  update_recent_files();
  return recent_paths_;
}

//-----------------------------------------------------------------------------
void Preferences::add_recent_file(QString file, QString path) {
  QStringList files = get_recent_files();
  QStringList paths = get_recent_paths();

  while (paths.size() < files.size()) {
    paths << "";
  }

  // remove entry if it already exists
  while (files.indexOf(file) >= 0) {
    int idx = files.indexOf(file);
    files.removeAt(idx);
    paths.removeAt(idx);
  }
  files.prepend(file);
  paths.prepend(path);
  while (files.size() > Preferences::MAX_RECENT_FILES) {
    files.removeLast();
  }
  settings_.setValue("Main/recentFileListNew", files);

  while (paths.size() > Preferences::MAX_RECENT_FILES) {
    paths.removeLast();
  }
  settings_.setValue("Main/recentFilePaths", paths);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults() {
  // Don't reset recent files
  auto recent = get_recent_files();
  auto paths = get_recent_paths();
  settings_.clear();
  settings_.setValue("Main/recentFileListNew", recent);
  settings_.setValue("Main/recentFilePaths", paths);
}

//-----------------------------------------------------------------------------
Preferences::OrientationMarkerType Preferences::get_orientation_marker_type() {
  return static_cast<OrientationMarkerType>(
      settings_.value("Viewer/orientation_marker_type", OrientationMarkerType::medical).toInt());
}

//-----------------------------------------------------------------------------
void Preferences::set_orientation_marker_type(Preferences::OrientationMarkerType type) {
  settings_.setValue("Viewer/orientation_marker_type", type);
}

//-----------------------------------------------------------------------------
Preferences::OrientationMarkerCorner Preferences::get_orientation_marker_corner() {
  return static_cast<OrientationMarkerCorner>(
      settings_.value("Viewer/orientation_marker_corner", OrientationMarkerCorner::upper_right).toInt());
}

//-----------------------------------------------------------------------------
void Preferences::set_orientation_marker_corner(Preferences::OrientationMarkerCorner corner) {
  settings_.setValue("Viewer/orientation_marker_corner", corner);
}

//-----------------------------------------------------------------------------
QString Preferences::get_groom_file_template() {
  return settings_.value("Studio/groom_file_template", "groomed").toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_groom_file_template(QString groom_file_template) {
  settings_.setValue("Studio/groom_file_template", groom_file_template);
}

//-----------------------------------------------------------------------------
QString Preferences::get_optimize_file_template() {
  return settings_.value("Studio/optimize_file_template", "<project>_particles").toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_optimize_file_template(QString optimize_file_template) {
  settings_.setValue("Studio/optimize_file_template", optimize_file_template);
}

//-----------------------------------------------------------------------------
QSize Preferences::get_export_override_size() {
  return settings_.value("Export/override_size", QSize(1024, 1204)).toSize();
}

//-----------------------------------------------------------------------------
void Preferences::set_export_override_size(QSize size) { settings_.setValue("Export/override_size", size); }

//-----------------------------------------------------------------------------
bool Preferences::get_export_override_size_enabled() {
  return settings_.value("Export/override_size_enabled", false).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_export_override_size_enabled(bool enabled) {
  settings_.setValue("Export/override_size_enabled", enabled);
}

//-----------------------------------------------------------------------------
bool Preferences::get_export_show_orientation_marker() {
  return settings_.value("Export/show_orientation_marker", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_export_show_orientation_marker(bool value) {
  settings_.setValue("Export/show_orientation_marker", value);
}

//-----------------------------------------------------------------------------
bool Preferences::get_export_show_color_scale() { return settings_.value("Export/show_color_scale", true).toBool(); }

//-----------------------------------------------------------------------------
void Preferences::set_export_show_color_scale(bool value) { settings_.setValue("Export/show_color_scale", value); }

//-----------------------------------------------------------------------------
int Preferences::get_export_num_pca_images() { return settings_.value("Export/num_pca_images", 1).toInt(); }

//-----------------------------------------------------------------------------
void Preferences::set_export_num_pca_images(int number) { settings_.setValue("Export/num_pca_images", number); }

//-----------------------------------------------------------------------------
double Preferences::get_export_pca_range() { return settings_.value("Export/pca_range", 2.0).toDouble(); }

//-----------------------------------------------------------------------------
void Preferences::set_export_pca_range(double range) { settings_.setValue("Export/pca_range", range); }

//-----------------------------------------------------------------------------
int Preferences::get_geodesic_cache_multiplier() { return settings_.value("Mesh/geodesic_cache_multiplier").toInt(); }

//-----------------------------------------------------------------------------
void Preferences::set_geodesic_cache_multiplier(int value) {
  settings_.setValue("Mesh/geodesic_cache_multiplier", value);
}

//-----------------------------------------------------------------------------
void Preferences::update_recent_files() {
  auto recent_files = settings_.value("Main/recentFileListNew").toStringList();
  auto recent_paths = settings_.value("Main/recentFilePaths").toStringList();

  while (recent_paths.size() < recent_files.size()) {
    recent_paths << "";
  }

  QStringList existing_files;
  QStringList existing_paths;
  for (int i = 0; i < recent_files.size(); i++) {
    if (QFile::exists(recent_files[i])) {
      existing_files << recent_files[i];
      existing_paths << recent_paths[i];
    }
  }
  recent_files = existing_files;
  recent_paths = existing_paths;

  QStringList no_dupes;
  QStringList no_dupe_paths;
  for (int i = 0; i < recent_files.size(); i++) {
    bool found_dupe = false;
    for (int j = i + 1; j < recent_files.size(); j++) {
      if (QFileInfo(recent_files[i]).canonicalFilePath() == QFileInfo(recent_files[j]).canonicalFilePath()) {
        found_dupe = true;
      }
    }
    if (!found_dupe) {
      no_dupes << recent_files[i];
      no_dupe_paths << recent_paths[i];
    }
  }
  recent_files_ = no_dupes;
  recent_paths_ = no_dupe_paths;
}
