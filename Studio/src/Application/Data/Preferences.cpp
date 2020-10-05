// qt
#include <QThread>
#include <QSize>
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
  return this->settings_.value("Project/glyph_quality", 5.0).toFloat();
}

//-----------------------------------------------------------------------------
void Preferences::set_glyph_quality(float value)
{
  this->settings_.setValue("Project/glyph_quality", value);
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
  return this->settings_.value("Main/recentFileListNew").toStringList();
}

//-----------------------------------------------------------------------------
void Preferences::add_recent_file(QString file)
{
  QStringList files = this->get_recent_files();
  files.removeAll(file);
  files.prepend(file);
  while (files.size() > Preferences::MAX_RECENT_FILES) {
    files.removeLast();
  }

  this->settings_.setValue("Main/recentFileListNew", files);
}

//-----------------------------------------------------------------------------
void Preferences::restore_defaults()
{
  this->settings_.clear();
  return;
}
