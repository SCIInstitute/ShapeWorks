// qt
#include <QThread>
#include <QSize>
#include <QFile>
#include <QFileInfo>

#include <Data/Preferences.h>

#include <iostream>

//-----------------------------------------------------------------------------
Preferences::Preferences()
  : settings_("Scientific Computing and Imaging Institute", "ShapeWorksStudio")
{
  this->settings_.setFallbacksEnabled(false);
}

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_geometry()
{
  return this->settings_.value("Studio/window_geometry", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_geometry(QByteArray geometry)
{
  this->settings_.setValue("Studio/window_geometry", geometry);
}

//-----------------------------------------------------------------------------
QByteArray Preferences::get_window_state()
{
  return this->settings_.value("Studio/window_state", QByteArray()).toByteArray();
}

//-----------------------------------------------------------------------------
void Preferences::set_window_state(QByteArray state)
{
  this->settings_.setValue("Studio/window_state", state);
}

//-----------------------------------------------------------------------------
QString Preferences::get_last_directory()
{
  return this->settings_.value("Studio/last_directory", "").toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_last_directory(QString value)
{
  this->settings_.setValue("Studio/last_directory", value);
}

//-----------------------------------------------------------------------------
bool Preferences::get_cache_enabled()
{
  return this->settings_.value("Studio/cache_enabled", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_cache_enabled(bool value)
{
  this->settings_.setValue("Studio/cache_enabled", value);
}

//-----------------------------------------------------------------------------
bool Preferences::get_parallel_enabled()
{
  return this->settings_.value("Studio/parallel_enabled", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_parallel_enabled(bool value)
{
  this->settings_.setValue("Studio/parallel_enabled", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_memory_cache_percent()
{
  return this->settings_.value("Studio/memory_cache_percent", 25).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_memory_cache_percent(int value)
{
  this->settings_.setValue("Studio/memory_cache_percent", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_num_threads()
{
  return this->settings_.value("Studio/num_threads", QThread::idealThreadCount()).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_num_threads(int num_threads)
{
  this->settings_.setValue("Studio/num_threads", num_threads);
}

//-----------------------------------------------------------------------------
float Preferences::get_cache_epsilon()
{
  return this->settings_.value("Studio/cache_epsilon", 1e-3f).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_cache_epsilon(float value)
{
  this->settings_.setValue("Studio/cache_epsilon", value);
}

//-----------------------------------------------------------------------------
float Preferences::get_glyph_size()
{
  return this->settings_.value("Project/glyph_size", 5.0).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_size(float value)
{
  this->settings_.setValue("Project/glyph_size", value);
}

//-----------------------------------------------------------------------------
float Preferences::get_glyph_quality()
{
  return this->settings_.value("Project/glyph_quality", 10.0).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_quality(float value)
{
  this->settings_.setValue("Project/glyph_quality", value);
}

//-----------------------------------------------------------------------------
bool Preferences::get_glyph_auto_size()
{
  return this->settings_.value("Project/glyph_auto_size", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_auto_size(bool value)
{
  this->settings_.setValue("Project/glyph_auto_size", value);
}

//-----------------------------------------------------------------------------
float Preferences::get_pca_range()
{
  return this->settings_.value("Analysis/pca_range", 2.0).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_pca_range(float value)
{
  this->settings_.setValue("Analysis/pca_range", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_pca_steps()
{
  return this->settings_.value("Analysis/pca_steps", 20).toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_pca_steps(int value)
{
  this->settings_.setValue("Analysis/pca_steps", value);
}

//-----------------------------------------------------------------------------
void Preferences::set_color_scheme(int value)
{
  this->settings_.setValue("Studio/color_scheme", value);
}

//-----------------------------------------------------------------------------
int Preferences::get_color_scheme()
{
  return this->settings_.value("Studio/color_scheme", 0).toInt();
}

//-----------------------------------------------------------------------------
bool Preferences::get_center_checked()
{
  return this->settings_.value("Studio/center_checked", true).toBool();
}

//-----------------------------------------------------------------------------
void Preferences::set_center_checked(bool value)
{
  this->settings_.setValue("Studio/center_checked", value);
}

//-----------------------------------------------------------------------------
bool Preferences::not_saved()
{
  return !this->saved_;
}

//-----------------------------------------------------------------------------
void Preferences::set_saved(bool saved)
{
  this->saved_ = saved;
}

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_files()
{
  this->update_recent_files();
  return this->recent_files_;
}

//-----------------------------------------------------------------------------
QStringList Preferences::get_recent_paths()
{
  this->update_recent_files();
  return this->recent_paths_;
}

//-----------------------------------------------------------------------------
void Preferences::add_recent_file(QString file, QString path)
{
  QStringList files = this->get_recent_files();
  QStringList paths = this->get_recent_paths();

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
  this->settings_.setValue("Main/recentFileListNew", files);

  while (paths.size() > Preferences::MAX_RECENT_FILES) {
    paths.removeLast();
  }
  this->settings_.setValue("Main/recentFilePaths", paths);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults()
{
  // Don't reset recent files
  auto recent = this->get_recent_files();
  auto paths = this->get_recent_paths();
  this->settings_.clear();
  this->settings_.setValue("Main/recentFileListNew", recent);
  this->settings_.setValue("Main/recentFilePaths", paths);
}

//-----------------------------------------------------------------------------
Preferences::OrientationMarkerType Preferences::get_orientation_marker_type()
{
  return static_cast<OrientationMarkerType>(
    this->settings_.value("Viewer/orientation_marker_type",
                          OrientationMarkerType::medical).toInt());
}

//-----------------------------------------------------------------------------
void Preferences::set_orientation_marker_type(Preferences::OrientationMarkerType type)
{
  this->settings_.setValue("Viewer/orientation_marker_type", type);
}

//-----------------------------------------------------------------------------
Preferences::OrientationMarkerCorner Preferences::get_orientation_marker_corner()
{
  return static_cast<OrientationMarkerCorner>(this->settings_.value(
                                                "Viewer/orientation_marker_corner",
                                                OrientationMarkerCorner::upper_right).toInt());
}

//-----------------------------------------------------------------------------
void Preferences::set_orientation_marker_corner(Preferences::OrientationMarkerCorner corner)
{
  this->settings_.setValue("Viewer/orientation_marker_corner", corner);
}

//-----------------------------------------------------------------------------
QString Preferences::get_groom_file_template()
{
  return this->settings_.value("Studio/groom_file_template", "groomed").toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_groom_file_template(QString groom_file_template)
{
  this->settings_.setValue("Studio/groom_file_template", groom_file_template);
}

//-----------------------------------------------------------------------------
QString Preferences::get_optimize_file_template()
{
  return this->settings_.value("Studio/optimize_file_template", "<project>_particles").toString();
}

//-----------------------------------------------------------------------------
void Preferences::set_optimize_file_template(QString optimize_file_template)
{
  this->settings_.setValue("Studio/optimize_file_template", optimize_file_template);
}

//-----------------------------------------------------------------------------
int Preferences::get_geodesic_cache_multiplier()
{
  return this->settings_.value("Mesh/geodesic_cache_multiplier").toInt();
}

//-----------------------------------------------------------------------------
void Preferences::set_geodesic_cache_multiplier(int value)
{
  this->settings_.setValue("Mesh/geodesic_cache_multiplier", value);
}

//-----------------------------------------------------------------------------
void Preferences::update_recent_files()
{
  auto recent_files = this->settings_.value("Main/recentFileListNew").toStringList();
  auto recent_paths = this->settings_.value("Main/recentFilePaths").toStringList();

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
      if (QFileInfo(recent_files[i]).canonicalFilePath() ==
          QFileInfo(recent_files[j]).canonicalFilePath()) {
        found_dupe = true;
      }
    }
    if (!found_dupe) {
      no_dupes << recent_files[i];
      no_dupe_paths << recent_paths[i];
    }
  }
  this->recent_files_ = no_dupes;
  this->recent_paths_ = no_dupe_paths;
}
